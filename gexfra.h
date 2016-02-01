#ifndef GEXFRA_H
#define GEXFRA_H

#include <stdbool.h>
#include <stdint.h>
#include "defines.h"
/**********************************************************************************/
/**********************************************************************************/
typedef struct _event
{
	uint8_t ev_id;
	bool used;
}Event_t;

typedef struct _timeout
{
	uint8_t id;
	uint32_t msec;
	Event_t ev;
	bool used;
}Timeout_t;

/**********************************************************************************/
/**********************************************************************************/
typedef struct _state_machine
{
	bool execute;
	uint8_t id;
	int8_t (*state_machine_function)(Event_t * ev);
}State_Machine_t;

void State_Machine_init(State_Machine_t * sm);
void State_Machine_set_function(State_Machine_t * sm, 
	int8_t (*state_machine_function)(Event_t * ev));
void State_Machine_execute(State_Machine_t * sm);

/**********************************************************************************/
/**********************************************************************************/
typedef struct _ev_handler
{
	Event_t ev_list[MAX_EV];
}Ev_Handler;

void EvHandler_init(Ev_Handler * evh);
Event_t EvHandler_get_next_event(Ev_Handler * evh);
int8_t EvHandler_add_event_to_list(Ev_Handler * evh, Event_t ev);

/**********************************************************************************/
/**********************************************************************************/
typedef struct _tm_handler
{
	Ev_Handler * evh;
	Timeout_t tm_list[MAX_TM];
}Time_Handler;

void TmHandler_init(Time_Handler * tmh);
void TmHandler_set_event_handler(Time_Handler * tmh, Ev_Handler * evh);
void TmHandler_decremente_timer(Time_Handler * tmh);
int8_t TmHandler_add_timeout_to_list(Time_Handler * tmh, Timeout_t tm);

/**********************************************************************************/
/**********************************************************************************/
typedef struct _gexfra
{
	bool must_run;
	State_Machine_t * machines[NUM_OF_STATE_MACHINES];
	Ev_Handler evh;
	Time_Handler tmh;
}Gexfra;

void Gexfra_init(Gexfra * gxf);
int8_t Gexfra_add_state_machine(Gexfra * gxf, State_Machine_t * sm);
void Gexfra_del_state_machine(Gexfra * gxf, State_Machine_t * sm);
void Gexfra_run(Gexfra * gxf);
void Gexfra_stop(Gexfra * gxf);


#endif /* GEXFRA_H */