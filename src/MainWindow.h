//
//  Copyright (C) 2016  David Hedbor <neotron@gmail.com>
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
#include "System.h"
#include "TSPWorker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void createRoute();
    void routeCalculated(const RouteResult &route);
    void updateFilters();
    void updateSystemCoordinates();
    void systemCoordinatesReceived(double x, double y, double z);

private:
    Ui::MainWindow *_ui;

    void cleanupCheckboxes();


    void buildLookupMap();

    void loadSystems();
private:
    QMap<QString, SettlementFlags> _flagsLookup;

    SystemList _systems;
    SystemList _filteredSystems;
    int32 _matchingSettlementCount;
};
