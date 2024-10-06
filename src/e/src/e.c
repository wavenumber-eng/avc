#include "e.h"

#if CONFIG__E_SHELL__ENABLE != 0

static e_shell__context_t e_shell__system = NULL;

void e__register_system_shell(e_shell__context_t shell)
{
	e_shell__system = shell;
}

e_shell__context_t e__get_system_shell()
{
	return e_shell__system;
}


#endif

void e__init()
{
	e_tick__init();
}

void e__crunch()
{
	e_tick__crunch();

	#if CONFIG__E_SHELL__ENABLE != 0

		if(e_shell__system!=NULL)
		{
			e_shell__crunch(e_shell__system);
		}

	#endif

}
