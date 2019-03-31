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
#include "ModelDefs.h"

#include <QString>
#include <QUrl>
#include <QMap>

class SettlementType {
public:
    static const QString IMAGE_BASE_ICON;
    static const QString IMAGE_SATELLITE;
    static const QString IMAGE_COREFULLMAP;
    static const QString IMAGE_OVERVIEW;
    static const QString IMAGE_PATHMAP;
    static const QString IMAGE_OVERVIEW3D;
    static const QString IMAGE_CORE;


    SettlementType(SettlementSize size, ThreatLevel securityLevel, QString economy);

    SettlementType(const SettlementType &other) = default;
    SettlementType(SettlementType &&other) = default;
    SettlementType();

    SettlementSize size() const;

    ThreatLevel securityLevel() const;

    const QString &economy() const;

    const QUrl imageNamed(const QString &name) const;

    SettlementType &operator=(SettlementType &&other) = default;
    SettlementType &operator=(const SettlementType &other) = default;

    void addImage(const QString &name, const QUrl &url);

    const QStringList imageTitles() const;

private:
    SettlementSize _size;
    ThreatLevel _securityLevel;
    QString _economy{};

    QMap<QString, QUrl> _images{};
};
