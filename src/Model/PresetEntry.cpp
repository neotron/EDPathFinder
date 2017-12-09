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

#include "PresetEntry.h"

static const char *const kShortNameKey = "short";
static const char *const kSysNameKey = "sys";
static const char *const kDetailsKey = "desc";
static const char *const kUrlKey = "url";
static const char *const kTypeKey = "type";

PresetEntry::PresetEntry(const QJsonObject &obj):
        _systemName(obj.value(kSysNameKey).toString()),
        _shortDescription(obj.value(kShortNameKey).toString()),
        _details(obj.value(kDetailsKey).toString()),
        _urlString(obj.value(kUrlKey).toString()),
        _type(obj.value(kTypeKey).toString())
{

}

const QString &PresetEntry::shortDescription() const {
    return _shortDescription;
}

void PresetEntry::setShortDescription(const QString &shortDescription) {
    _shortDescription = shortDescription;
}

const QString &PresetEntry::details() const {
    return _details;
}

void PresetEntry::setDetails(const QString &details) {
    _details = details;
}

const QString &PresetEntry::systemName() const {
    return _systemName;
}

void PresetEntry::setSystemName(const QString &systemName) {
    _systemName = systemName;
}

const QString &PresetEntry::urlString() const {
    return _urlString;
}

void PresetEntry::setUrlString(const QString &urlString) {
    _urlString = urlString;
}

QJsonObject PresetEntry::toJson() const {
    QJsonObject object;
    object[kSysNameKey] = _systemName;
    object[kShortNameKey] = _shortDescription;
    object[kDetailsKey] = _details;
    object[kUrlKey] = _urlString;
    object[kTypeKey] = _type;
    return object;
}

bool PresetEntry::isValid() const {
    return !_systemName.isEmpty() && !_shortDescription.isEmpty();
}

const QString &PresetEntry::type() const {
    return _type;
}

void PresetEntry::setType(const QString &type) {
    _type = type;
}

