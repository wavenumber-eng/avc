#include "e.h"

static volatile uint32_t e__delay_ticker;

static volatile uint32_t e__ms_ticker;

static uint32_t e_tick__trigger_ticker;

static e_tick__trigger_handler_t *e_tick__trigger_list__irq = CONFIG__E_LIST_NULL;

static e_tick__trigger_handler_t *e_tick__trigger_list__foreground = CONFIG__E_LIST_NULL;


uint32_t e_tick__get_ms()
{
    return e__ms_ticker;
}

void e_tick__delay_ms(uint32_t delay)
{
	e__delay_ticker = 0;

    while (e__delay_ticker < delay)
    {
    	CONFIG__E_WFI;
    }
}


uint32_t e_tick__delta(uint32_t *time_in)
{
	volatile uint32_t elapsed;

    if (time_in == NULL)
        return 0;

    elapsed = e__ms_ticker;

    if (elapsed >= *time_in)
    {
    	elapsed -= *time_in;
    }
    else
    {
    	elapsed += (0xFFFFFFFF - *time_in);
    }

    return elapsed;
}

uint32_t e_tick__timeout(uint32_t *time_in, uint32_t delta)
{
	volatile uint32_t elapsed;

    elapsed = e_tick__delta(time_in);

    if (elapsed >= delta)
    {
        *time_in = e_tick__get_ms();
        return 1 + (elapsed - delta);
    }

    return 0;
}

void e_tick__init()
{
	e_tick__trigger_list__irq = CONFIG__E_LIST_NULL;
	e_tick__trigger_list__foreground = CONFIG__E_LIST_NULL;

	CONFIG__E_TICK_INIT();
}

void e_tick__trigger_scan(e_tick__trigger_handler_t *trigger_list, uint32_t interval)
{
	e_tick__trigger_handler_t *nt = trigger_list;

	while(nt!=CONFIG__E_LIST_NULL)
	{
		if(nt->count!=0)
		{
			nt->duration__mS-=interval;

			if(nt->duration__mS<=0)
			{
				nt->duration__mS += nt->interval__mS;

				nt->marked_for_exe = true;
			 }
		 }

		nt=nt->next_item;
	}
}

void e_tick__trigger_exe(e_tick__trigger_handler_t *trigger_list)
{

	e_tick__trigger_handler_t *nt = trigger_list;

	while(nt!=CONFIG__E_LIST_NULL)
	{
		if(nt->hook!=NULL)
		{
			if(nt->marked_for_exe)
			{
				nt->hook(nt->arg);
				nt->marked_for_exe = false;

				if(nt->count != E_TICK__TRIGGER_FOREVER)
				{
					nt->count = nt->count - 1;
				}
			}
		}
		nt=nt->next_item;
	}
}

void e_tick__register_trigger(e_tick__trigger_handler_t *trigger,bool irq_exe)
{
	e_tick__trigger_handler_t ** tl;

	if(irq_exe)
	{
		tl = &e_tick__trigger_list__irq;
	}
	else
	{
		tl = &e_tick__trigger_list__foreground;
	}

	e_tick__trigger_handler_t* next_trigger = *tl;

	if(next_trigger!=CONFIG__E_LIST_NULL)
	{
		while(next_trigger->next_item!=CONFIG__E_LIST_NULL)
		{
			next_trigger = next_trigger->next_item;
		}

		next_trigger->next_item = trigger;
	}
	else //Deal with case where the list is empty
	{
		*tl = trigger;
		next_trigger = trigger;
		next_trigger->next_item = NULL;
	}
}

void e_tick__crunch()
{
	uint32_t interval;

	if((interval = e_tick__timeout(&e_tick__trigger_ticker,CONFIG__E_FOREGROUND_TRIGGER_CHECK_INTERVAL)))
	{
		e_tick__trigger_scan(e_tick__trigger_list__foreground,interval);
		e_tick__trigger_exe(e_tick__trigger_list__foreground);
	}
}

void CONFIG__E_TICK_IRQ_HANDLER_NAME()
{
	e__delay_ticker += CONFIG__E_TICK_PERIOD__MS;

	e__ms_ticker+= CONFIG__E_TICK_PERIOD__MS;

	e_tick__trigger_scan(e_tick__trigger_list__irq,CONFIG__E_TICK_PERIOD__MS);

	e_tick__trigger_exe(e_tick__trigger_list__irq);
}






