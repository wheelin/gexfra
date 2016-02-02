#ifndef TIMERSLOT_H
#define TIMERSLOT_H

#include <QObject>

#include "../../gexfra.h"

class TimeoutSlot : public QObject
{
    Q_OBJECT
private:
    Gexfra * gxf;
public:
    TimeoutSlot(Gexfra * gxf);
public slots:
    void callRefreshTimeouts();
public:
    ~TimeoutSlot();
};

#endif // TIMERSLOT_H
