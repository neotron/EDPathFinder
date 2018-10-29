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
static const char *const kPlanetKey = "planet";
static const char *const kLatKey = "lat";
static const char *const kLonKey = "lon";
static const char *const kRadiusKey = "radius";

PresetEntry::PresetEntry(const QJsonObject &obj):
        _systemName(obj.value(kSysNameKey).toString().trimmed()),
        _shortDescription(obj.value(kShortNameKey).toString().trimmed()),
        _details(obj.value(kDetailsKey).toString().trimmed()),
        _urlString(obj.value(kUrlKey).toString().trimmed()),
        _type(obj.value(kTypeKey).toString().trimmed()),
        _planet(obj.value(kPlanetKey).toString().trimmed()),
        _lat(obj.value(kLatKey).toDouble()),
        _lon(obj.value(kLonKey).toDouble()),
        _radius(obj.value(kRadiusKey).toDouble())
{

}

const QString &PresetEntry::shortDescription() const {
    return _shortDescription;
}

void PresetEntry::setShortDescription(const QString &shortDescription) {
    _shortDescription = shortDescription;
}

const QString PresetEntry::details() const {
    if(!_planet.isEmpty()) {
        return QString("Planet %1: %2, %3%4%5").arg(_planet).arg(_lat).arg(_lon).arg(_details.isEmpty()?"":"\n").arg(_details);
    }
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
    object[kPlanetKey] = _planet;
    object[kLatKey] = _lat;
    object[kLonKey] = _lon;
    object[kRadiusKey] = _radius;
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

PresetEntry::PresetEntry(const QString &systemName)
        : _systemName(systemName), _shortDescription("Manual stop"), _type("Custom")
{
}

bool PresetEntry::operator==(const PresetEntry &rhs) const {
    return std::tie(_systemName, _shortDescription, _type) ==
           std::tie(rhs._systemName, rhs._shortDescription, rhs._type);
}

bool PresetEntry::operator!=(const PresetEntry &rhs) const {
    return !(rhs == *this);
}

const QString &PresetEntry::planet() const {
    return _planet;
}

void PresetEntry::setPlanet(const QString &planet) {
    _planet = planet;
}

double PresetEntry::lat() const {
    return _lat;
}

void PresetEntry::setLat(double lat) {
    _lat = lat;
}

double PresetEntry::lon() const {
    return _lon;
}

void PresetEntry::setLon(double lon) {
    _lon = lon;
}

double PresetEntry::radius() const {
    return _radius;
}

void PresetEntry::setRadius(double radius) {
    _radius = radius;
}

bool PresetEntry::operator<(const PresetEntry &rhs) const {
    return std::tie(_systemName, _type) < std::tie(rhs._systemName, rhs._type);
}

bool PresetEntry::operator>(const PresetEntry &rhs) const {
    return rhs < *this;
}

bool PresetEntry::operator<=(const PresetEntry &rhs) const {
    return !(rhs < *this);
}

bool PresetEntry::operator>=(const PresetEntry &rhs) const {
    return !(*this < rhs);
}

