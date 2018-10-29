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

#include "CustomRouteTableWidget.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QMenu>

CustomRouteTableWidget::CustomRouteTableWidget(QWidget *parent) : QTableView(parent) {
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QWidget::customContextMenuRequested, [=](const QPoint &pt) {
        QMenu contextMenu("Edit Entry", this);
        QAction action1("Remove Selected", this);
        connect(&action1, SIGNAL(triggered()), this, SLOT(deleteSelectedEntry()));
        contextMenu.addAction(&action1);
        contextMenu.exec(mapToGlobal(pt));

    });

}

void CustomRouteTableWidget::keyReleaseEvent(QKeyEvent *event) {
    if(event->matches(QKeySequence::Delete)
       || event->matches(QKeySequence::Backspace)
       || event->key()== Qt::Key_Backspace) {
        deleteSelectedEntry();
    }
    QWidget::keyReleaseEvent(event);
}

void CustomRouteTableWidget::deleteSelectedEntry() const {
    if(!selectedIndexes().empty()) {
        emit deleteStop(model()->data(model()->index(currentIndex().row(), 0), Qt::DisplayRole).toString());
    }
}

