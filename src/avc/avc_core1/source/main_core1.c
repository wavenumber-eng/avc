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

int main(void)
{
    avc_ipc.core1_magic_boot_value = CORE1__MAGIC_BOOT_VALUE;
    
    eGFX_InitDriver(0);

//    for (int i = 0; i < 1; i++)
//    {
//        SDK_DelayAtLeastUs(1000000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
//    }


    MAILBOX_SetValueBits(MAILBOX, kMAILBOX_CM33_Core0, CORE0__MAILBOX___CORE1_READY);
    avc_ipc.display_request = true;

    while (1)
    {
        //SDK_DelayAtLeastUs(500000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
        
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
