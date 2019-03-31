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

#include "Planet.h"

Planet::Planet(QString name, int distance, const Settlement &settlement)
    : _name(std::move(name)), _distance(distance), _settlements() {
    _settlements.push_back(settlement);
}

Planet::Planet() : _name(), _distance(0), _settlements() {}

Planet::Planet(QString name, int distance, SettlementList settlements)
    : _name(std::move(name)), _distance(distance), _settlements(std::move(settlements)) {}

const SettlementList &Planet::settlements() const {
    return _settlements;
}

void Planet::addSettlement(const Settlement &settlement) {
    _settlements.push_back(settlement);
}

const QString &Planet::name() const {
    return _name;
}

int Planet::distance() const {
    return _distance;
}
