#ifndef CUSTOMROUTER_H
#define CUSTOMROUTER_H

#include <QMainWindow>
#include <ui_MissionRouter.h>
#include "MissionScanner.h"
#include "MissionTableModel.h"
#include "SystemEntryCoordinateResolver.h"
#include "PresetsManager.h"

class RouteProgressAnnouncer;

namespace Ui {
    class MissionRouter;
}

class MissionRouter : public QMainWindow {
Q_OBJECT


public:
    explicit MissionRouter(QWidget *parent, AStarRouter *router, const SystemList &systems);

    ~MissionRouter() override;

public slots:

    void refreshMissions();
    void updateMissionTable();
    void optimizeRoute();
    void clearCustom();
    void addStop();
    void exportAsCSV();
    void exportAsTabNewline();
    void importSystems();

private:
    void refreshTableView(QAbstractItemModel *model) const;

    void showMessage(const QString &message, int timeout = 10000);

    void routeCalculated(const RouteResult &route);

private slots:
    void onSystemLookupInitiated(const QString &systemName);

    void onSystemCoordinatesReceived(const System &system);

    void onSystemCoordinatesRequestFailed(const QString &systemName);

    void copySelectedItem();

private:
    Ui::MissionRouter *_ui;
    MissionScanner    _scanner;
    AStarRouter       *_router;
    const SystemList  &_systems;
    MissionTableModel *_currentModel;
    bool _routingPending;
    QSet<QString>     _customStops;
    SystemEntryCoordinateResolver *_systemResolver;
    RouteProgressAnnouncer *_progressAnnouncer;
    PresetsManager _presetManager;
};

#endif // CUSTOMROUTER_H
