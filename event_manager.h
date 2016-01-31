#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "event.h"

#define MAX_EVENTS 20

typedef struct _event_manager
{
    Event_t event_list[MAX_EVENTS];
}Event_manager;

void EvMan_init(Event_manager * em);
Event_t EvMan_get_next_event(Event_manager * em);
uint8_t EvMan_add_new_event(Event_manager * em, Event_t ev);

#endif /* EVENT_MANAGER_H */
