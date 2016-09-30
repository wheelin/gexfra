#include "gexfra.h"

#include "gexfra.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#ifdef __linux
#include <time.h>
#endif

Event_t create_Event(uint8_t id) {
    Event_t ev;
    ev.ev_id = id;
    return ev;
}

Timeout_t create_Timeout(Event_t ev, uint32_t duration) {
    Timeout_t tm;
    tm.id = ev.ev_id;
    tm.ev = ev;
    tm.msec = duration;

    return tm;
}

/**********************************************************************************/
/**********************************************************************************/
// PUBLIC FUNCTION
// This function initializes the State machine object with :
// 		- uint8_t id : sm identifier, useful to
//			unregister state machine when useless
//		- State_t initial_state : state used to initialize the previous
// and
//			current state
// 		- int8_t (*state_machine_function)(State_Machine_t * sm, Event_t
// *
// ev)
// 			function pointer pointing on the function describing the
// state
// machine
void State_Machine_init(State_Machine_t *sm, uint8_t id, State_t initial_state,
                        int8_t (*state_machine_function)(State_Machine_t *sm,
                                                         Event_t *ev)) {
    sm->previous_state = initial_state;
    sm->current_state = initial_state;
    sm->id = id;
    sm->state_machine_function = state_machine_function;
}

/**********************************************************************************/
/**********************************************************************************/
// This function initializes the event pool by setting all event ids to zero
// And the flag that indicates if an event is used at the moment to false.
void EvHandler_init(Ev_Handler *evh) {
    uint32_t i;
    for (i = 0; i < MAX_EV; ++i) {
        evh->ev_list[i].ev_id = 0;
        evh->ev_list[i].used = false;
    }
    evh->num_of_evs = 0;
}

// This function returns the first event from the list, no matter if this event
// is in use or not.
// It then rearranges the array by pushing the next event to the first place.
// Last event is automatically initialized with the common
// values for an unused event.
Event_t EvHandler_get_next_event(Ev_Handler *evh) {
    uint32_t i;
    Event_t ret_ev;
    if (evh->num_of_evs == 0) {
        return (Event_t){0, false};
    } else {
        ret_ev = evh->ev_list[0];
        for (i = 1; i < evh->num_of_evs; i++) {
            evh->ev_list[i - 1] = evh->ev_list[i];
        }
        evh->ev_list[evh->num_of_evs - 1] = (Event_t){0, false};
        (evh->num_of_evs)--;
        return ret_ev;
    }
}

// This function adds a new event to the event list. This array is managed like
// a FIFO.
// It returns -1 if there is no more empty place in the list, else number of
// events
// in the list.
int8_t EvHandler_add_event_to_list(Ev_Handler *evh, Event_t ev) {
    if (evh->num_of_evs == MAX_EV) {
        return -1;
    } else {
        ev.used = true;
        evh->ev_list[evh->num_of_evs] = ev;
        (evh->num_of_evs)++;
        return evh->num_of_evs;
    }
}

/**********************************************************************************/
/**********************************************************************************/
// This function initializes the timeout pool with the unused timeout values
void TmHandler_init(Time_Handler *tmh) {
    uint32_t i;
    for (i = 0; i < MAX_TM; i++) {
        tmh->tm_list[i].id = 0;
        tmh->tm_list[i].msec = 0;
        tmh->tm_list[i].used = false;
    }
    tmh->num_of_tms = 0;
}

// The TmHandler needs to have a pointer on the EvHandler in order to create a
// new event when a timeout is issued.
void TmHandler_set_event_handler(Time_Handler *tmh, Ev_Handler *evh) {
    tmh->evh = evh;
}

// This function is called by a timer interrupt handler for example. This will
// decrement all the used timeouts inside the array until one has its value
// equal
// or less than zero. Then, a new event is created for each of this timeouts.
// The used flag is then set to false to indicate that this tiemout has already
// been issued and is ready to accept a new timeout
void TmHandler_decremente_timer(Time_Handler *tmh) {
    uint32_t i;
    for (i = 0; i < MAX_TM; i++) {
        if (tmh->tm_list[i].used == true) {
            tmh->tm_list[i].msec -= TM_RESOLUTION;
        }
        if (tmh->tm_list[i].used == true && tmh->tm_list[i].msec <= 0) {
            EvHandler_add_event_to_list(tmh->evh, tmh->tm_list[i].ev);
            tmh->tm_list[i].used = false;
            (tmh->num_of_tms)--;
        }
    }
}

// This function adds a new timeout to the list. The first hole in the list
// is filled with this timeout.
// If we have reached the end of the array without finding a hole, we return -1.
// Else, if the timeout has correctly been added, we return the number of
// timeouts in the list.
int8_t TmHandler_add_timeout_to_list(Time_Handler *tmh, Timeout_t tm) {
    uint32_t i;
    for (i = 0; i < MAX_TM; i++) {
        if (tmh->tm_list[i].used == false) {
            break;
        }
    }
    if (i == (MAX_TM - 1) && tmh->tm_list[MAX_TM - 1].used == true) {
        return -1;
    }
    tm.used = true;
    tmh->tm_list[i] = tm;
    (tmh->num_of_tms)++;
    return tmh->num_of_tms;
}

/**********************************************************************************/
/**********************************************************************************/
// PUBLIC FUNCTION
// This function initializes the state machine array
// and the event and timeout pools
void Gexfra_init(Gexfra *gxf) {
    uint32_t i;
    gxf->must_run = true;
    for (i = 0; i < NUM_OF_STATE_MACHINES; i++) {
        gxf->machines[i] = NULL;
    }
    gxf->num_of_state_machines_registered = 0;
    EvHandler_init(&gxf->evh);
    TmHandler_init(&gxf->tmh);
}

// PUBLIC FUNCTION
// This function allows a state machine to be registered in order to be executed
// when Gexfra is running
int8_t Gexfra_add_state_machine(Gexfra *gxf, State_Machine_t *sm) {
    uint32_t i;
    for (i = 0; i < NUM_OF_STATE_MACHINES; i++) {
        if (gxf->machines[i] == NULL) {
            break;
        }
    }
    if (i == (NUM_OF_STATE_MACHINES - 1) &&
        gxf->machines[NUM_OF_STATE_MACHINES - 1] != NULL) {
        return -1;
    }
    gxf->num_of_state_machines_registered++;
    gxf->machines[i] = sm;
    return 0;
}

// PUBLIC FUNCTION
// This function unregisters a state machine that became useless.
void Gexfra_del_state_machine(Gexfra *gxf, State_Machine_t *sm) {
    uint32_t i;
    for (i = 0; i < NUM_OF_STATE_MACHINES; i++) {
        if (sm->id == gxf->machines[i]->id) {
            gxf->machines[i] = NULL;
            gxf->num_of_state_machines_registered--;
        }
    }
}

// PUBLIC FUNCTION
// Wrapper around the EvHandler function
int8_t Gexfra_add_event(Gexfra *gxf, Event_t ev) {
    return EvHandler_add_event_to_list(&(gxf->evh), ev);
}

// PUBLIC FUNCTION
// Wrapper around the TmHandler function
int8_t Gexfra_add_timeout(Gexfra *gxf, Timeout_t tm) {
    return TmHandler_add_timeout_to_list(&(gxf->tmh), tm);
}

// PUBLIC FUNCTION
// This function executes all the registered functions
// and distributes events to all of them
void Gexfra_run(Gexfra *gxf) {
    uint32_t i;
    Event_t ev;

#ifdef __linux
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 5000;
#endif

    TmHandler_set_event_handler(&(gxf->tmh), &(gxf->evh));
    EvHandler_add_event_to_list(&(gxf->evh), (Event_t){EV_INIT, true});
    while (gxf->must_run == true) {
        ev = EvHandler_get_next_event(&(gxf->evh));
        for (i = 0; i < gxf->num_of_state_machines_registered; i++) {
            if (ev.used == false)
                break;
            if (gxf->machines[i]->state_machine_function(gxf->machines[i],
                                                         &ev) < 0) {
                return;
            }
        }

#ifdef __linux
        nanosleep(&ts, NULL);
#endif
    }
}
