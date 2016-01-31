#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <stdint.h>
#include "timeout.h"

#define MAX_TIMEMOUT 20

typedef struct _timer_manager
{
    Timeout_t timeout_list[MAX_TIMEMOUT];
}Timer_manager;

uint8_t TiMan_decremente_timeouts(Timer_manager * tm);

#endif /* TIMER_MANAGER_H */
