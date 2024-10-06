#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_device_registers.h"
#include "fsl_mailbox.h"
#include "ipc.h"


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

int main(void)
{

    for (int i = 0; i < 1; i++)
    {
        SDK_DelayAtLeastUs(1000000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    }

    avc_ipc.core1_magic_boot_value = CORE1__MAGIC_BOOT_VALUE;



    MAILBOX_SetValueBits(MAILBOX, kMAILBOX_CM33_Core0, CORE0__MAILBOX___CORE1_READY);

    for (;;)
    {
    	SDK_DelayAtLeastUs(500000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
        avc_ipc.core1_counter++;

    }


    
}
