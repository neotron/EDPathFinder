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

#include <QCompleter>
#include <QLineEdit>
#include <QLabel>
#include "AStarRouter.h"


class SystemEntryCoordinateResolver : public QObject {
Q_OBJECT


public:
    SystemEntryCoordinateResolver(QObject *parent, AStarRouter *router, QLineEdit *lineEdit,
                                 QLabel *x = nullptr, QLabel *y = nullptr, QLabel *z = nullptr);

    void resolve(const QString &systemName);

    bool isComplete() const;

private slots:

    void onCompletion(const QString &systemName);

    void onEntry();

    void systemCoordinatesRequestFailed(const QString &systemName);

    void systemCoordinatesReceived(const System &system);

signals:

    void systemLookupInitiated(const QString &system);

    void systemLookupCompleted(const System &system);

    void systemLookupFailed(const QString &systemName);


private:
    void downloadSystemCoordinates(const QString &systemName);

    AStarRouter   *_router;
    QLineEdit     *_lineEdit;
    QSet<QString> _pendingLookups;
    QLabel *_x, *_y, *_z;
    int _retries{};

    void sendSystemLookupCompleted(const System &system);
};



