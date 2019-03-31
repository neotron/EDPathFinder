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

#include "SettlementType.h"

const QString SettlementType::IMAGE_BASE_ICON = "Icon";
const QString SettlementType::IMAGE_SATELLITE = "Satellite Map";
const QString SettlementType::IMAGE_COREFULLMAP = "Core Full Map";
const QString SettlementType::IMAGE_OVERVIEW = "Overview Map";
const QString SettlementType::IMAGE_PATHMAP = "Datapoint Path Map";
const QString SettlementType::IMAGE_OVERVIEW3D = "3D Overview";
const QString SettlementType::IMAGE_CORE = "Core Map";

SettlementType::SettlementType(SettlementSize size, ThreatLevel securityLevel, QString economy)
    : _size(size), _securityLevel(securityLevel), _economy(std::move(economy)), _images() {}

SettlementType::SettlementType() : _size(SettlementSizeUnknown), _securityLevel(ThreatLevelUnknown) {}

SettlementSize SettlementType::size() const {
    return _size;
}

ThreatLevel SettlementType::securityLevel() const {
    return _securityLevel;
}

const QString &SettlementType::economy() const {
    return _economy;
}

const QUrl SettlementType::imageNamed(const QString &name) const {
    return _images.contains(name) ? _images[name] : QUrl();
}

void SettlementType::addImage(const QString &name, const QUrl &url) {
    if(url.isValid()) {
        _images[name] = url;
    }
}

const QStringList SettlementType::imageTitles() const {
    auto keys = _images.keys();
    keys.removeAll(SettlementType::IMAGE_BASE_ICON);
    keys.sort();
    return std::move(keys);
}
