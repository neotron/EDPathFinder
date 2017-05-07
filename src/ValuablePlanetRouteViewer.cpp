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
#include "ValuablePlanetRouteViewer.h"
#include "ui_ValuablePlanetRouteViewer.h"


ValuablePlanetRouteViewer::ValuablePlanetRouteViewer(const RouteResult &result, QWidget *parent)
        : QMainWindow(parent), _ui(new Ui::ValuablePlanetRouteViewer) {
    _ui->setupUi(this);
    QTableView *table = _ui->tableView;
    _routeModel = new RouteTableModel(this, result);
    _routeModel->setResultType(RouteTableModel::ResultTypeValuableSystems);
    table->setModel(_routeModel);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->adjustSize();
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setSelectionMode(QTableView::SingleSelection);
    QHeaderView *horizontalHeader = table->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader->setStretchLastSection(true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    connect(table->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(copySelectedItem()));
    table->selectRow(0);
}

ValuablePlanetRouteViewer::~ValuablePlanetRouteViewer() {
    delete _ui;
}

void ValuablePlanetRouteViewer::copySelectedItem() {
    //updateSettlementInfo();
}

