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

#include <QThread>
#include "System.h"

class AStarRouter;

class SystemLoader : public QThread {
Q_OBJECT

public:
    explicit SystemLoader(AStarRouter *router)
            : QThread(), _router(router) {}

    void run() override;

    ~SystemLoader() override;

    void loadSettlements();

    void loadSettlementTypes();

    const SystemList &systems() const {
        return _systems;
    }

    const QMap<QString, SettlementType *> &settlementTypes() const {
        return _settlementTypes;
    }

signals:

    void systemsLoaded(const SystemList &systems);

    void progress(int progress);

    void sortingSystems();

public slots:

    void dataDecompressed(const QByteArray &bytes);

    void valuableSystemDataDecompressed(const QByteArray &bytes);

private:

    QMap<QString, SettlementType *> _settlementTypes;
    SystemList _systems;
    AStarRouter *_router;
    QByteArray _bytes;
    QByteArray _valueBytes;
    QJsonObject _bodyDistances;
    QAtomicInt _progress1, _progress2;
    SystemList * loadSystemFromTextFile();
    void loadValueSystemFromTextFile();

    int getDistance(const QString &system, const QString &planet);
};
