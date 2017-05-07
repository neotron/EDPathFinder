#ifndef VALUEROUTER_H
#define VALUEROUTER_H

#include <QMainWindow>
#include "AStarRouter.h"
#include "MissionScanner.h"
#include "SystemEntryCoordinateResolver.h"
#include "ui_ValueRouter.h"
#include "AbstractBaseWindow.h"
#include "TSPWorker.h"

class ValueRouter : public AbstractBaseWindow<Ui::ValueRouter> {
Q_OBJECT

public:
    ValueRouter(QWidget *parent, AStarRouter *router, SystemList *systems);


    virtual ~ValueRouter();


protected slots:

    virtual void updateFilters();
    virtual void routeCalculated(const RouteResult &route);
};

#endif // VALUEROUTER_H
