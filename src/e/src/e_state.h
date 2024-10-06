
#ifndef E_STATE_H_
#define E_STATE_H_

typedef enum
{
	e_state__crunching = 0,
	e_state__waiting,
	e_state__transitioning

}e_state_e;

typedef struct e_state_machine e_state_machine_t;

typedef int32_t (*e_state_function_t)(e_state_machine_t *);
typedef int32_t (*e_state_transition_function_t)(e_state_machine_t *,int32_t);


typedef struct e_state_machine
{
	//internal for tracking if state machine is waiting, etc.
	e_state_e master_state;

	void * user;

	int32_t queued_state;
	int32_t current_state;

	//Internal timing
	uint32_t time_to_wait;
	uint32_t state_tick;

	//total number if states
	//use to bound check array of state function pointers
	int32_t num_states;

	//The transition callback should execute one time transition logic
	//and return the state index of the new/requested state if it is allowed
	e_state_transition_function_t transition;

	//array of function pointers to the logic for the different states
	e_state_function_t *crunch;

}e_state_machine_t;



void e_state__init(e_state_machine_t *sm,
				   e_state_transition_function_t state_transitition_function,
				   e_state_function_t *state_logic_functions,
				   uint32_t num_states);

void e_state__crunch(e_state_machine_t *sm);

void e_state__transition(e_state_machine_t * sm,int32_t next_state);

void e_state__delayed_transition(e_state_machine_t * sm,int32_t next_state,uint32_t ms_to_wait);

void e_state__wait(e_state_machine_t * sm,int32_t ms_to_wait);




#endif /* E_STATE_H_ */
