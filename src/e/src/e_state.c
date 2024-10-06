#include "e.h"


void e_state__init(e_state_machine_t *sm,
				   e_state_transition_function_t state_transitition_function,
				   e_state_function_t *state_logic_functions,
				   uint32_t num_states)
{

	sm->current_state = 0;
	sm->queued_state = 0;
	sm->master_state = e_state__crunching;
	sm->transition = state_transitition_function;
	sm->crunch = state_logic_functions;
	sm->num_states = num_states;

	//Zero is always the 1st/init state
	e_state__transition(sm,0);

}

void e_state__crunch(e_state_machine_t *sm)
{

	if(sm->master_state == e_state__waiting)
	{
		if(e_tick__timeout(&(sm->state_tick),sm->time_to_wait))
		{
			sm->master_state = e_state__crunching;

			if(sm->queued_state != sm->current_state)
			{
				e_state__transition(sm,sm->queued_state);
			}

		}
	}

	if(sm->master_state == e_state__transitioning)
	{
		sm->current_state = sm->queued_state;
		sm->master_state = e_state__crunching;
		sm->state_tick = e_tick__get_ms();
	}

	if (sm->master_state == e_state__crunching)
	{
		if(sm->crunch[sm->current_state] != CONFIG__E_LIST_NULL)
		{
			sm->crunch[sm->current_state](sm);
		}
	}

}

void e_state__transition(e_state_machine_t * sm,int32_t next_state)
{
	if (sm->master_state == e_state__crunching)
	{
		if(sm->transition != CONFIG__E_LIST_NULL)
		{
			int32_t ns = sm->transition(sm,next_state);

			if(ns == next_state)
			{
				sm->queued_state = ns;
				sm->master_state = e_state__transitioning;
			}
		}
	}

}

void e_state__wait(e_state_machine_t * sm,int32_t ms_to_wait)
{
	if (sm->master_state == e_state__crunching)
	{
		sm->master_state = e_state__waiting;
		sm->state_tick = e_tick__get_ms();
		sm->time_to_wait = ms_to_wait;
		sm->queued_state = sm->current_state;
	}
}

void e_state__delayed_transition(e_state_machine_t * sm,int32_t next_state,uint32_t ms_to_wait)
{
	if (sm->master_state == e_state__crunching)
	{
		sm->master_state = e_state__waiting;
		sm->state_tick = e_tick__get_ms();
		sm->time_to_wait = ms_to_wait;
		sm->queued_state = next_state;
	}
}




