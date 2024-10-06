#ifndef _E_CONFIG__PORT_H_
#define _E_CONFIG__PORT_H_


#ifndef E__PORT_OPTION
	#error "Porting layer not defined for e"
#endif

#if  E__PORT_OPTION == E__PORT_OPTION__MCXN

#include <port/nxp__mcxn/e_port__nxp__mcxn.h>

#elif E__PORT_OPTION == E__PORT_OPTION__MCXA

#include <port/nxp__mcxa/e_port__nxp__mcxa.h>

#elif E__PORT_OPTION == E__PORT_OPTION__LPC865

#include <port/nxp__lpc865/e_port__nxp__lpc865.h>

#endif

#ifndef  CONFIG__E_LIST_NULL
	#error "e port layer needs  CONFIG__E_LIST_NULL  defined"
#endif

#ifndef  CONFIG__E_TICK_PERIOD__MS
	#error "e port layer needs  CONFIG__E_TICK_PERIOD__MS  defined"
#endif

#ifndef  CONFIG__E_TICK_IRQ_PRIORITY
	#error "e port layer needs  CONFIG__E_TICK_IRQ_PRIORITY  defined"
#endif

#ifndef  CONFIG__E_WFI
	#error "e port layer needs  CONFIG__E_WFI  defined"
#endif

#ifndef  CONFIG__E_TICK_INIT
	#error "e port layer needs  CONFIG__E_TICK_INIT  defined"
#endif


#endif
