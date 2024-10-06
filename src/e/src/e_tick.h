#ifndef E__TICK_H_
#define E__TICK_H_

uint32_t e_tick__get_ms();

void e_tick__delay_ms(uint32_t delay);

uint32_t e_tick__delta(uint32_t *time_in);

uint32_t e_tick__timeout(uint32_t *time_in, uint32_t delta);

typedef void (*e_tick__hook_t)(void * arg);

#define E_TICK__TRIGGER_FOREVER		(-1)

typedef struct
{
	void *next_item;	  //Internal linked list

	int32_t interval__mS; //how often to fire
	int32_t duration__mS; //time until 1st fire
	int32_t     count; 	  //Number of times to call. Set to E_TICK__TRIGGER_FOREVER to keep triggering
	e_tick__hook_t hook;     //function to call when timer fires
	bool marked_for_exe;  //Use to flag a the hook call.

	void *arg;			  //argument to pass

} e_tick__trigger_handler_t;

void e_tick__register_trigger(e_tick__trigger_handler_t *trigger,bool irq_exe);

void e_tick__init();

void e_tick__crunch();



#endif /* SYSTEM__TICK_H_ */
