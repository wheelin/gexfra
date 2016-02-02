#include "timerslot.h"
#include "../../gexfra.h"


TimeoutSlot::TimeoutSlot(Gexfra * gxf)
{
    this->gxf = gxf;
}

void TimeoutSlot::callRefreshTimeouts()
{
    TmHandler_decremente_timer(&(gxf->tmh));
}

TimeoutSlot::~TimeoutSlot()
{

}
