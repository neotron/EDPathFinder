#ifndef CUSTOMROUTER_H
#define CUSTOMROUTER_H

#include <QMainWindow>
#include "MissionScanner.h"
#include "MissionTableModel.h"

namespace Ui {
    class MissionRouter;
}

class MissionRouter : public QMainWindow {
Q_OBJECT


public:
    explicit MissionRouter(QWidget *parent, AStarRouter *router, const SystemList &systems);

    ~MissionRouter();

public slots:

    void refreshMissions();

    void updateMissionTable();

    void optimizeRoute();

    void addStop();

    void clearCustom();


private:
    void refreshTableView(QAbstractItemModel *model) const;

    void showMessage(const QString &message, int timeout = 10000);


    void routeCalculated(const RouteResult &route);

    void downloadSystemCoordinates(const QString &system);

    void systemCoordinatesReceived(const System &system);

    void systemCoordinatesRequestFailed(const QString &);

    Ui::MissionRouter *_ui;
    MissionScanner    _scanner;
    AStarRouter       *_router;
    const SystemList  &_systems;
    MissionTableModel *_currentModel;
    QSet<QString>     _pendingLookups;
    bool _routingPending;
    QSet<QString>     _customStops;
};

#endif // CUSTOMROUTER_H
