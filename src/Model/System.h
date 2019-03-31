#include <utility>

#include <utility>

//
//  Copyright (C) 2016-2017  David Hedbor <neotron@gmail.com>
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

#include <cmath>
#include <QString>
#include <AStar.h>
#include <QVector3D>
#include <QJsonObject>
#include <QThread>
#include <QDebug>
#include <QUrl>
#ifdef Q_OS_WIN
#pragma warning(push, 0)
#include <ortools/base/integral_types.h>
#pragma warning(pop)
#else
#include <ortools/base/integral_types.h>
#endif

#include <QJsonDocument>
#include <utility>
#include "PresetEntry.h"

#include "Planet.h"


class System {

public:

    explicit System() = default;

    System(QString name, float x, float y, float z)
        : _name(std::move(name)), _position(x, y, z) {}

    System(QString name, const Planet &planet, float x, float y, float z)
        : _name(std::move(name)), _position(x, y, z) {
        _planets.push_back(planet);
    }

    System(QString system, PlanetList planets, const QVector3D &position)
        : _name(std::move(system)), _position(position), _planets(std::move(planets)) {}

    System(QString name, const QVector3D &position)
        : _name(std::move(name)), _position(position) {}

    explicit System(AStarSystemNode *system);


    // Copy and assignment
    System(const System &other)  {
        _name = other._name;
        _position = other._position;
        setNumPlanetsFrom(other);
        _estimatedValue = other._estimatedValue;
        _key = other._key;
        _planets = other._planets;

        if(other._presetEntry) {
            _presetEntry = new PresetEntry(*other._presetEntry);
        }
    }

    System &operator=(const System &other) {
        _name = other._name;
        _position = other._position;
        setNumPlanetsFrom(other);
        _estimatedValue = other._estimatedValue;
        _key = other._key;
        _planets = other._planets;
        delete _presetEntry;
        if(other._presetEntry) {
            _presetEntry = new PresetEntry(*other._presetEntry);
        }
        return *this;
    }

    explicit System(const QJsonObject &jsonObject);

    ~System();

// Return distance as a fixed point value with two decimals. Used by TSP
    int64 distance(const System &other) const {
        float distance = _position.distanceToPoint(other._position) * 10;
        return (int64) distance;
    }

    static QString formatDistance(int64 dist, bool trunc = false);

    float z() const {
        return _position.z();
    }

    float y() const {
        return _position.y();
    }

    float x() const {
        return _position.x();
    }

    const QVector3D &position() const {
        return _position;
    }

    void setNumPlanets(const QList<uint8_t> &planets) {
        memset(_numPlanets, 0, sizeof(_numPlanets));
        for(int i = 0; i < planets.count() &&  i < ValuableBodyFlagsCount; i++) {
            _numPlanets[i] = planets[i];
        }
    }
    void setNumPlanetsFrom(const System &system) {
        memcpy(_numPlanets, system.numPlanets(), sizeof(_numPlanets));
    }
    const uint8_t *numPlanets() const {
        return _numPlanets;
    }

    bool matchesFilter(const QList<bool> &filter) const {
        for(int i = 0; i < filter.count() &&  i < ValuableBodyFlagsCount; i++) {
            if(filter[i] && _numPlanets[i]) {
                return true;
            }
        }
        return false;
    }

    void addSettlement(const QString &planetName, const Settlement &settlement, int distance);

    const QString &name() const {
        return _name;
    }
    const PlanetList &planets() const { return _planets; }

    const QString formatPlanets() const;

    const std::string &key() const;

    static const std::string makeKey(const QString &name);

    bool operator<(const System &rhs) const;

    bool operator>(const System &rhs) const;

    bool operator<=(const System &rhs) const;

    bool operator>=(const System &rhs) const;

    int estimatedValue() const;

    void setEstimatedValue(int estimatedValue);

    const PresetEntry &presetEntry() const;

    void setPresetEntry(const PresetEntry &presetEntry);

protected:
    void addSystemString(QStringList &list, ValuableBodyFlags type, QString name) const;

    System(float x, float y, float z): _position(x, y, z) {}

    QString _name;
    QVector3D _position;
    uint8_t _numPlanets[ValuableBodyFlagsCount]{};
    int _estimatedValue{};
    mutable std::string _key;
    PlanetList _planets{};

    static PresetEntry s_presetEntry;

    PresetEntry *_presetEntry{nullptr};

};

inline bool System::operator<(const System &rhs) const {
    return _name < rhs._name;
}

inline bool System::operator>(const System &rhs) const {
    return rhs < *this;
}

inline bool System::operator<=(const System &rhs) const {
    return !(rhs < *this);
}

inline bool System::operator>=(const System &rhs) const {
    return !(*this < rhs);
}
