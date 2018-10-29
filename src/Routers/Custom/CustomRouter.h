#pragma once

#include <QMainWindow>
#include <ui_CustomRouter.h>
#include "MissionScanner.h"

class PresetsTableModel;
class RouteProgressAnnouncer;
class BearingCalculator;
class SystemEntryCoordinateResolver;
class PresetsManager;
class RouteResult;

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
    void openBearingCalculator();
    void fuzzyImportSystemsFromPasteBoard();
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

    void finishAsyncSetup();

protected:
    void showEvent(QShowEvent *event) override;



private:
    bool resolversComplete();

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
    BearingCalculator *_bearingCalculator;
};
