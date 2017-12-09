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
    PresetEntry(const QJsonObject &obj);

    PresetEntry(const PresetEntry &other) = default;

    PresetEntry(PresetEntry &&other) = default;

    PresetEntry &operator=(const PresetEntry &other) = default;

    PresetEntry &operator=(PresetEntry &&other) = default;

    const QString &shortDescription() const;

    void setShortDescription(const QString &shortDescription);

    const QString &details() const;

    void setDetails(const QString &details);

    const QString &systemName() const;

    void setSystemName(const QString &systemName);

    const QString &urlString() const;

    void setUrlString(const QString &urlString);

    const QString &type() const;

    void setType(const QString &type);

    QJsonObject toJson() const;

    bool isValid() const;


private:
    QString _systemName;
    QString _shortDescription;
    QString _details;
    QString _urlString;
    QString _type;
};

typedef QList<PresetEntry> PresetEntryList;
