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

#include "PresetsEntry.h"

static const char *const kShortNameKey = "short";
static const char *const kSysNameKey = "sys";
static const char *const kDetailsKey = "desc";
static const char *const kUrlKey = "url";
static const char *const kTypeKey = "type";

PresetsEntry::PresetsEntry(const QJsonObject &obj):
        _systemName(obj.value(kSysNameKey).toString()),
        _shortDescription(obj.value(kShortNameKey).toString()),
        _details(obj.value(kDetailsKey).toString()),
        _urlString(obj.value(kUrlKey).toString()),
        _type(obj.value(kTypeKey).toString())
{

}

const QString &PresetsEntry::shortDescription() const {
    return _shortDescription;
}

void PresetsEntry::setShortDescription(const QString &shortDescription) {
    _shortDescription = shortDescription;
}

const QString &PresetsEntry::details() const {
    return _details;
}

void PresetsEntry::setDetails(const QString &details) {
    _details = details;
}

const QString &PresetsEntry::systemName() const {
    return _systemName;
}

void PresetsEntry::setSystemName(const QString &systemName) {
    _systemName = systemName;
}

const QString &PresetsEntry::urlString() const {
    return _urlString;
}

void PresetsEntry::setUrlString(const QString &urlString) {
    _urlString = urlString;
}

QJsonObject PresetsEntry::toJson() const {
    QJsonObject object;
    object[kSysNameKey] = _systemName;
    object[kShortNameKey] = _shortDescription;
    object[kDetailsKey] = _details;
    object[kUrlKey] = _urlString;
    object[kTypeKey] = _type;
    return object;
}

bool PresetsEntry::isValid() const {
    return !_systemName.isEmpty() && !_shortDescription.isEmpty();
}

const QString &PresetsEntry::type() const {
    return _type;
}

void PresetsEntry::setType(const QString &type) {
    _type = type;
}

