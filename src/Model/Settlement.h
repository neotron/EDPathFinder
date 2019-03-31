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

#include <QString>
#include <ortools/base/integral_types.h>
#include "SettlementType.h"


class Settlement {
public:


    explicit Settlement(QString name, uint32 flags = 0, ThreatLevel threatLevel = ThreatLevelLow, const SettlementType *type = nullptr,
                        QMap<SettlementFlags,float> materials = QMap<SettlementFlags,float>());

    // Copy and assignments.
    Settlement(Settlement &&other) = default;
    Settlement(const Settlement &other) = default;
    Settlement &operator=(Settlement &&other) = default;
    Settlement &operator=(const Settlement &other)  = default;

    const QString &name() const;

    SettlementSize size() const;

    ThreatLevel threatLevel() const;

    uint32 flags() const;

    const SettlementType *type() const;

    float materialProbability(SettlementFlags material) const;

private:
    QString _name;
    uint32 _flags;
    ThreatLevel _threatLevel;
    const SettlementType *_type;
    QMap<SettlementFlags,float> _materialProbabilities;
};
