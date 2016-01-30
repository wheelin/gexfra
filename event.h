#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

typedef enum
{

} Event_type_t;

typedef struct _event
{
    uint8_t id;
    Event_type_t type;
}Event_t;




#endif /* EVENT_H */
