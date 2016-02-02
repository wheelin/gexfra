#include <QCoreApplication>
#include <QThread>
#include <QTimer>

#include "../../gexfra.h"
#include "../../defines.h"
#include "timerslot.h"

#include <cstdint>

static Gexfra gxf;

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
            TmHandler_add_timeout_to_list(&(gxf.tmh), tmp_tm);
            break;
        case SM1_after_interrupt:
            printf("In 'after interrupt' state\n");
            tmp_tm = create_Timeout(create_Event(TM_5000), 5000);
            TmHandler_add_timeout_to_list(&(gxf.tmh), tmp_tm);
            break;
    }
    sm->previous = sm->current;
    return 0;
}

class GexfraThread : public QThread
{
private:
    void run() {
        State_Machine_t sm;
        State_Machine_init(&sm, 1, SM1_state_Init, sm_func);
        Gexfra_init(&gxf);
        Gexfra_add_state_machine(&gxf, &sm);
        Gexfra_run(&gxf);
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    GexfraThread gf;
    TimeoutSlot * tms = new TimeoutSlot(&gxf);
    QTimer * timer = new QTimer();
    QObject::connect(&gf, SIGNAL(finished()), &a, SLOT(quit()));
    QObject::connect(timer, SIGNAL(timeout()), tms, SLOT(callRefreshTimeouts()));
    gf.start();
    timer->start(100);
    return a.exec();
}

