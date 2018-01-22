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

#pragma once


#include <QObject>
#include <QMap>
#include <Events.h>

struct Mission {
    Mission(const QString destination, const QString origin, const QString station)
            : _destination(std::move(destination)), _origin(std::move(origin)), _station(std::move(station)) { }

    Mission() = default;

    QString _destination;
    QString _origin;
    QString _station;
};

class MissionScanner : public Journal::EventDispatchObject {
Q_OBJECT
public:
    explicit MissionScanner(QObject *parent);

    const QStringList commanders() const {
        auto commanders = _commanderLastSystem.keys();
        commanders.sort();
        return commanders;
    }

    const QList<Mission> commanderMission(const QString &cmdr);
    const QString commanderSystem(const QString &cmdr);

    void scanJournals();

    const QString &recentCommander() const;

protected:
    void onEventGeneric(Journal::Event *event) override;

private:
    QMap<QString, QMap<int64_t, Mission>> _commanderMissions;
    QMap<QString, QString>            _commanderLastSystem;
    QString _recentCommander;
};




