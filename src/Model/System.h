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
#pragma warning(push, 0)
#include <ortools/base/integral_types.h>
#pragma warning(pop)
#include <QJsonDocument>
#include <utility>
#include "PresetEntry.h"

class AStarSystemNode;

class AStarRouter;

class Settlement;

class Planet;

class System;

typedef QList<Settlement> SettlementList;
typedef QList<Planet> PlanetList;
typedef QList<System> SystemList;

enum ThreatLevel {
    ThreatLevelUnknown = 0,
    ThreatLevelLow = 1 << 0,
    ThreatLevelRestrictedLongDistance = 1 << 1,
    ThreatLevelMedium = 1 << 2,
    ThreatLeveLHigh = 1 << 3,
};

enum SettlementSize {
    SettlementSizeUnknown = 0,
    SettlementSizeSmall = 1 << 0,
    SettlementSizeMedium = 1 << 1,
    SettlementSizeLarge = 1 << 2
};

enum SettlementFlags {
    SettlementFlagsCoreDataTerminal = 1 << 0,
    SettlementFlagsJumpClimbRequired = 1 << 1,
    SettlementFlagsClassifiedScanDatabanks = 1 << 2,
    SettlementFlagsClassifiedScanFragment = 1 << 3,
    SettlementFlagsCrackedIndustrialFirmware = 1 << 4,
    SettlementFlagsDivergentScanData = 1 << 5,
    SettlementFlagsModifiedConsumerFirmware = 1 << 6,
    SettlementFlagsModifiedEmbeddedFirmware = 1 << 7,
    SettlementFlagsOpenSymmetricKeys = 1 << 8,
    SettlementFlagsSecurityFirmwarePatch = 1 << 9,
    SettlementFlagsSpecializedLegacyFirmware = 1 << 10,
    SettlementFlagsTaggedEncryptionCodes = 1 << 11,
    SettlementFlagsUnusualEncryptedFiles = 1 << 12,
    SettlementFlagsAnarchy = 1 << 13
};

enum ValuableBodyFlags {
    ValuableBodyFlagsEW,
    ValuableBodyFlagsWW,
    ValuableBodyFlagsWT,
    ValuableBodyFlagsAW,
    ValuableBodyFlagsTF,
    ValuableBodyFlagsCount
};


class SettlementType {
public:
    static const QString IMAGE_BASE_ICON;
    static const QString IMAGE_SATELLITE;
    static const QString IMAGE_COREFULLMAP;
    static const QString IMAGE_OVERVIEW;
    static const QString IMAGE_PATHMAP;
    static const QString IMAGE_OVERVIEW3D;
    static const QString IMAGE_CORE;


    SettlementType(SettlementSize size, ThreatLevel securityLevel, QString economy)
            : _size(size), _securityLevel(securityLevel), _economy(std::move(economy)), _images() {}

    SettlementType(const SettlementType &other) = default;
    SettlementType(SettlementType &&other) = default;
    SettlementType(): _size(SettlementSizeUnknown), _securityLevel(ThreatLevelUnknown) {}

    SettlementSize size() const {
        return _size;
    }

    ThreatLevel securityLevel() const {
        return _securityLevel;
    }

    const QString &economy() const {
        return _economy;
    }


    const QUrl imageNamed(const QString &name) const {
        return _images.contains(name) ? _images[name] : QUrl();
    }

    SettlementType &operator=(SettlementType &&other) = default;
    SettlementType &operator=(const SettlementType &other) = default;

    void addImage(const QString &name, const QUrl &url) {
        if(url.isValid()) {
            _images[name] = url;
        }
    }

    const QStringList imageTitles() const {
        auto keys = _images.keys();
        keys.removeAll(SettlementType::IMAGE_BASE_ICON);
        keys.sort();
        return keys;
    }

private:
    SettlementSize _size;
    ThreatLevel _securityLevel;
    QString _economy;

    QMap<QString, QUrl> _images;
};

class Settlement {
public:


    explicit Settlement(QString name, int32 flags = 0, ThreatLevel threatLevel = ThreatLevelLow, const SettlementType *type = nullptr,
                        QMap<SettlementFlags,float> materials = QMap<SettlementFlags,float>())
            : _name(std::move(name)), _flags(flags), _threatLevel(threatLevel), _type(type), _materialProbabilities(std::move(materials)) {}

    // Copy and assignments.
    Settlement(Settlement &&other) = default;
    Settlement(const Settlement &other) = default;
    Settlement &operator=(Settlement &&other) = default;
    Settlement &operator=(const Settlement &other)  = default;

    const QString &name() const {
        return _name;
    }

    SettlementSize size() const {
        return _type->size();
    }

    ThreatLevel threatLevel() const {
        return _threatLevel;
    }

    int32 flags() const {
        return _flags;
    }

    const SettlementType *type() const {
        return _type;
    }

    float materialProbability(SettlementFlags material) const {
        return _materialProbabilities.contains(material) ? _materialProbabilities[material] : 0.0f;
    }

private:
    QString _name;
    int32 _flags;
    ThreatLevel _threatLevel;
    const SettlementType *_type;
    QMap<SettlementFlags,float> _materialProbabilities;
};

class Planet {
public:
    Planet() : _name(), _distance(0), _settlements() {}

    Planet(QString name, int distance, const Settlement &settlement)
            : _name(std::move(name)), _distance(distance), _settlements() {
        _settlements.push_back(settlement);
    }

    Planet(QString name, int distance, SettlementList settlements)
            : _name(std::move(name)), _distance(distance), _settlements(std::move(settlements)) {}

    // Copy and assignments.
    Planet(Planet &&other) = default;
    Planet(const Planet &other) = default;
    Planet &operator=(const Planet &other) = default;
    Planet &operator=(Planet &&other) = default;

    const SettlementList &settlements() const {
        return _settlements;
    }

    void addSettlement(const Settlement &settlement) {
        _settlements.push_back(settlement);
    }

    const QString &name() const {
        return _name;
    }

    int distance() const {
        return _distance;
    }

private:
    QString _name;
    int _distance;
    SettlementList _settlements;
};

class System {

public:

    System()  : _name(), _planets(), _position(), _numPlanets(), _presetEntry() {}

    System(QString name, float x, float y, float z)
            : _name(std::move(name)), _position(x, y, z), _numPlanets() {}

    System(QString name, const Planet &planet, float x, float y, float z)
            : _name(std::move(name)), _planets(), _position(x, y, z), _numPlanets() {
        _planets.push_back(planet);
    }

    System(QString system, PlanetList planets, const QVector3D &position)
            : _name(std::move(system)), _planets(std::move(planets)), _position(position), _numPlanets() {}

    System(QString name, const QVector3D &position)
            : _name(std::move(name)), _position(position), _numPlanets() {}
    explicit System(AStarSystemNode *system);

    // Copy and assignment
    System(const System &other) = default;
    System(System &&other) = default;
    System &operator=(const System &other) = default;
    System &operator=(System &&other) = default;

    explicit System(const QJsonObject &jsonObject);

    virtual ~System();

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

    void setNumPlanets(const QList<int8_t> &numPlanets) {
        _numPlanets = numPlanets;
    }

    bool matchesFilter(const QList<bool> &filter) const {
        for(int i = 0; i < filter.count() &&  i < _numPlanets.count(); i++) {
            if(filter[i] && _numPlanets[i] > 0) {
                return true;
            }
        }
        return false;
    }

    const PlanetList &planets() const { return _planets; }

    void addSettlement(const QString &planetName, const Settlement &settlement, int distance);

    const QString &name() const {
        return _name;
    }

    const QString formatPlanets() const;

    int32 estimatedValue() const;

    void setKey(std::string key);

    const std::string &key() const;
    static const std::string makeKey(const QString &name);

    bool operator<(const System &rhs) const;

    bool operator>(const System &rhs) const;

    bool operator<=(const System &rhs) const;

    bool operator>=(const System &rhs) const;

    const PresetEntry &presetEntry() const;

    void setPresetEntry(const PresetEntry &presetEntry);

protected:

    System(float x, float y, float z)
            : _position(x, y, z) {}

    float sqr(float val) const { return val * val; }

    QString _name;
    PlanetList _planets;
    QVector3D _position;
    QList<int8_t>  _numPlanets;
    mutable std::string _key;
    PresetEntry _presetEntry;

    void addSystemString(QStringList &list, ValuableBodyFlags type, QString name) const;
};
