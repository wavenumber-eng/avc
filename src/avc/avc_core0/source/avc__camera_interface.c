#include "avc__master_config.h"
#include "avc__camera_interface.h"
#include "fsl_clock.h"
#include "fsl_inputmux.h"
#include "fsl_ov7670.h"
#include "fsl_ov5640.h"
#include "board.h"
#include "fsl_smartdma.h"
#include "lpspi1.h"

    

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

static uint16_t g_camera_buffer[DEMO_BUFFER_WIDTH * DEMO_BUFFER_HEIGHT];
volatile uint8_t g_samrtdma_stack[32] = {0};


uint16_t processing_buffer [160 * 120];
eGFX_ImagePlane camera_image =
{
    .Type =  eGFX_IMAGE_PLANE_16BPP_RGB565, 
    .Data = (uint8_t *)processing_buffer, 
    .SizeX = 320, 
    .SizeY = 240, 
    .User = NULL
};

void avc__camera_interface_init()
{

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

static void DEMO_InitSmartDma(void)
{
    static smartdma_camera_param_t smartdmaParam;

    /* Clear camera buffer. */
    memset((void *)g_camera_buffer, 0, sizeof(g_camera_buffer));

    /* Init smartdma for camera. */
    SMARTDMA_InitWithoutFirmware();
    SMARTDMA_InstallFirmware(SMARTDMA_CAMERA_MEM_ADDR, s_smartdmaCameraFirmware, SMARTDMA_CAMERA_FIRMWARE_SIZE);
    SMARTDMA_InstallCallback(SDMA_CompleteCallback, NULL); /* Set camera call back. */
    NVIC_EnableIRQ(SMARTDMA_IRQn);
    NVIC_SetPriority(SMARTDMA_IRQn, 3);

    /* Boot smartdma. */
    smartdmaParam.smartdma_stack = (uint32_t *)g_samrtdma_stack;
    smartdmaParam.p_buffer = (uint32_t *)g_camera_buffer;
    /* Make sure the frame size that the firmware fetches is smaller than or equal to the camera resolution.
       In this case it is half of the camera resolution. */
    SMARTDMA_Boot(DEMO_SMARTDMA_API, &smartdmaParam, 0x2);
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
    if(display_data_request)
    {
        dma_copy_buffer((uint32_t * )g_camera_buffer, (uint32_t * )camera_image.Data, 1, 160 * 120 * 2);
        display_data_request = false;
    }

    GPIO_PinWrite(GPIO4, 0, 1);
}
