#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_device_registers.h"
#include "fsl_mailbox.h"
#include "ipc.h"
#include "eGFX_Driver_ER-TFT020-3.h"


void SystemInitHook(void)
{


    MAILBOX_Init(MAILBOX);

    #if defined(FSL_FEATURE_MAILBOX_SIDE_A)
        NVIC_SetPriority(MAILBOX_IRQn, 5);
    #else
        NVIC_SetPriority(MAILBOX_IRQn, 2);
    #endif

    NVIC_EnableIRQ(MAILBOX_IRQn);

}

volatile uint32_t test_counter=0;


#define LCD_RS__SET		GPIO_PinWrite(GPIO0,26,1)
#define LCD_RS__CLR		GPIO_PinWrite(GPIO0,26,0)
#define LCD_RES__SET	GPIO_PinWrite(GPIO0,28,1)
#define LCD_RES__CLR	GPIO_PinWrite(GPIO0,28,0)

/* Symbols to be used with GPIO driver */
#define BOARD_LCDPINS_LCD_RS_GPIO GPIO0                /*!<@brief GPIO peripheral base pointer */
#define BOARD_LCDPINS_LCD_RS_GPIO_PIN 26U              /*!<@brief GPIO pin number */
#define BOARD_LCDPINS_LCD_RS_GPIO_PIN_MASK (1U << 26U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_LCDPINS_LCD_RS_PORT PORT0                /*!<@brief PORT peripheral base pointer */
#define BOARD_LCDPINS_LCD_RS_PIN 26U                   /*!<@brief PORT pin number */
#define BOARD_LCDPINS_LCD_RS_PIN_MASK (1U << 26U)      /*!<@brief PORT pin mask */
                                                       /* @} */

/*! @name PORT0_28 (coord E8), P0_28/J2[2]
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_LCDPINS_LCD_RST_GPIO GPIO0                /*!<@brief GPIO peripheral base pointer */
#define BOARD_LCDPINS_LCD_RST_GPIO_PIN 28U              /*!<@brief GPIO pin number */
#define BOARD_LCDPINS_LCD_RST_GPIO_PIN_MASK (1U << 28U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_LCDPINS_LCD_RST_PORT PORT0                /*!<@brief PORT peripheral base pointer */
#define BOARD_LCDPINS_LCD_RST_PIN 28U                   /*!<@brief PORT pin number */
#define BOARD_LCDPINS_LCD_RST_PIN_MASK (1U << 28U)      /*!<@brief PORT pin mask */
                                                        /* @} */


#include "fsl_port.h"
int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    avc_ipc.core1_magic_boot_value = CORE1__MAGIC_BOOT_VALUE;

    eGFX_InitDriver(0);

    MAILBOX_SetValueBits(MAILBOX, kMAILBOX_CM33_Core0, CORE0__MAILBOX___CORE1_READY);

    while (1)
    {
        
        switch(avc_ipc.core1_cmd)
        {
            case CORE1_IDLE:
                
                break;


            case CORE1_DUMP:
                eGFX_Dump(avc_ipc.image_plane_ptr);                 //Image resolution 320x240
                avc_ipc.core1_cmd = CORE1_IDLE;
                break;

            
            case CORE1_DOUBLE_AND_DUMP:
                eGFX_duplicate_and_dump(avc_ipc.image_plane_ptr);   //Image resolution 160x120
                avc_ipc.core1_cmd = CORE1_IDLE;
                avc_ipc.display_request = true;
                break;
                

            default:
                avc_ipc.core1_cmd = CORE1_IDLE;
                break;
        }
        
        avc_ipc.core1_counter++;
    }
        
}
