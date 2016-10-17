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
#include "RouteViewer.h"
#include "ui_RouteViewer.h"
#include "RouteTableModel.h"

RouteViewer::RouteViewer(const RouteResult &result, QWidget *parent) : QMainWindow(parent), _ui(new Ui::RouteViewer) {
    _ui->setupUi(this);
    QTableView *table = _ui->tableView;
    table->setModel(new RouteTableModel(this, result));
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->adjustSize();
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setSelectionMode(QTableView::SingleSelection);
    QHeaderView *horizontalHeader = table->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::Stretch);
    setAttribute(Qt::WA_DeleteOnClose, true);

    connect(table->selectionModel(), SIGNAL(selectionChanged(
                                                    const QItemSelection &, const QItemSelection &)), this,
            SLOT(copySelectedItem()));
}

RouteViewer::~RouteViewer() {
    delete _ui;
}

void RouteViewer::copySelectedItem() {
    auto indices = _ui->tableView->selectionModel()->selectedIndexes();;
    if(indices.count() >= 1) {
        const auto &selectedIndex(indices[0]);
        if(selectedIndex.row() > 0) {
            const auto &cellValue = _ui->tableView->model()->data(selectedIndex).toString();
            if(cellValue.length()) {
                QApplication::clipboard()->setText(cellValue);
                _ui->statusbar->showMessage(QString("Copied system name `%1' to the system clipboard.").arg(cellValue));
            }
        }
    }
}


