

#include "avc__master_config.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"

#include "fsl_gpio.h"

#include "avc__io.h"

#include "avc__adc.h"
#include "avc__motor_control.h"
#include "avc__servo_control.h"
#include "ipc.h"
#include "fsl_device_registers.h"
#include "fsl_mailbox.h"
#include "e.h"

#define CONFIG__CAMERA_CORE0_ENABLE 1


#if (CONFIG__CAMERA_CORE0_ENABLE == 1)
#include "st7789.h"
#include "lpspi1.h"
#include "eGFX_Driver_ER-TFT020-3.h"
#include "assets/Sprites_16BPP_RGB565.h"
#include "fsl_lpspi_edma.h"
#include "avc__camera_interface.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Address of RAM, where the image for core1 should be copied */
#define CORE1_BOOT_ADDRESS  0x20050000        ///0x2004E000



void SystemInitHook(void)
{
     MAILBOX_Init(MAILBOX);

	#if defined(FSL_FEATURE_MAILBOX_SIDE_A)
		NVIC_SetPriority(MAILBOX_IRQn, 5);
	#else
		NVIC_SetPriority(MAILBOX_IRQn, 2);
	#endif

	//	NVIC_EnableIRQ(MAILBOX_IRQn);

}

volatile uint16_t bat;

/*!
 * @brief Main function
 */

volatile uint32_t test;

int main(void)
{

    int8_t left_intensity, right_intensity, servo_position;


    /* Init board hardware.*/
    /* attach FRO 12M to FLEXCOMM4 (debug console) */

    BOARD_InitBootPins();
    BOARD_InitBootClocks(); 

    CLOCK_EnableClock(kCLOCK_Gpio0);

    avc_io__uart_init();

    e__init();


    avc__adc_init();
    avc__motor_control_init();
    avc__servo_control_init();
    avc__enable_motor_control();


    /* Print the initial banner from Primary core */
    (void)DEBUG("\r\nHello World from the Primary Core!\r\n\n");

    /* Boot Secondary core application */
    (void)DEBUG("Starting Secondary core.\r\n");

    /* Boot source for Core 1 from RAM */
    SYSCON->CPBOOT = ((uint32_t)(char *)CORE1_BOOT_ADDRESS & SYSCON_CPBOOT_CPBOOT_MASK);

    uint32_t temp = SYSCON->CPUCTRL;
    temp |= 0xc0c40000U;
    SYSCON->CPUCTRL = temp | SYSCON_CPUCTRL_CPU1RSTEN_MASK | SYSCON_CPUCTRL_CPU1CLKEN_MASK;
    SYSCON->CPUCTRL = (temp | SYSCON_CPUCTRL_CPU1CLKEN_MASK) & (~SYSCON_CPUCTRL_CPU1RSTEN_MASK);

    (void)PRINTF("The secondary core application has been started.\r\n");




#if (CONFIG__CAMERA_CORE0_ENABLE == 1)
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom1Clk, 1u);
    CLOCK_AttachClk(kPLL_DIV_to_FLEXCOMM1);

    CLOCK_EnableClock(kCLOCK_Dma0);
    lpspi1_init(8); // Initialize with 8-bit SPI transactions
    eGFX_InitDriver(0);
    eGFX_Dump(&Sprite_16BPP_RGB565_bg1);
    avc__camera_interface_init();
/*
 *
 * * RAM ECC Setting -->> ECC is disabled
 * Check AHB Clock Control --> All RAMs enabled
 *
 *disable 2nd core and try ipc struct --> Fails
 *disable 2nd core but moved its memory to 1st64k SRAMX
 *disable
 *disable
 * What is full dump time of 320x240
 * What is DMA Copy time for temporatory buffer
 * does can full dump work with entire frame

 * Expand Flash
 * Make backbuffer for camera
 * Bunny cam
 * Check Vsync to 1st Hync Timing
 * Check actual fraem rate
 * add "e"
 * add avc__frame_ready()
 * add avc__request_frame()
 *
 *
 */
#endif

    while (1)
    {
        test = avc_ipc.core1_counter;

        bat = avc__read_battery_voltage();
        
        // Test mode enable
        if(GPIO_PinRead(GPIO3, 18) == 0)
        {   
            
            left_intensity = (2*(int16_t)avc__read_alpha()) - 100;
            right_intensity = (2*(int16_t)avc__read_gamma()) - 100;
            servo_position = (2*(int16_t)avc__read_beta()) - 100;   //to change the full range of servo

            avc__set_motor_pwm(left_intensity, right_intensity);
            avc__set_servo(servo_position);


            if((display_data_request == false)  && (mem_transfer_done == true))
            {
                #if CONFIG__OV7670_IS_160x120 == 1
                    eGFX_duplicate_and_dump(&camera_image);
                #else
                    eGFX_Dump(&camera_image);
                #endif
                display_data_request = true;
            }

        }
    }
    
}
