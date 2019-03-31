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

#include "Settlement.h"

Settlement::Settlement(QString name, uint32 flags, ThreatLevel threatLevel, const SettlementType *type,
                       QMap<SettlementFlags, float> materials)
    : _name(std::move(name)), _flags(flags), _threatLevel(threatLevel), _type(type), _materialProbabilities(std::move(materials)) {}

const QString &Settlement::name() const {
    return _name;
}

ThreatLevel Settlement::threatLevel() const {
    return _threatLevel;
}

SettlementSize Settlement::size() const {
    return _type->size();
}

uint32 Settlement::flags() const {
    return _flags;
}

const SettlementType *Settlement::type() const {
    return _type;
}

float Settlement::materialProbability(SettlementFlags material) const {
    return _materialProbabilities.contains(material) ? _materialProbabilities[material] : 0.0f;
}
