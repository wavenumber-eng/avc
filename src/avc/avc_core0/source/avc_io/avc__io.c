#include "avc__io.h"

#ifndef CONFIG__AVC_UART_TX_Q_SIZE_BYTES
	#define CONFIG__AVC_UART_TX_Q_SIZE_BYTES 2048
#endif

#ifndef CONFIG__AVC_UART_RX_Q_SIZE_BYTES
	#define CONFIG__AVC_UART_RX_Q_SIZE_BYTES 2048
#endif

BYTE_QUEUE__MAKE(UART4_TX_Q , CONFIG__AVC_UART_TX_Q_SIZE_BYTES);
BYTE_QUEUE__MAKE(UART4_RX_Q , CONFIG__AVC_UART_TX_Q_SIZE_BYTES);



button_t left_btn, right_btn, center_btn;


void avc_io__uart_init();


void LP_FLEXCOMM4_IRQHandler(void)
{
    /* If new data arrived. */
    if ((kLPUART_RxDataRegFullFlag)&LPUART_GetStatusFlags(LPUART4))
    {
    	bq__enqueue(&UART4_RX_Q, LPUART4->DATA);
    }

    if ((kLPUART_TxDataRegEmptyFlag)&LPUART_GetStatusFlags(LPUART4))
    {
        if(bq__bytes_available_to_write(&UART4_TX_Q))
    	{
    		LPUART4->DATA = bq__dequeue_next(&UART4_TX_Q);
    	}
    	else
    	{
    		 LPUART_DisableInterrupts(LPUART4, kLPUART_TxDataRegEmptyInterruptEnable);
    	}
    }

    SDK_ISR_EXIT_BARRIER;
}

void avc_io__uart_enqueue_hook(void *arg)
{
	LPUART_EnableInterrupts((LPUART_Type *)arg, kLPUART_TxDataRegEmptyInterruptEnable);
}

void avc_io__uart_init()
{
	    CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
	    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);

    	UART4_TX_Q.enqueue_hook = avc_io__uart_enqueue_hook;
    	UART4_TX_Q.hook_arg = (void *)LPUART4;

    	lpuart_config_t config;

    	/*
    	 *
    	 * UART 4
    	 *
    	 */

        RESET_ClearPeripheralReset(kFC4_RST_SHIFT_RSTn);
        LPUART_GetDefaultConfig(&config);
	    config.baudRate_Bps = 115200;
	    config.enableTx     = true;
	    config.enableRx     = true;

	    LPUART_Init(LPUART4, &config, 12000000);

	    LPUART_GetDefaultConfig(&config);

	    LPUART_EnableInterrupts(LPUART4, kLPUART_RxDataRegFullInterruptEnable);

	    EnableIRQ(LP_FLEXCOMM4_IRQn);

}



void avc__init()
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Dma0);

    avc_io__uart_init();

    e__init();

    MAILBOX_Init(MAILBOX);

    button__init(&left_btn, IN_PORT, LEFT_BTN_PIN, BUTTON_POLARITY_LOW_ACTIVE, 50);
    button__init(&right_btn, IN_PORT, RIGHT_BTN_PIN, BUTTON_POLARITY_LOW_ACTIVE, 50);
    button__init(&center_btn, IN_PORT, CENTER_BTN_PIN, BUTTON_POLARITY_LOW_ACTIVE, 50);

    /* Print the initial banner from Primary core */
    (void)DEBUG("\r\nHello World from core 0!\r\n");

    /* Boot Secondary core application */
    (void)DEBUG("Starting Secondary core.\r\n");


	#if defined(FSL_FEATURE_MAILBOX_SIDE_A)
		NVIC_SetPriority(MAILBOX_IRQn, 5);
	#else
		NVIC_SetPriority(MAILBOX_IRQn, 2);
	#endif

	NVIC_EnableIRQ(MAILBOX_IRQn);


//    /* Boot source for Core 1 from RAM */
//    SYSCON->CPBOOT = ((uint32_t)(char *)CORE1_EXE_ADDRESS & SYSCON_CPBOOT_CPBOOT_MASK);
//
//    uint32_t temp = SYSCON->CPUCTRL;
//    temp |= 0xc0c40000U;
//    SYSCON->CPUCTRL = temp | SYSCON_CPUCTRL_CPU1RSTEN_MASK | SYSCON_CPUCTRL_CPU1CLKEN_MASK;
//    SYSCON->CPUCTRL = (temp | SYSCON_CPUCTRL_CPU1CLKEN_MASK) & (~SYSCON_CPUCTRL_CPU1RSTEN_MASK);
//
//    (void)DEBUG("The secondary core application has been started.\r\n");


    avc_camera__init();
    eGFX_InitDriver(0);
    eGFX_Dump((eGFX_ImagePlane *)&Sprite_16BPP_RGB565_fit2025);
    e_tick__delay_ms(2000);

    avc__adc_init();
    avc__motor_control_init();
    avc__servo_control_init();


}
