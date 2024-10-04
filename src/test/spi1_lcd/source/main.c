/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "st7789.h"
#include "lpspi1.h"
#include "eGFX_Driver_ER-TFT020-3.h"
#include "assets/Sprites_16BPP_RGB565.h"
#include "fsl_lpspi_edma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t test_arr [] = {1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void SysTick_Handler(void)
{

}

int main(void)
{
    
    /* attach FRO 12M to FLEXCOMM4 (debug console) */
    
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    CLOCK_SetClkDiv(kCLOCK_DivFlexcom1Clk, 1u);
    CLOCK_AttachClk(kPLL_DIV_to_FLEXCOMM1);

    CLOCK_EnableClock(kCLOCK_Dma0);

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    lpspi1_init(8); // Initialize with 8-bit SPI transactions

    eGFX_InitDriver(0);

    eGFX_DrawLine(&eGFX_BackBuffer[0], 0,0,240,320,0xFFFF);

    while (1)
    {   
        

        eGFX_Dump(&Sprite_16BPP_RGB565_bg2_save);
        for(volatile uint32_t i = 0; i < 20000000; i++);

        eGFX_Dump(&Sprite_16BPP_RGB565_bg1);
	    for(volatile uint32_t i = 0; i < 20000000; i++);
        
        /* Start master transfer, transfer data to slave.*/
        // lpspi1_transfer_block(test_arr, sizeof(test_arr));
        // show_picture();

        /* Increase loop count to change transmit buffer */
    }
}



