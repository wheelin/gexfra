#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "event.h"
#include <stdint.h>

typedef struct _timeout
{
    uint8_t id;
    uint32_t msec;
    Event_t * associated_ev;
}Timeout_t;



#endif /* TIMEOUT_H */
