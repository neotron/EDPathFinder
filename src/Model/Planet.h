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

#include "Settlement.h"

class Planet {
public:
    explicit Planet();

    Planet(QString name, int distance, const Settlement &settlement);

    Planet(QString name, int distance, SettlementList settlements);

    // Copy and assignments.
    Planet(Planet &&other) = default;
    Planet(const Planet &other) = default;
    Planet &operator=(const Planet &other) = default;
    Planet &operator=(Planet &&other) = default;

    const SettlementList &settlements() const;

    void addSettlement(const Settlement &settlement);

    const QString &name() const;

    int distance() const;


private:
    QString _name;
    int _distance;
    SettlementList _settlements;
};
