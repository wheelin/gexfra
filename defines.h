#ifndef DEFINES_H
#define DEFINES_H

#include <stdbool.h>
#include <stdint.h>

/* Gexfra CONF */
#define MAX_EV 20
#define MAX_TM 20
#define NUM_OF_STATE_MACHINES 1
#define TM_RESOLUTION 100 /* milliseconds */

/* EVENTS */
#define EV_INIT 0

#define TM_5000 1
#define TM_500  2

#define INTERRUPT 3
#define EV_OTHER 4

typedef enum
{
	SM1_state_Init,
	SM1_other_state,
	SM1_after_interrupt
}State_t;

#endif /* DEFINES_H */
