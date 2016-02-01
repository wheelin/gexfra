#include "gexfra.h"

#include "gexfra.h"
#include <stdbool.h>

/**********************************************************************************/
/**********************************************************************************/
void State_Machine_init(State_Machine_t * sm)
{
	sm->execute = false;
	sm->state_machine_function = NULL;
}

void State_Machine_set_function(uint8_t (*state_machine_function)(Event_t * ev))
{
	sm->state_machine_function = state_machine_function;
}

void State_Machine_execute(State_Machine_t * sm)
{
	sm->execute = true;
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
		ev->ev_list[i - 1] = ev->ev_list[i];
	}
	ev->ev_list[MAX_EV - 1] = {0, false};
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
	for(i = 0; i < NUM_OF_STATE_MACHINES)
	{
		gxf->machines[i] = NULL;
	}
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
		}
	}
}

void Gexfra_run(Gexfra * gxf)
{
	uint32_t i;
	while(gxf->must_run == true)
	{
		for(i = 0; i < NUM_OF_STATE_MACHINES; i++)
		{
			if(gxf->machines[i]->state_machine_function(&EvHandler_get_next_event(gxf->evh)) < 0)
			{
				return;
			}
		}
	}
}