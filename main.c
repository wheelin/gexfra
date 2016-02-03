#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "defines.h"
#include "gexfra.h"

static Gexfra gxf;

int8_t sm_func(State_Machine_t * sm, Event_t * ev)
{
	Event_t temp_ev;
	printf("Event id is : %d\n", ev->ev_id);
	switch(sm->previous)
	{
		case SM1_state_Init:
			sm->current = SM1_other_state;
			break;
		case SM1_other_state:
			if(ev->ev_id == INTERRUPT)
			{
				sm->current = SM1_after_interrupt;
			}
			break;
		case SM1_after_interrupt:
			if(ev->ev_id == EV_OTHER)
			{
				sm->current = SM1_other_state;
			}
			break;
	}
	switch(sm->current)
	{
		case SM1_other_state:
			printf("In other state\n");
			temp_ev = create_Event(INTERRUPT);
			Gexfra_add_event(&gxf, temp_ev);
			break;
		case SM1_after_interrupt:
			printf("In 'after interrupt' state\n");
			temp_ev = create_Event(EV_OTHER);
			Gexfra_add_event(&gxf, temp_ev);
			break;
		default:
			printf("Problem with the states\n");
			temp_ev = create_Event(EV_OTHER);
			Gexfra_add_event(&gxf, temp_ev);
			break;
	}
	sm->previous = sm->current;
	return 0;
}

void main(void)
{
	State_Machine_t sm;
	State_Machine_init(&sm, 1, SM1_state_Init, sm_func);

	Gexfra_init(&gxf);
	Gexfra_add_state_machine(&gxf, &sm);

	Gexfra_run(&gxf);
}