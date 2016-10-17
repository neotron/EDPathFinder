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

#include <cmath>
#include <base/integral_types.h>
#include <QString>
#include <AStar.h>
#include <QVector3D>

class AStarSystemNode;

class AStarRouter;

class Settlement;

class Planet;

class System;

typedef QVector<Settlement> SettlementList;
typedef QVector<Planet>     PlanetList;
typedef QVector<System>     SystemList;

enum ThreatLevel {
    ThreatLevelLow, ThreatLevelRestrictedLongDistance, ThreatLevelMedium, ThreatLeveLHigh
};

enum SettlementSize {
    SettlementSizeSmall = 1 << 0, SettlementSizeMedium = 1 << 1, SettlementSizeLarge = 1 << 2
};

enum SettlementFlags {
    SettlementFlagsCoreDataTerminal          = 1 << 0,
    SettlementFlagsJumpClimbRequired         = 1 << 1,
    SettlementFlagsClassifiedScanDatabanks   = 1 << 2,
    SettlementFlagsClassifiedScanFragment    = 1 << 3,
    SettlementFlagsCrackedIndustrialFirmware = 1 << 4,
    SettlementFlagsDivergentScanData         = 1 << 5,
    SettlementFlagsModifiedConsumerFirmware  = 1 << 6,
    SettlementFlagsModifiedEmbeddedFirmware  = 1 << 7,
    SettlementFlagsOpenSymmetricKeys         = 1 << 8,
    SettlementFlagsSecurityFirmwarePatch     = 1 << 9,
    SettlementFlagsSpecializedLegacyFirmware = 1 << 10,
    SettlementFlagsTaggedEncryptionCodes     = 1 << 11,
    SettlementFlagsUnusualEncryptedFiles     = 1 << 12,
    SettlementFlagsAnarchy                   = 1 << 13
};

class Settlement {
public:
    Settlement() : _name(), _size(), _threatLevel(), _flags() { }

    Settlement(const QString &name, SettlementSize size = SettlementSizeSmall, ThreatLevel threatLevel = ThreatLevelLow,
               int32 flags = 0) : _name(name), _size(size), _threatLevel(threatLevel), _flags(flags) { }

    Settlement(const Settlement &&other) : _name(std::move(other._name)), _size(other._size),
                                           _threatLevel(other._threatLevel), _flags(other._flags) {
    }

    Settlement(const Settlement &other) : _name(other._name), _size(other._size), _threatLevel(other._threatLevel),
                                          _flags(other._flags) {
    }

    Settlement &operator=(const Settlement &&other) {
        _name        = std::move(other._name);
        _size        = other._size;
        _threatLevel = other._threatLevel;
        _flags       = other._flags;
        return *this;
    }

    Settlement &operator=(const Settlement &other) {
        _name        = other._name;
        _size        = other._size;
        _threatLevel = other._threatLevel;
        _flags       = other._flags;
        return *this;
    }

    const QString &name() const {
        return _name;
    }

    SettlementSize size() const {
        return _size;
    }

    ThreatLevel threatLevel() const {
        return _threatLevel;
    }

    int32 flags() const {
        return _flags;
    }

private:
    QString        _name;
    SettlementSize _size;
    ThreatLevel    _threatLevel;
    int32          _flags;
};

class Planet {
public:
    Planet() : _name(), _settlements() { }

    Planet(const QString &name, const Settlement &settlement) : _name(name), _settlements() {
        _settlements.push_back(settlement);
    }

    Planet(const QString &name, const SettlementList &settlements) : _name(name), _settlements(settlements) { }

    Planet(const Planet &&other) : _name(std::move(other._name)), _settlements(std::move(other._settlements)) { }

    Planet(const Planet &other) : _name(other._name), _settlements(other._settlements) { }

    Planet &operator=(const Planet &other) {
        _name        = other._name;
        _settlements = other._settlements;
        return *this;
    }

    Planet &operator=(const Planet &&other) {
        _name        = std::move(other._name);
        _settlements = std::move(other._settlements);
        return *this;
    }

    const SettlementList &settlements() const {
        return _settlements;
    }

    void addSettlement(const Settlement &settlement) {
        _settlements.push_back(settlement);
    }

    const QString &name() const {
        return _name;
    }

private:
    QString        _name;
    SettlementList _settlements;
};

class System {

public:

    System() { }

    System(const QString &name, float x, float y, float z) : _name(name), _position(x, y, z) { }

    System(const QString &name, const Planet &planet, float x, float y, float z) : _name(name), _planets(),
                                                                                   _position(x, y, z) {
        _planets.push_back(planet);
    }

    System(const QString &system, const PlanetList &planets, float x, float y, float z) : _name(system),
                                                                                          _planets(planets),
                                                                                          _position(x, y, z) { }

    System(const QString &name, const QVector3D &position) : _name(name), _position(position) { }

    System(const System &other) : _name(other._name), _planets(other._planets), _position(other._position) { }

    System(const System &&other) : _name(std::move(other._name)), _planets(std::move(other._planets)),
                                   _position(std::move(other._position)) { }

    System &operator=(const System &other) {
        _name = other._name;
        _planets = other._planets;
        _position = other._position;
        return *this;
    }

    System &operator=(const System &&other) {
        _name = std::move(other._name);
        _planets = std::move(other._planets);
        _position = std::move(other._position);
        return *this;
    }

    System(AStarSystemNode *system);

    virtual ~System();

// Return distance as a fixed point value with two decimals. Used by TSP
    int64 distance(const System &other) const {
        float distance = _position.distanceToPoint(other._position) * 10;
        return (int64) distance;
    }

    static QString formatDistance(int64 dist);

    float z() const {
        return _position.x();
    }

    float y() const {
        return _position.y();
    }

    float x() const {
        return _position.z();
    }

    const QVector3D &position() const {
        return _position;
    }

    const PlanetList &planets() const { return _planets; }

    void addSettlement(const QString &planetName, const Settlement &settlement);

    const QString &name() const {
        return _name;
    }

protected:

    System(float x, float y, float z) : _position(x, y, z) { }

    float sqr(float val) const { return val * val; }

    QString    _name;
    PlanetList _planets;
    QVector3D  _position;
};

class SystemLoader {
public:
    SystemList loadSettlements(AStarRouter *router);
};
