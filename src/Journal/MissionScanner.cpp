//
//  Copyright (C) 2017  David Hedbor <neotron@gmail.com>
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

#include <QDebug>
#include <QDirIterator>
#include "MainWindow.h"
#include "MissionScanner.h"
#include "Settings.h"
using namespace Journal;


MissionScanner::MissionScanner(QObject *parent)
        : EventDispatchObject(parent), _commanderMissions() {
}

void MissionScanner::scanJournals() {
    _commanderMissions.clear();
    QDir dir(Settings::restoreJournalPath(), "Journal.*.log");
    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Time | QDir::Reversed);
    auto monitorDate = QDateTime::currentDateTime().addDays(-30); // Missions last at most a month.

    for (const auto &entry: list) {
        if (entry.lastModified() < monitorDate) {
            continue;
        }
        auto file = entry.absoluteFilePath();
        Journal::JFile journalFile(file);
        journalFile.registerHandler(this);
        journalFile.parse();
    }
}


void MissionScanner::onEventGeneric(Event *event) {
    QDateTime now = QDateTime::currentDateTimeUtc();
    auto file(event->file());
    _recentCommander = file->commander();
    switch (event->journalEvent()) {
    case Event::MissionAccepted:
         //   qDebug() << ev->obj();
        if (event->date("Expiry") >= now) {
            auto destination = event->string("DestinationSystem");
            if (!destination.isEmpty() && file->system() != destination) {
                auto mission = Mission(destination, file->system(), event->string("DestinationStation"));
                _commanderMissions[file->commander()][event->integer("MissionID")] = mission;
            }
        }
        break;
    case Event::MissionRedirected: {
        auto destination = event->string("NewDestinationSystem");
        auto missionId = event->integer("MissionID");
        if (!destination.isEmpty() && _commanderMissions[file->commander()].contains(missionId)) {
            _commanderMissions[file->commander()][missionId]._destination = destination;
        }
    }
        break;
    case Event::MissionAbandoned:
    case Event::MissionFailed:
    case Event::MissionCompleted:
        _commanderMissions[file->commander()].remove(event->integer("MissionID"));
        break;
    case Event::FSDJump:
    case Event::Location:
        //      qDebug() << file->commander() << " in "<<file->system();
        _commanderLastSystem[file->commander()] = file->system();
        break;
    default:
        break;
    }
}

const QString &MissionScanner::recentCommander() const { return _recentCommander; }

const QString MissionScanner::commanderSystem(const QString &cmdr) { return _commanderLastSystem[cmdr]; }

const QList<Mission> MissionScanner::commanderMission(const QString &cmdr) { return _commanderMissions[cmdr].values(); }


