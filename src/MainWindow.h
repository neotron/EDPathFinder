//
//  Copyright (C) 2016-2017  David Hedbor <neotron@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#pragma once

#include <QMainWindow>
#include <QMap>
#include <deps/EDJournalQT/src/JournalWatcher.h>
#include "System.h"
#include "CommanderInfo.h"

class RouteResult;

class AStarRouter;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

public slots:
    void systemsLoaded(const SystemList &systems);

    void createRoute();

    void routeCalculated(const RouteResult &route);

    void updateFilters();

    void updateSystemCoordinates();

    void systemCoordinatesReceived(const System &system);

    void systemCoordinatesRequestFailed();

    void handleEvent(const JournalFile &journal, const Event &event);

    void systemLoadProgress(int progress);
    void systemSortingProgress();

private:
    void cleanupCheckboxes();

    void buildLookupMap();

    void loadCompressedData();

    void downloadSystemCoordinates(const QString &systemName);

    void showMessage(const QString &message, int timeout = 10000);

    void updateSliderParams(int size);

    void updateSystemCoordinateDisplay(const System &system) const;

    Ui::MainWindow *_ui;

    QMap<QString, SettlementFlags> _flagsLookup;

    SystemList    _systems;
    SystemList    _filteredSystems;
    int32         _matchingSettlementCount;
    bool          _routingPending;
    AStarRouter   *_router;
    QSet<QString> _pendingLookups;

    JournalWatcher *_journalWatcher;
    QMap<QString,QMap<QString,QDateTime>> _settlementDates;
    QMap<QString,CommanderInfo> _commanderInformation;
    const QString makeSettlementKey(const System &system, const Planet &planet, const Settlement &settlement) const;

    const QString makeSettlementKey(const QString &system, const QString &planet, const QString &settlement) const;

    void updateSettlementScanDate(const QString &commander, const QString &key, const QDateTime &timestamp);

    void updateCommanderAndSystem();

    bool _loading;

    void updateSystemForCommander(const QString &commander);

    int distanceSliderValue() const;
};
