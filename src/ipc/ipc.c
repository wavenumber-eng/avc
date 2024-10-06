#include "ipc.h"
#include "cr_section_macros.h"
#include "fsl_device_registers.h"
#include "fsl_mailbox.h"


 #if defined(FSL_FEATURE_MAILBOX_SIDE_A)
__BSS(IPC_MEM) volatile ipc_t avc_ipc;
#else
__NOINIT(IPC_MEM) volatile ipc_t avc_ipc;
#endif

 void MAILBOX_IRQHandler(void)
 {
     mailbox_cpu_id_t cpu_id;
 #if defined(FSL_FEATURE_MAILBOX_SIDE_A)
     cpu_id = kMAILBOX_CM33_Core0;
 #else
     cpu_id = kMAILBOX_CM33_Core1;
 #endif

     uint32_t value = MAILBOX_GetValue(MAILBOX, cpu_id);

     if ((value & 0x01) != 0UL)
     {
         MAILBOX_ClearValueBits(MAILBOX, cpu_id, 0x01);

     }
     if ((value & 0x02) != 0UL)
     {
         MAILBOX_ClearValueBits(MAILBOX, cpu_id, 0x02);
     }

     MAILBOX_ClearValueBits(MAILBOX, cpu_id, value);
 }

 void ipc__init()
 {

 }
