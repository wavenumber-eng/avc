
#ifndef E_PORT_NXP__MCXN_
#define E_PORT_NXP__MCXN_

#include "fsl_common.h"

//Some systems have RAM/Flash at this location.  Pick a number that works for you...
#define CONFIG__E_LIST_NULL			  			  (void *)0

#define CONFIG__E_TICK_IRQ_HANDLER_NAME			   SysTick_Handler

#define CONFIG__E_TICK_PERIOD__MS			      (1)

#define CONFIG__E_TICK_IRQ_PRIORITY		  	      ((1<<__NVIC_PRIO_BITS)-1)

#define CONFIG__E_WFI						       __WFI()

#define CONFIG__E_TICK_INIT()				       SystemCoreClockUpdate(); \
										           SysTick_Config((uint32_t)SystemCoreClock * (uint32_t)CONFIG__E_TICK_PERIOD__MS / 1000);\
									               NVIC_SetPriority(SysTick_IRQn , CONFIG__E_TICK_IRQ_PRIORITY)\


#endif
