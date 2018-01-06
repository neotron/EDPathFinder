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
#include <QApplication>
#include <QClipboard>
#include <QTableView>
#include <deps/EDJournalQT/src/Event.h>
#include <deps/EDJournalQT/src/JournalFile.h>
#include "LiveJournal.h"
#include "RouteTableModel.h"
#include "RouteProgressAnnouncer.h"
#include "MessageToaster.h"


RouteProgressAnnouncer::RouteProgressAnnouncer(QObject *parent, RouteTableModel *routeModel, QTableView *tableView)
        : QObject(parent), _routeModel(routeModel), _tableView(tableView) {

    switch(_routeModel->resultType()) {
    case RouteTableModel::ResultTypeSettlement:
        connect(LiveJournal::instance(), SIGNAL(onEvent(const JournalFile &, const Event &)),
                this, SLOT(handleEventSettlements(const JournalFile &, const Event &)));
        break;
    case RouteTableModel::ResultTypePresets:
    case RouteTableModel::ResultTypeSystemsOnly:
    case RouteTableModel::ResultTypeValuableSystems:
        connect(LiveJournal::instance(), SIGNAL(onEvent(const JournalFile &, const Event &)),
                this, SLOT(handleEventSystemOnly( const JournalFile &, const Event &)));
        break;
    }
}

RouteProgressAnnouncer::~RouteProgressAnnouncer() = default;


void RouteProgressAnnouncer::handleEventSystemOnly(const JournalFile &journal, const Event &event) {
    bool hasArrived = false;
    size_t arrivedAt = 0;

    switch(event.type()) {
    case EventTypeFSDJump:
    case EventTypeLocation:
        // This means we changed system or opened the game.
        arrivedAt = findArrivalHop(journal, false, hasArrived);
        if(hasArrived) {
            auto &route = _routeModel->result().route();
            _tableView->selectRow(static_cast<int>(arrivedAt));
            QString nextSystem;
            for(size_t i = arrivedAt + 1; i < route.size(); i++) {
                auto system = route[i][0];
                if(system != route[arrivedAt][0]) {
                    nextSystem = system;
                }
            }

            if(nextSystem.isEmpty()) {
                // Last route
                MessageToaster::send("Final destination reached.",
                                     QString("You have arrived at %1, which is is the final route destination.").arg(
                                             route[arrivedAt][0]));
            } else {
                MessageToaster::send(QString("You have arrived at %1.").arg(route[arrivedAt][0]),
                                     QString("The next system in your route, %1, has been copied to the clipboard.").arg(
                                             nextSystem));
                QApplication::clipboard()->setText(nextSystem);
            }
        }
        break;
    default:
        break;
    }
}

void RouteProgressAnnouncer::handleEventSettlements(const JournalFile &journal, const Event &event) {
    bool hasArrived = false;
    size_t arrivedAt = 0;
    auto &route = _routeModel->result().route();

    switch(event.type()) {
    case EventTypeFSDJump:
    case EventTypeLocation:
        // This means we changed system or opened the game.
        arrivedAt = findArrivalHop(journal, false, hasArrived);
        if(hasArrived) {
            auto &currentHop = route[arrivedAt];
            QString settlement = QString("Make your way towards %1").arg(currentHop[2]);
            if(!currentHop[1].isEmpty()) {
                settlement += QString(", on planet %1").arg(currentHop[1]);
            }
            settlement += ".";
            MessageToaster::send(QString("You have arrived at %1.").arg(currentHop[0]), settlement);
        }
        break;
    case EventTypeApproachSettlement:
        findArrivalHop(journal, true, hasArrived);
        break;
    case EventTypeTouchdown:
        arrivedAt = findArrivalHop(journal, true, hasArrived);
        if(hasArrived) {
            auto &currentHop = route[arrivedAt];
            if(arrivedAt < route.size() - 1) {
                // Not last hop
                auto &nextHop = route[arrivedAt + 1];
                QString next = QString("Your next stop is %1, ").arg(nextHop[2]);
                if(!nextHop[1].isEmpty()) {
                    next += QString("on planet %1, ").arg(nextHop[1]);
                }
                if(nextHop[0] == currentHop[0]) {
                    // Same system.
                    next += "in this system.";
                } else {
                    next += QString("in %1. System name copied to the clipboard.").arg(nextHop[0]);
                    QApplication::clipboard()->setText(nextHop[0]);
                }
                MessageToaster::send(QString("You have arrived at %1.").arg(currentHop[2]), next);
            } else {
                // last hop
                MessageToaster::send(QString("You have arrived at %1.").arg(currentHop[2]),
                                     "This is the last settlement in your route.");
            }
        }
        break;
    default:
        break;
    }
}

size_t RouteProgressAnnouncer::findArrivalHop(const JournalFile &journal, bool matchSettlement, bool &matchFound) const {
    auto route = _routeModel->result().route();
    size_t matchedHop = 0;
    for(size_t hop = 0; hop < route.size(); hop++) {
        if(!route[hop][0].compare(journal.system(), Qt::CaseInsensitive)) {
            if(matchSettlement && _routeModel->resultType() == RouteTableModel::ResultTypeSettlement) {
                if(route[hop][2].compare(journal.settlement().split(" +")[0], Qt::CaseInsensitive)) {
                    continue; // didn't match settlement
                }
            }
            matchedHop = hop;
            matchFound = true;
            _tableView->selectRow(static_cast<int>(hop));
            break;
        }
    }
    return matchedHop;
}
