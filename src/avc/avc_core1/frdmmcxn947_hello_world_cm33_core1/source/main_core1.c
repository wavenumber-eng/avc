/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "board.h"
#include "mcmgr.h"

#include "fsl_common.h"
#include "fsl_gpio.h"


//#include "st7789.h"
//#include "lpspi1.h"
//#include "eGFX_Driver_ER-TFT020-3.h"
//#include "assets/Sprites_16BPP_RGB565.h"
//#include "fsl_lpspi_edma.h"
//#include "avc__camera_interface.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LED_INIT()   LED_RED_INIT(LOGIC_LED_ON)
#define LED_TOGGLE() LED_RED_TOGGLE()

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Application-specific implementation of the SystemInitHook() weak function.
 */
void SystemInitHook(void)
{
    /* Initialize MCMGR - low level multicore management library. Call this
       function as close to the reset entry as possible to allow CoreUp event
       triggering. The SystemInitHook() weak function overloading is used in this
       application. */
    (void)MCMGR_EarlyInit();
}
/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t startupData, i;
    mcmgr_status_t status;

    /* Init board hardware.*/
    /* enable clock for GPIO */
    CLOCK_EnableClock(kCLOCK_Gpio0);
    BOARD_InitBootPins();

    /* Initialize MCMGR, install generic event handlers */
    (void)MCMGR_Init();

    /* Get the startup data */
    do
    {
        status = MCMGR_GetStartupData(&startupData);
    } while (status != kStatus_MCMGR_Success);

    /* Make a noticable delay after the reset */
    /* Use startup parameter from the master core... */
    for (i = 0; i < startupData; i++)
    {
        SDK_DelayAtLeastUs(1000000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    }

    /* Configure LED */
//    LED_INIT();

//    CLOCK_SetClkDiv(kCLOCK_DivFlexcom1Clk, 1u);
//    CLOCK_AttachClk(kPLL_DIV_to_FLEXCOMM1);
//
//    CLOCK_EnableClock(kCLOCK_Dma0);
//    lpspi1_init(8); // Initialize with 8-bit SPI transactions
//    eGFX_InitDriver(0);
//    eGFX_Dump(&Sprite_16BPP_RGB565_bg1);

//    avc__camera_interface_init();

    for (;;)
    {
        SDK_DelayAtLeastUs(500000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
//        LED_TOGGLE();
        //if(cam_data_rdy)
        //{
        //    LED_TOGGLE();
        //}
    }


    
}
