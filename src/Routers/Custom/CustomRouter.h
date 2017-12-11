#ifndef CUSTOMROUTER_H
#define CUSTOMROUTER_H

#include <QMainWindow>
#include <ui_CustomRouter.h>
#include "MissionScanner.h"
#include "PresetsTableModel.h"
#include "SystemEntryCoordinateResolver.h"
#include "PresetsManager.h"

class RouteProgressAnnouncer;

namespace Ui {
    class CustomRouter;
}

class CustomRouter : public QMainWindow {
Q_OBJECT


public:
    explicit CustomRouter(QWidget *parent, AStarRouter *router, const SystemList &systems);

    ~CustomRouter() override;

public slots:

    void refreshMissions();
    void updateMissionTable();
    void optimizeRoute();
    void clearCustom();
    void addStop();
    void exportAsCSV();
    void exportAsTabNewline();
    void importSystems();
    void changeCommander(const QString &cmdr);

private:
    void refreshTableView(QAbstractItemModel *model) const;

    void showMessage(const QString &message, int timeout = 10000);

    void routeCalculated(const RouteResult &route);

private slots:
    void onSystemLookupInitiated(const QString &systemName);

    void onSystemCoordinatesReceived(const System &system);

    void onSystemCoordinatesRequestFailed(const QString &systemName);

    void onSystemCoordinatesReceivedCustom(const System &system);

    void copySelectedItem();

private:
    Ui::CustomRouter *_ui;
    MissionScanner    _scanner;
    AStarRouter       *_router;
    const SystemList  &_systems;
    PresetsTableModel *_currentModel;
    bool _routingPending;
    QSet<PresetEntry>     _customStops;
    RouteProgressAnnouncer *_progressAnnouncer;
    PresetsManager *_presetsManager;
    QList<SystemEntryCoordinateResolver*> _resolvers;

    bool resolversComplete();
};

#endif // CUSTOMROUTER_H
