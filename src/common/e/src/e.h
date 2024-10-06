#include "stdio.h"
#include "stdlib.h"
#include "stddef.h"
#include "stdbool.h"
#include "stdarg.h"

#include "e_config.h"
#include "e_vt100.h"

#include "e_queue.h"
#include "e_debug.h"
#include "e_shell.h"
#include "e_tick.h"
#include "e_state.h"


#ifndef __E_H__
#define __E_H__

#define E__INTERFACE__VERSION  0x0001

void 	e__init();
void 	e__crunch();

#if CONFIG__E_SHELL__ENABLE != 0

    void e__register_system_shell(e_shell__context_t shell);

	e_shell__context_t e__get_system_shell();

#endif

#endif /* SYSTEM_H_ */
