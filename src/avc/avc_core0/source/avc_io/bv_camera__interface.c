

#include "fsl_ov7670.h"
#include "fsl_ov5640.h"
#include "fsl_lpi2c.h"
#include "fsl_lpflexcomm.h"
#include "fsl_smartdma.h"
#include "fsl_inputmux.h"
#include "board.h"
#include "bunny_build.h"
#include "cr_section_macros.h"
#include "pin_mux.h"
#include "bv_camera__interface.h"
#include "avc__master_config.h"

#define EZH_STACK_SIZE 64   /* stack size for EZH, see smart_dma driver recommendation */


void camera__i2c_init();

status_t camera__i2c_receiveSCCB(uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);

status_t camera__i2c_sendSCCB(uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *txBuff, uint8_t txBuffSize);

volatile uint8_t img_ready = 0;   /* non-zero when new data signaled by SmartDMA IRQ */

static volatile uint8_t g_samrtdma_stack[EZH_STACK_SIZE];

uint16_t g_camera_buffer[320 *240];


__BSS(EZH) volatile uint32_t ezh_binary[512];


uint16_t * camera__get_buffer()
{
	return g_camera_buffer;
}

void camera__pull_reset_pin(bool pullUp)
{
    if (pullUp)
        GPIO_PortSet(BOARD_INITCAMERAPINS_CAM_RST_GPIO, BOARD_INITCAMERAPINS_CAM_RST_GPIO_PIN_MASK);
    else
        GPIO_PortClear(BOARD_INITCAMERAPINS_CAM_RST_GPIO, BOARD_INITCAMERAPINS_CAM_RST_GPIO_PIN_MASK);
}

void camera__pull_power_pin(bool pullUp)
{
    if (pullUp)
        GPIO_PortSet(BOARD_INITCAMERAPINS_CAM_PDWN_GPIO, BOARD_INITCAMERAPINS_CAM_PDWN_GPIO_PIN_MASK);
    else
        GPIO_PortClear(BOARD_INITCAMERAPINS_CAM_PDWN_GPIO, BOARD_INITCAMERAPINS_CAM_PDWN_GPIO_PIN_MASK);
}



#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
/* Camera resource and handle */

	const ov7670_resource_t resource = {
		.i2cSendFunc    = BOARD_Camera_I2C_SendSCCB,
		.i2cReceiveFunc = BOARD_Camera_I2C_ReceiveSCCB,
		.xclock         = kOV7670_InputClock12MHZ,
	};

#endif



#if CONFIG__CAMERA_SELECT == CAMERA__OV5640

	const ov5640_resource_t resource = {
		 .i2cSendFunc    = BOARD_Camera_I2C_SendSCCB,
		 .i2cReceiveFunc = BOARD_Camera_I2C_ReceiveSCCB,
		.pullResetPin = camera__pull_reset_pin,      /*!< Function to pull reset pin high or low. */
		.pullPowerDownPin = camera__pull_power_pin,  /*!< Function to pull the power down pin high or low. */
	};

#endif



//this is from  with AN14191SW
void  EZH_Camera_320240_Whole_Buf(void)
{
	E_NOP;/*pattern*/
	E_NOP;/*pattern*/
	E_NOP;/*pattern*/
	E_NOP;/*pattern*/


	E_PER_READ(R6, ARM2EZH);
	//Align the value of R6 to word
	E_LSR(R6, R6, 2);
	E_LSL(R6, R6, 2);
	E_LDR(SP, R6, 0);//EZH stack initial
	E_LDR(R3, R6, 1);//R3 point to the store buffer in the RAM

	E_LABEL("init_0");
	E_LDR(CFS, PC, 1); // Load CFS
	E_LDR(CFM, PC, 1); // Load CFM
	E_ADD_IMM(PC, PC, 1 * 4); //E_goto
	E_DCD_VAL(BS7(0) | BS6(0) | BS5(0) | BS4(0) | BS3(0) | BS2(1) | BS1(0) | BS0(2));   // Config source (C^D + C^D-)  where C^ is Clock rise, D- is Data inverted
	E_DCD_VAL(BS7(6) | BS6(6) | BS5(6) | BS4(6) | BS3(6) | BS2(BS_FALL) | BS1(BS_FALL) | BS0(BS_FALL) | (1 << 2)); // Config MUX    (C^D + C^D-)  where C^ is Clock rise, D- is Data inverted, enable OR

	E_BCLR_IMM(GPD, GPD, 0);
	E_BCLR_IMM(GPD, GPD, 1);
	E_BCLR_IMM(GPD, GPD, 2);
	E_BCLR_IMM(GPD, GPD, 3);
	E_BCLR_IMM(GPD, GPD, 4);
	E_BCLR_IMM(GPD, GPD, 5);
	E_BCLR_IMM(GPD, GPD, 6);
	E_BCLR_IMM(GPD, GPD, 7);
	E_BSET_IMM(GPD, GPD, 13);


	E_LOAD_IMM(R1, 0xFF);
	E_LABEL("PCLK_0");
	E_ACC_VECTORED_HOLD_LV(PC, (1 << 0));
	E_STRB_POST(R3, GPI, 1);  				// data will be stored from GPI bottom byte to RAM
	E_BSET_IMM(CFM, CFM, 0); 				//clear the vector flag

	E_SUB_IMMS(PC, PC, 4 * 5);//BS0
	E_BSET_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
	E_BCLR_IMM(GPO, GPO, 13);
	E_NOP;

	E_GOSUB("VSYNC_0");//BS1
	E_NOP;
	E_NOP;
	E_NOP;

	E_GOSUB("HSYNC_0");//BS2
	E_NOP;
	E_NOP;
	E_NOP;


	E_LABEL("VSYNC_0");
	E_BSET_IMM(CFM, CFM, 0); 			//clear the VSYNC vector flag
	E_BSET_IMM(CFM, CFM, 1); 			//enable HSYNC interrupt
	E_BSET_IMM(CFM, CFM, 2); 			//enable PCLK  interrupt
	E_LDR(R3, R6, 1);							//R3 point to the store buffer in the RAM

//#E_BTOG_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timing in logic device
	E_INT_TRIGGER(0x11); // interrupt and told ARM data is ready

	E_GOSUB("PCLK_0");

	E_LABEL("HSYNC_0");
	E_BSET_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
	E_BCLR_IMM(GPO, GPO, 13);
	E_BCLR_IMM(R3, R3, 0);
	E_BCLR_IMM(R3, R3, 1);
	E_BSET_IMM(CFM, CFM, 0); 			//clear the vector flag
	E_BSET_IMM(CFM, CFM, 1); 			//clear the vector flag
	//E_BTOG_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
	E_GOSUB("PCLK_0");
}


static void ezh_camera_callback(void *param)
{
	img_ready++;
}



camera_device_handle_t handle =
{
    .resource = (void *)&resource,

	#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
		.ops      = &ov7670_ops,
	#endif

	#if CONFIG__CAMERA_SELECT == CAMERA__OV5640
		.ops      = &ov5640_ops
	#endif

};

//This needs to be global
smartdma_camera_param_t smartdmaParam;

uint8_t bv_camera__init()
{

	DEBUG("Build EZH Camera application\r\n");

	uint32_t words_assembled = bunny_build((uint32_t* )&ezh_binary[0],
							   sizeof(ezh_binary),
							   EZH_Camera_320240_Whole_Buf);

	DEBUG(VT100_YELLOW"%d "VT100_DEFAULT" 32bit words assembled (%d bytes) \r\n"VT100_DEFAULT,words_assembled, words_assembled<<2);


#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
    CLOCK_AttachClk(kFRO12M_to_CLKOUT);
    CLOCK_SetClkDiv(kCLOCK_DivClkOut, 1U);
#else

    /*6MHz For OV5640*/

    CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);

    CLOCK_SetClkDiv(kCLOCK_DivClkOut, 11U); //320x102

    //CLOCK_SetClkDiv(kCLOCK_DivClkOut, 6U); //320x120
    //CLOCK_SetClkDiv(kCLOCK_DivClkOut, 7U); //160x120  <<see if we can get pll working
    // CLOCK_SetClkDiv(kCLOCK_DivClkOut, 23U); //320x240


#endif

    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7);
    CLOCK_EnableClock(kCLOCK_LPFlexComm7);
    CLOCK_EnableClock(kCLOCK_LPI2c7);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom7Clk, 1u);

    camera__i2c_init();




#if (CONFIG__CAMERA_SELECT == CAMERA__OV7670)


        /* Init ov7670 module with default setting. */
        camera_config_t camconfig = {
            .pixelFormat                = kVIDEO_PixelFormatRGB565,
            .resolution                 = kVIDEO_ResolutionQVGA,
            .framePerSec                = 30,
            .interface                  = kCAMERA_InterfaceGatedClock,
            .frameBufferLinePitch_Bytes = 0, /* Not used. */
            .controlFlags               = 0, /* Not used. */
            .bytesPerPixel              = 0, /* Not used. */
            .mipiChannel                = 0, /* Not used. */
            .csiLanes                   = 0, /* Not used. */
        };

    	camera__pull_reset_pin(true);

    	camera__pull_power_pin(true);
    	SDK_DelayAtLeastUs(10000,SystemCoreClock);
       	camera__pull_power_pin(false);
        SDK_DelayAtLeastUs(20000,SystemCoreClock);

    	camera__pull_reset_pin(false);
    	SDK_DelayAtLeastUs(20000,SystemCoreClock);

    	camera__pull_reset_pin(true);
    	SDK_DelayAtLeastUs(20000,SystemCoreClock);


#else

        camera_config_t camconfig = {
            .pixelFormat                = kVIDEO_PixelFormatRGB565,
            .resolution                 = FSL_VIDEO_RESOLUTION(320, 102),
            .framePerSec                = 30,//
            .interface                  = kCAMERA_InterfaceNonGatedClock ,
            .frameBufferLinePitch_Bytes = 0, /* Not used. */
            .controlFlags               = kCAMERA_HrefActiveHigh | kCAMERA_VsyncActiveHigh, /* Not used. */
            .bytesPerPixel              = 0, /* Not used. */
            .mipiChannel                = 0, /* Not used. */
            .csiLanes                   = 0, /* Not used. */
        };

#endif

    CAMERA_DEVICE_Init(&handle, &camconfig);



    INPUTMUX_Init(INPUTMUX0);

    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_GpioPort0Pin10ToSmartDma);
    /* P0_11 is selected for SMARTDMA arch B 1 */
    INPUTMUX_AttachSignal(INPUTMUX0, 1U, kINPUTMUX_GpioPort0Pin11ToSmartDma);
    /* P0_14 is selected for SMARTDMA arch B 2 */
    INPUTMUX_AttachSignal(INPUTMUX0, 2U, kINPUTMUX_GpioPort0Pin14ToSmartDma);
    /* Enable clock for PCLK. */

    //figure out what these do... From the old camera project.
    SYSCON->LPCAC_CTRL &= ~1;                                  // rocky: enable LPCAC ICache
    SYSCON->NVM_CTRL &= SYSCON->NVM_CTRL & ~(1 << 2 | 1 << 4); // enable flash Data cache     /* init I3C0*/
    SYSCON->AHBMATPRIO |= (0x3<<4)|(0x3<<6); // Give priority to SmartDMA
    
    /* Attach camera signals. P0_4/P0_11/P0_5 is set to EZH_CAMERA_VSYNC/EZH_CAMERA_HSYNC/EZH_CAMERA_PCLK. */
    INPUTMUX_Init(INPUTMUX0);
    INPUTMUX_AttachSignal(INPUTMUX0, 0, kINPUTMUX_GpioPort0Pin4ToSmartDma);
    INPUTMUX_AttachSignal(INPUTMUX0, 1, kINPUTMUX_GpioPort0Pin11ToSmartDma);
    INPUTMUX_AttachSignal(INPUTMUX0, 2, kINPUTMUX_GpioPort0Pin5ToSmartDma);
    /* Turn off clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX0);

    PORT1->PCR[4] = (7 << 8) | (1 << 12); // EZH_PIO0, PIO1_4,P1_4/EZH_LCD_D0_CAMERA_D0/SAI0_TXD1
    PORT1->PCR[5] = (7 << 8) | (1 << 12); // EZH_PIO1, PIO1_5,P1_5/EZH_LCD_D1_CAMERA_D1
    PORT1->PCR[6] = (7 << 8) | (1 << 12); // EZH_PIO2, PIO1_6,P1_6/EZH_LCD_D2_CAMERA_D2
    PORT1->PCR[7] = (7 << 8) | (1 << 12); // EZH_PIO3, PIO1_7,P1_7/EZH_LCD_D3_CAMERA_D3
    PORT3->PCR[4] = (7 << 8) | (1 << 12); // EZH_PIO4, PIO1_8,P1_8/EZH_LCD_D4_CAMERA_D4
    PORT3->PCR[5] = (7 << 8) | (1 << 12); // EZH_PIO5, PIO1_9,P1_9/EZH_LCD_D5_CAMERA_D5
    PORT1->PCR[10] = (7 << 8) | (1 << 12); // EZH_PIO6, PIO1_10,P1_10/EZH_LCD_D6_CAMERA_D6
    PORT1->PCR[11] = (7 << 8) | (1 << 12); // EZH_PIO7, PIO1_11,P1_11/EZH_LCD_D7_CAMERA_D7

    PORT1->PCR[17] = (7 << 8) | (1 << 12); // EZH_PIO13, PIO1_17



	smartdmaParam.smartdma_stack = (uint32_t*)g_samrtdma_stack;
	smartdmaParam.p_buffer  	 = (uint32_t*)g_camera_buffer;
    SMARTDMA_InstallCallback(ezh_camera_callback, NULL);
    NVIC_EnableIRQ(SMARTDMA_IRQn);
    NVIC_SetPriority(SMARTDMA_IRQn, 3);

    memset((void *)g_camera_buffer, 2, sizeof(g_camera_buffer));

    SMARTDMA_InitWithoutFirmware();

    #define SMARTDMA_HANDSHAKE_EVENT  0U
	#define SMARTDMA_HANDSHAKE_ENABLE 1U
	#define SMARTDMA_MASK_RESP        2U
	#define SMARTDMA_ENABLE_AHBBUF    3U
	#define SMARTDMA_ENABLE_GPISYNCH  4U

    LPC_EZH_ARCH_B0->EZHB_CTRL    = (0xC0DE0000U | (1U << SMARTDMA_ENABLE_GPISYNCH));
    LPC_EZH_ARCH_B0->EZHB_ARM2EZH = ((uint32_t)(uint8_t *)(&smartdmaParam)) | 0x02;
    LPC_EZH_ARCH_B0->EZHB_BOOT = (uint32_t)(&ezh_binary[0]);
    LPC_EZH_ARCH_B0->EZHB_CTRL    = 0xC0DE0011U | (0U << SMARTDMA_MASK_RESP) | (0U << SMARTDMA_ENABLE_AHBBUF); /* BOOT */


    DEBUG("EZH interface configured\r\n");
    return 0;
}


//efine BOARD_CAMERA_I2C_INSTANCE   0
#define BOARD_CAMERA_I2C_CLOCK_FREQ       CLOCK_GetLPFlexCommClkFreq(BOARD_CAMERA_I2C_INSTANCE)

void camera__i2c_init(){
    CLOCK_EnableClock(kCLOCK_LPI2c7);
    LP_FLEXCOMM_Init(BOARD_CAMERA_I2C_INSTANCE, LP_FLEXCOMM_PERIPH_LPI2C);

    lpi2c_master_config_t lpi2cConfig = {0};
    LPI2C_MasterGetDefaultConfig(&lpi2cConfig);
    LPI2C_MasterInit(LPI2C7, &lpi2cConfig, BOARD_CAMERA_I2C_CLOCK_FREQ);
}

