#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <stdint.h>

#include "defines.h"
#include "gexfra.h"

static Gexfra gxf;

int8_t sm_func(Event_t * ev)
{
	Event_t temp_ev;
	printf("Event id is : %d\n", ev->ev_id);
	switch(ev->ev_id)
	{
		case TM_500:
			printf("%s\n", "tm event\n");
			break;
		case INTERRUPT:
			printf("%s\n", "interrupt event\n");
			temp_ev.ev_id = INTERRUPT;
			temp_ev.used = true;
			EvHandler_add_event_to_list(&(gxf.evh), temp_ev);
			break;
		default:
			printf("%s\n", "not handled event\n");
			temp_ev.ev_id = INTERRUPT;
			temp_ev.used = true;
			EvHandler_add_event_to_list(&(gxf.evh), temp_ev);
	}
	return 0;
}

void main(void)
{
	Event_t ev = (Event_t) {INTERRUPT, true};
	State_Machine_t sm;
	State_Machine_init(&sm);
	State_Machine_set_function(&sm, sm_func);
	State_Machine_execute(&sm);
	Gexfra_init(&gxf);
	Gexfra_add_state_machine(&gxf, &sm);
	printf("%s\n", "Executing functions\n");
	EvHandler_add_event_to_list(&(gxf.evh), ev);
	gxf.must_run = true;
	Gexfra_run(&gxf);
}