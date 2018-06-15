#pragma once


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

    ~ValueRouter() override;

protected slots:
    void updateFilters() override;
    void routeCalculated(const RouteResult &route) override;
    void onRouterCreated(TSPWorker *worker) override;

    void scanJournals();
    void updateSystem();
    void saveSettings() const override;

    void finishAsyncSetup();
protected:
    void showEvent(QShowEvent *event) override;
    void onEventGeneric(Journal::Event *event) override;

private:
    void restoreSettings() const;

    QMap<QString, QSet<QString>> _commanderExploredSystems;
    SystemEntryCoordinateResolver *_systemResolverDestination;

};
