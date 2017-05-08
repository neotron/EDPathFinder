#ifndef VALUEROUTER_H
#define VALUEROUTER_H

#include <QMainWindow>
#include "AStarRouter.h"
#include "MissionScanner.h"
#include "SystemEntryCoordinateResolver.h"
#include "ui_ValueRouter.h"
#include "AbstractBaseWindow.h"
#include "TSPWorker.h"
#include "CommanderInfo.h"

class ValueRouter : public AbstractBaseWindow<Ui::ValueRouter> {
Q_OBJECT

public:
    ValueRouter(QWidget *parent, AStarRouter *router, SystemList *systems);


    virtual ~ValueRouter();

protected slots:
    void handleEvent(const JournalFile &file, const Event &ev);

    void scanJournals();
    virtual void updateFilters();
    virtual void routeCalculated(const RouteResult &route);
    virtual void updateSystem();
private:
    QMap<QString, QSet<QString>> _commanderExploredSystems;
};

#endif // VALUEROUTER_H
