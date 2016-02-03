#Gexfra
Gexfra is a small state machine framework intended to work on embedded systems where RTOS are just too much. It doesn't need dynamic memory management. 
The idea was taken from bachelor courses where our first real project was to design a Snake game on a PIC. A small state machine framework was given to us. Sadly, I've lost this piece of code, so I've taken the decision to rewrite something similar.
##Wath is it?
Gexfra is a small library to handle the state machines developement. State machines are created by registering a function with a specific prototype and implementation format into a `State_Machine_t`object. This object also contains two objects describing the previous and current states.

`State_Machine_t` are then registered into a `Gexfra` object whose role is to execute functions registered into state machines and distribute events to them.

`Gexfra` contains the two pools storing statically a user defined number of events and timers. Functions to manage theses pools are provided by a __Timeout manager__ and an __Event manager__.

## How to use?
*	Set Gexfra settings in the `defines.h` file
```
/* Gexfra CONF */
#define MAX_EV 20
#define MAX_TM 20
#define NUM_OF_STATE_MACHINES 1
#define TM_RESOLUTION 100 /* milliseconds */
```
*	Define your events and states in the `defines.h` file.
```
/* EVENTS */
// EV_INIT is mandatory to start the process
#define EV_INIT 0

// First event, timer in fact
#define TM_5000 1	
#define TM_500  2

#define INTERRUPT 3	// Interrupt event
#define EV_OTHER 4

// States of the first state machine, implementation comming
typedef enum
{
	SM1_state_Init, 
	SM1_other_state,
	SM1_after_interrupt
}State_t;
```
*	Now, it's time to implement a function using this events and states
	First, with timers examples
```c
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
```
   And second with timers
```
static int8_t sm_func(State_Machine_t * sm, Event_t * ev)
{
    Event_t temp_ev;
    Timeout_t tmp_tm;
    printf("Event id is : %d\n", ev->ev_id);
    switch(sm->previous)
    {
        case SM1_state_Init:
            sm->current = SM1_other_state;
            break;
        case SM1_other_state:
            if(ev->ev_id == TM_500)
            {
                sm->current = SM1_after_interrupt;
            }
            break;
        case SM1_after_interrupt:
            if(ev->ev_id == TM_5000)
            {
                sm->current = SM1_other_state;
            }
            break;
    }
    switch(sm->current)
    {
        case SM1_state_Init:
            printf("In initial state\n");
            break;
        case SM1_other_state:
            printf("In other state\n");
            tmp_tm = create_Timeout(create_Event(TM_500), 500);
            Gexfra_add_timeout(&gxf, tmp_tm);
            break;
        case SM1_after_interrupt:
            printf("In 'after interrupt' state\n");
            tmp_tm = create_Timeout(create_Event(TM_5000), 5000);
            Gexfra_add_timeout(&gxf, tmp_tm);
            break;
    }
    sm->previous = sm->current;
    return 0;
}
```
Prototype of this function must look like `int8_t <function_name> (State_Machine_t * sm, Event_t * ev)`. 

*	The `Gexfra` object must be a global object. 
*	Here is the minimal __main__ function to create and exectute a single state machine:
```
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "defines.h"
#include "gexfra.h"

static Gexfra gxf;

void main(void)
{
	State_Machine_t sm;
	State_Machine_init(&sm, 1, SM1_state_Init, sm_func);

	Gexfra_init(&gxf);
	Gexfra_add_state_machine(&gxf, &sm);

	Gexfra_run(&gxf);
}
```
*	The final point is to call the `void TmHandler_decremente_timer(Time_Handler * tmh)` function from a timer interrupt. A small Qt project can be found in this repository for testing timeout capabilities. This can explain why `Gexfra` object must be global : allow to be accessed from the interrupt routines

##State of the project
For the moment, two very simple state machines have been implemented, one with two states and each one producing an event to the other state and one simulating a monostable vibrator with different timeouts. 
The next stage will be to create a state machine working on a stm32f429i-disco, with timeouts and buttons events, just to toggle some leds first.

Some optimisations for the memory pools must be done, too. For an efficient work, the event and timeout handling time overhead must be as little as possible. This is not the case at this time. 


