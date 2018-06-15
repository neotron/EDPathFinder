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
#include <QItemSelection>

class RouteResult;
class RouteTableModel;

namespace Ui {
    class ValuablePlanetRouteViewer;
}


class ValuablePlanetRouteViewer : public QMainWindow {
Q_OBJECT

public:
    explicit ValuablePlanetRouteViewer(const RouteResult &result, QWidget *parent = nullptr);

    ~ValuablePlanetRouteViewer() override;

public slots:
    void copySelectedItem();
    void exportAsCSV();
    void exportAsTabNewline();

private:
    Ui::ValuablePlanetRouteViewer *_ui;
    RouteTableModel *_routeModel;

};

