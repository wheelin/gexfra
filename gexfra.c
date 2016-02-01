#include "gexfra.h"

#include "gexfra.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

/**********************************************************************************/
/**********************************************************************************/
void State_Machine_init(State_Machine_t * sm,
	uint8_t id, 
	State_t initial_state,
	int8_t (*state_machine_function)(State_Machine_t * sm, Event_t * ev))
{
	sm->previous = initial_state;
	sm->current = initial_state;
	sm->id = id;
	sm->state_machine_function = state_machine_function;
}

/**********************************************************************************/
/**********************************************************************************/
void EvHandler_init(Ev_Handler * evh)
{
	uint32_t i;
	for (i = 0; i < MAX_EV; ++i)
	{
		evh->ev_list[i].ev_id = 0;
		evh->ev_list[i].used = false;
	}
}

Event_t EvHandler_get_next_event(Ev_Handler * evh)
{
	uint32_t i;
	Event_t ret_ev = evh->ev_list[0];
	for(i = 1; i < MAX_EV; i++)
	{
		evh->ev_list[i - 1] = evh->ev_list[i];
	}
	evh->ev_list[MAX_EV - 1] = (Event_t){0, false};
	return ret_ev;
}

int8_t EvHandler_add_event_to_list(Ev_Handler * evh, Event_t ev)
{
	uint32_t i;
	for(i = 0; i < MAX_EV; i++)
	{
		if(evh->ev_list[i].used == false)
		{
			break;
		}
	}
	if(i == MAX_EV - 1 && evh->ev_list[MAX_EV - 1].used == true)
	{
		return -1;
	}
	evh->ev_list[i] = ev;
	return 0;
}

/**********************************************************************************/
/**********************************************************************************/
void TmHandler_init(Time_Handler * tmh)
{
	uint32_t i;
	for(i = 0; i < MAX_TM; i++)
	{
		tmh->tm_list[i].id = 0;
		tmh->tm_list[i].msec = 0;
		tmh->tm_list[i].used = false;
	}
}

void TmHandler_set_event_handler(Time_Handler * tmh, Ev_Handler * evh)
{
	tmh->evh = evh;
}

void TmHandler_decremente_timer(Time_Handler * tmh)
{
	uint32_t i;
	for(i = 0; i < MAX_TM; i++)
	{
		if(tmh->tm_list[i].used == true)
		{
			tmh->tm_list[i].msec -= TM_RESOLUTION;
		}
	}
	for(i = 0; i < MAX_TM; i++)
	{
		if(tmh->tm_list[i].used == true && tmh->tm_list[i].msec <= 0)
		{
			EvHandler_add_event_to_list(tmh->evh, tmh->tm_list[i].ev);
			tmh->tm_list[i].used = false;
		}
	}
}

int8_t TmHandler_add_timeout_to_list(Time_Handler * tmh, Timeout_t tm)
{
	uint32_t i;
	for(i = 0; i < MAX_TM; i++)
	{
		if(tmh->tm_list[i].used == false)
		{
			break;
		}
	}
	if(i == (MAX_TM - 1) && tmh->tm_list[MAX_TM - 1].used == true)
	{
		return -1;
	}
	tmh->tm_list[i] = tm;
	return 0;
}

/**********************************************************************************/
/**********************************************************************************/
void Gexfra_init(Gexfra * gxf)
{
	uint32_t i;
	gxf->must_run = true;
	for(i = 0; i < NUM_OF_STATE_MACHINES; i++)
	{
		gxf->machines[i] = NULL;
	}
	gxf->num_of_state_machines_registered = 0;
	EvHandler_init(&gxf->evh);
	TmHandler_init(&gxf->tmh);
}

int8_t Gexfra_add_state_machine(Gexfra * gxf, State_Machine_t * sm)
{
	uint32_t i;
	for(i = 0; i < NUM_OF_STATE_MACHINES; i++)
	{
		if(gxf->machines[i] == NULL)
		{
			break;
		}
	}
	if(i == (NUM_OF_STATE_MACHINES - 1) && 
		gxf->machines[NUM_OF_STATE_MACHINES - 1] != NULL)
	{
		return -1;
	}
	gxf->num_of_state_machines_registered++;
	gxf->machines[i] = sm;
	return 0;
}

void Gexfra_del_state_machine(Gexfra * gxf, State_Machine_t * sm)
{
	uint32_t i;
	for(i = 0; i < NUM_OF_STATE_MACHINES; i++)
	{
		if(sm->id == gxf->machines[i]->id)
		{
			gxf->machines[i] = NULL;
			gxf->num_of_state_machines_registered--;
		}
	}
}

void Gexfra_run(Gexfra * gxf)
{
	uint32_t i;
	Event_t ev;
	while(gxf->must_run == true)
	{
		ev = EvHandler_get_next_event(&(gxf->evh));
		for(i = 0; i < gxf->num_of_state_machines_registered; i++)
		{
			if(gxf->machines[i]->state_machine_function(gxf->machines[i], &ev) < 0)
			{
				return;
			}
		}
		sleep(1);
	}
}