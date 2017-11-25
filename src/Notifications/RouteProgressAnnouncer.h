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
#include <QObject>


class QTableView;
class RouteTableModel;
class JournalFile;
class Event;

class RouteProgressAnnouncer : QObject {
Q_OBJECT
public:
    RouteProgressAnnouncer(QObject *parent, RouteTableModel *routeModel, QTableView *tableView);

    ~RouteProgressAnnouncer() override;

public slots:
    void handleEventSystemOnly(const JournalFile &journal, const Event &event);
    void handleEventSettlements(const JournalFile &journal, const Event &event);
private:
    RouteTableModel *_routeModel;
    QTableView *_tableView;

    size_t findArrivalHop(const JournalFile &journal, bool matchSettlement, bool &matchFound) const;
};


