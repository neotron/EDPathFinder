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
// This represents a system entry in the custom router presets/bookmarks..


#pragma once
#include <QString>
#include <QObject>
#include <QVector3D>
#include <QJsonObject>

class System;
class PresetEntry  {
public:
    explicit PresetEntry(const QJsonObject &obj);
    explicit PresetEntry(const QString &systemName);
    PresetEntry() = default;

    PresetEntry(const PresetEntry &other) = default;

    PresetEntry(PresetEntry &&other) = default;


    PresetEntry &operator=(const PresetEntry &other) = default;

    PresetEntry &operator=(PresetEntry &&other) = default;

    bool operator==(const PresetEntry &rhs) const;

    bool operator!=(const PresetEntry &rhs) const;

    const QString &shortDescription() const;

    void setShortDescription(const QString &shortDescription);

    const QString details() const;

    void setDetails(const QString &details);

    const QString &systemName() const;

    void setSystemName(const QString &systemName);

    const QString &urlString() const;

    void setUrlString(const QString &urlString);

    const QString &type() const;

    void setType(const QString &type);

    const QString &planet() const;

    void setPlanet(const QString &planet);

    double lat() const;

    void setLat(double lat);

    double lon() const;

    void setLon(double lon);

    double radius() const;

    void setRadius(double radius);

    QJsonObject toJson() const;

    bool isValid() const;


private:
    QString _systemName;
    QString _shortDescription;
    QString _details;
    QString _urlString;
    QString _type;
    QString _planet;
    double _lat, _lon, _radius;
};

inline uint qHash(const PresetEntry &key, uint seed) {
    uint hash = qHash(key.systemName(), seed);
    hash = qHash(key.type(), hash);
    return qHash(key.shortDescription(), hash);
}

typedef QList<PresetEntry> PresetEntryList;
