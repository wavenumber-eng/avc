#include "avc__master_config.h"
#include "avc__camera_interface.h"
#include "fsl_clock.h"
#include "fsl_inputmux.h"
#include "fsl_ov7670.h"
#include "fsl_ov5640.h"
#include "board.h"
#include "fsl_smartdma.h"
#include "lpspi1.h"
#include "bunny_build.h"
#include "cr_section_macros.h"
#include "stdbool.h"

	/*
		EZH_Camera_320_240_Whole_Buffer ,
		the camera resolution must be 320*240,
		the camera must be set that the pclk is disapeared during HSYNC blocking.
		the buffer is whole resolution size, 320*249*2=150K bytes

		RGB565 format
		R0:temporary variable
		R1:temporary variable
		R2:reserved
		R3:camera buffer address
		R4:reserved
		R5:reserved
		R6:para base address
		R7:reserved
	*/

//__BSS(SRAMH) volatile uint32_t ezh_binary[512];
volatile uint32_t my_ezh_program[512];

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
	E_STRB_POST(R3, GPI, 1);  			// data will be stored from GPI bottom byte to RAM
	E_BSET_IMM(CFM, CFM, 0); 			//clear the vector flag

	E_SUB_IMMS(PC, PC, 4 * 5);//BS0
	E_BTOG_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
	E_NOP;
	E_NOP;

	E_GOSUB("VSYNC_0");//BS1
	E_NOP;
	E_NOP;
	E_NOP;

	E_GOSUB("HSYNC_0");//BS2
	E_NOP;
	E_NOP;
	E_NOP;


	//	E_LABEL("PCLK_0");
	//	E_ACC_VECTORED_HOLD(PC,(1<<2));
	//	E_LSR_AND(R0, R1, GPI, 0);
	//	E_BSET_IMM(CFM, CFM, 2); 			//clear the vector flag
	//	E_GOSUB(VSYNC_0);//BS0
	//	E_GOSUB(HSYNC_0);//BS1
	//	E_SUB_IMMS(PC, PC, 4*7);
	//	E_STRB_POST(R3, R0, 1);  			// data will be stored from GPI bottom byte to RAM
	//	E_BTOG_IMM(GPO,GPO,13);				//toggle the P0_18, used to measure the timming in logic device
	//	E_GOSUB(PCLK_0);

	E_LABEL("VSYNC_0");
	E_BSET_IMM(CFM, CFM, 0); 			//clear the VSYNC vector flag
	E_BSET_IMM(CFM, CFM, 1); 			//enable HSYNC interrupt
	E_BSET_IMM(CFM, CFM, 2); 			//enable PCLK  interrupt
	E_LDR(R3, R6, 1);							//R3 point to the store buffer in the RAM
	E_BTOG_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
	E_INT_TRIGGER(0x11); // interrupt and told ARM data is ready
	E_GOSUB("PCLK_0");

	E_LABEL("HSYNC_0");
	E_BCLR_IMM(R3, R3, 0);
	E_BCLR_IMM(R3, R3, 1);
	E_BSET_IMM(CFM, CFM, 0); 			//clear the vector flag
	E_BSET_IMM(CFM, CFM, 1); 			//clear the vector flag
	E_BTOG_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
	E_GOSUB("PCLK_0");
}


#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
/* Camera resource and handle */
const ov7670_resource_t resource = {
    .i2cSendFunc = BOARD_Camera_I2C_SendSCCB,
    .i2cReceiveFunc = BOARD_Camera_I2C_ReceiveSCCB,
    .xclock = kOV7670_InputClock12MHZ,
};
#endif

#if CONFIG__CAMERA_SELECT == CAMERA__OV5640
const ov5640_resource_t resource = {
    .i2cSendFunc = BOARD_Camera_I2C_SendSCCB,
    .i2cReceiveFunc = BOARD_Camera_I2C_ReceiveSCCB,
    .pullResetPin = camera__pull_reset_pin,     /*!< Function to pull reset pin high or low. */
    .pullPowerDownPin = camera__pull_power_pin, /*!< Function to pull the power down pin high or low. */
};
#endif

camera_device_handle_t handle =
{
    .resource = (void *)&resource,

#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
    .ops = &ov7670_ops,
#endif

#if CONFIG__CAMERA_SELECT == CAMERA__OV5640
    .ops = &ov5640_ops
#endif

};


/**
 * EZH WILL BE FILLING ONE JUST OF THESE g_camera_buffers
 * The buffer that is not being filled is the one used to be shown in the display
 */
static uint16_t g_camera_buffer1[DEMO_BUFFER_WIDTH * DEMO_BUFFER_HEIGHT];
static uint16_t g_camera_buffer2[DEMO_BUFFER_WIDTH * DEMO_BUFFER_HEIGHT];
uint16_t * processing_buffer;
uint16_t * buffer_addresses [2];
uint8_t buffer_index;

volatile uint8_t g_samrtdma_stack[32] = {0};

uint16_t display_buffer [160 * 120];
eGFX_ImagePlane camera_image =
{
    .Type =  eGFX_IMAGE_PLANE_16BPP_RGB565, 
    .Data = (uint8_t *)display_buffer, 
    .SizeX = 160,
    .SizeY = 120,
    .User = NULL
};

void avc__camera_interface_init()
{

/*
NOt working yet...  Still have the wierd noise issue related to the pixel clock.

The program matches the smart DMA api verison.
*/

#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
    CLOCK_AttachClk(kFRO12M_to_CLKOUT);
    CLOCK_SetClkDiv(kCLOCK_DivClkOut, 1U);
#else

    /*6MHz For OV5640*/
    CLOCK_AttachClk(kFRO12M_to_CLKOUT);
    CLOCK_SetClkDiv(kCLOCK_DivClkOut, 2U);
#endif

    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7);
    CLOCK_EnableClock(kCLOCK_LPFlexComm7);
    CLOCK_EnableClock(kCLOCK_LPI2c7);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom7Clk, 1u);



    /* Attach camera signals. P0_4/P0_11/P0_5 is set to EZH_CAMERA_VSYNC/EZH_CAMERA_HSYNC/EZH_CAMERA_PCLK. */
    INPUTMUX_Init(INPUTMUX0);
    INPUTMUX_AttachSignal(INPUTMUX0, 0, kINPUTMUX_GpioPort0Pin4ToSmartDma);
    INPUTMUX_AttachSignal(INPUTMUX0, 1, kINPUTMUX_GpioPort0Pin11ToSmartDma);
    INPUTMUX_AttachSignal(INPUTMUX0, 2, kINPUTMUX_GpioPort0Pin5ToSmartDma);
    /* Turn off clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX0);

    DEMO_InitCamera();
    PORT1->PCR[4] = (7 << 8) | (1 << 12); // EZH_PIO0, PIO1_4,P1_4/EZH_LCD_D0_CAMERA_D0/SAI0_TXD1
    PORT1->PCR[5] = (7 << 8) | (1 << 12); // EZH_PIO1, PIO1_5,P1_5/EZH_LCD_D1_CAMERA_D1
    PORT1->PCR[6] = (7 << 8) | (1 << 12); // EZH_PIO2, PIO1_6,P1_6/EZH_LCD_D2_CAMERA_D2
    PORT1->PCR[7] = (7 << 8) | (1 << 12); // EZH_PIO3, PIO1_7,P1_7/EZH_LCD_D3_CAMERA_D3
    PORT3->PCR[4] = (7 << 8) | (1 << 12); // EZH_PIO4, PIO1_8,P1_8/EZH_LCD_D4_CAMERA_D4
    PORT3->PCR[5] = (7 << 8) | (1 << 12); // EZH_PIO5, PIO1_9,P1_9/EZH_LCD_D5_CAMERA_D5
    PORT1->PCR[10] = (7 << 8) | (1 << 12); // EZH_PIO6, PIO1_10,P1_10/EZH_LCD_D6_CAMERA_D6
    PORT1->PCR[11] = (7 << 8) | (1 << 12); // EZH_PIO7, PIO1_11,P1_11/EZH_LCD_D7_CAMERA_D7

    // Buffers initialization
    buffer_index = 0;
    buffer_addresses[0] = &g_camera_buffer1[0];
    buffer_addresses[1] = &g_camera_buffer2[0];
    processing_buffer = buffer_addresses[1];

    DEMO_InitSmartDma();

}


static void DEMO_InitCamera(void)
{

#if (CONFIG__CAMERA_SELECT == CAMERA__OV7670)

    /* Init ov7670 module with default setting. */
    camera_config_t camconfig = {
        .pixelFormat = kVIDEO_PixelFormatRGB565,
        .resolution = DEMO_CAMERA_RESOLUTION,
        .framePerSec = 30,
        .interface = kCAMERA_InterfaceGatedClock,
        .frameBufferLinePitch_Bytes = 0, /* Not used. */
        .controlFlags = 0,               /* Not used. */
        .bytesPerPixel = 0,              /* Not used. */
        .mipiChannel = 0,                /* Not used. */
        .csiLanes = 0,                   /* Not used. */
    };
#else

    camera_config_t camconfig = {
        .pixelFormat = kVIDEO_PixelFormatRGB565,
        .resolution = DEMO_CAMERA_RESOLUTION,
        .framePerSec = 15,
        .interface = kCAMERA_InterfaceNonGatedClock,                      // bug in driver.   This flag does the opposite.  We want gated clock
        .frameBufferLinePitch_Bytes = 0,                                  /* Not used. */
        .controlFlags = kCAMERA_HrefActiveHigh | kCAMERA_VsyncActiveHigh, /* Not used. */
        .bytesPerPixel = 0,                                               /* Not used. */
        .mipiChannel = 0,                                                 /* Not used. */
        .csiLanes = 0,                                                    /* Not used. */
    };

#endif

    camera__pull_reset_pin(true);
    camera__pull_power_pin(false);

#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
    PORT0->PCR[5] |= PORT_PCR_IBE(1) | PORT_PCR_INV(1);
#endif

    BOARD_Camera_I2C_Init();
    CAMERA_DEVICE_Init(&handle, &camconfig);
}

#define SMARTDMA_HANDSHAKE_EVENT  0U
#define SMARTDMA_HANDSHAKE_ENABLE 1U
#define SMARTDMA_MASK_RESP        2U
#define SMARTDMA_ENABLE_AHBBUF    3U
#define SMARTDMA_ENABLE_GPISYNCH  4U

#if defined(SMARTDMA0) && !(defined(SMARTDMA))
#define SMARTDMA SMARTDMA0
#endif

#include "ezh_init.h"

static smartdma_camera_param_t smartdmaParam;

static void DEMO_InitSmartDma(void)
{

    /*
      THis bunny build program not yet working with ov7670.  There seemed to be a change in the latest SDK.

	BUNNY_BUILD_PRINTF("Build EZH Camera application\r\n");


	uint32_t words_assembled = bunny_build((uint32_t* )&my_ezh_program[0],
							   sizeof(my_ezh_program),
							   EZH_Camera_320240_Whole_Buf);

	BUNNY_BUILD_PRINTF(VT100_YELLOW"%d "VT100_DEFAULT" 32bit words assembled (%d bytes) \r\n"VT100_DEFAULT,words_assembled, words_assembled<<2);

*/
    /* Clear camera buffer. */
    memset((void *)g_camera_buffer1, 0, sizeof(g_camera_buffer1));
    memset((void *)g_camera_buffer2, 0, sizeof(g_camera_buffer2));


    /* Init smartdma for camera. */
    SMARTDMA_InitWithoutFirmware();
    SMARTDMA_InstallFirmware(SMARTDMA_CAMERA_MEM_ADDR, s_smartdmaCameraFirmware, SMARTDMA_CAMERA_FIRMWARE_SIZE);

//   SMARTDMA_InstallFirmware(&my_ezh_program[0], my_ezh_program, sizeof(my_ezh_program));

    SMARTDMA_InstallCallback(SDMA_CompleteCallback, NULL); /* Set camera call back. */
    NVIC_EnableIRQ(SMARTDMA_IRQn);
    NVIC_SetPriority(SMARTDMA_IRQn, 3);

    /* Boot smartdma. */
    smartdmaParam.smartdma_stack = (uint32_t *)g_samrtdma_stack;
    smartdmaParam.p_buffer = (uint32_t *)g_camera_buffer1;
    /* Make sure the frame size that the firmware fetches is smaller than or equal to the camera resolution.
       In this case it is half of the camera resolution. */
    SMARTDMA_Boot(DEMO_SMARTDMA_API, &smartdmaParam, 0x2);

/*
    LPC_EZH_ARCH_B0->EZHB_CTRL    = (0xC0DE0000U | (1U << SMARTDMA_ENABLE_GPISYNCH));
    LPC_EZH_ARCH_B0->EZHB_ARM2EZH = ((uint32_t)(uint8_t *)(&smartdmaParam)) | 0x02;
    LPC_EZH_ARCH_B0->EZHB_BOOT = (uint32_t)(&my_ezh_program[0]);
    LPC_EZH_ARCH_B0->EZHB_CTRL    = 0xC0DE0011U | (0U << SMARTDMA_MASK_RESP) | (0U << SMARTDMA_ENABLE_AHBBUF);
*/
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



static void SDMA_CompleteCallback(void *param)
{

    processing_buffer = buffer_addresses[buffer_index];

    buffer_index = (buffer_index + 1) & 1;
    smartdmaParam.p_buffer = (uint32_t *)buffer_addresses[buffer_index];


    if(request_frame_for_display)
    {
        dma_copy_buffer((uint32_t * )processing_buffer, (uint32_t * )camera_image.Data, 1, 160 * 120 * 2);
        request_frame_for_display = false;
    }


}


bool avc__is_frame_ready()
{
	return (request_frame_for_display == false)  && (mem_transfer_done == true);
}

uint16_t * avc__get_frame_data()
{
	return processing_buffer;
}

void avc__request_new_frame_for_display()
{
    request_frame_for_display = true;
}



