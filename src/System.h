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

#include <string>
#include <deque>
#include <cmath>
#include <base/integral_types.h>
#include <QString>

class Settlement;

class Planet;

class System;

typedef std::deque<Settlement> SettlementList;
typedef std::deque<Planet> PlanetList;
typedef std::deque<System> SystemList;

enum ThreatLevel {
    ThreatLevelLow,
    ThreatLevelRestrictedLongDistance,
    ThreatLevelMedium,
    ThreatLeveLHigh
};

enum SettlementSize {
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
    SettlementFlagsAnarchy = 1 << 13,
};

class Settlement {


public:
    Settlement(const std::string &name, SettlementSize size = SettlementSizeSmall,
               ThreatLevel threatLevel = ThreatLevelLow, int32 flags = 0) : _name(name), _size(size),
                                                                            _threatLevel(threatLevel), _flags(flags) { }

    const std::string &name() const {
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
    std::string _name;
    SettlementSize _size;
    ThreatLevel _threatLevel;
    int32 _flags;
};

class Planet {
public:
    Planet(const std::string &name, const Settlement &settlement) : _name(name), _settlements() {
        _settlements.push_back(settlement);
    }

    Planet(const std::string &name, const SettlementList &settlements) : _name(name), _settlements(settlements) { }

    const SettlementList &settlements() const {
        return _settlements;
    }

    void addSettlement(const Settlement &settlement) {
        _settlements.push_back(settlement);
    }

    const std::string &name() const {
        return _name;
    }

private:
    std::string _name;
    SettlementList _settlements;
};

class System {

public:

    System() { }

    System(const std::string &name, const Planet &planet, double x, double y, double z) : _name(name), _planets(),
                                                                                          _x(x), _y(y), _z(z) {
        _planets.push_back(planet);
    }

    System(const std::string &system, const PlanetList &planets, double x, double y, double z) : _name(system),
                                                                                                 _planets(planets),
                                                                                                 _x(x), _y(y), _z(z) { }

    System(double x, double y, double z) : _x(x), _y(y), _z(z) { }

// Return distance as a fixed point value with two decimals.
    int64 distance(const System &other) const {
        double distance = sqrt(sqr(_x - other._x) + sqr(_y - other._y) + sqr(_z - other._z)) * 100;
        return (int64) distance;
    }

    static QString formatDistance(int64 dist);

    double z() const {
        return _z;
    }

    double y() const {
        return _y;
    }

    double x() const {
        return _x;
    }

    const PlanetList &planets() const { return _planets; };

    void addSettlement(const std::string &planetName, const Settlement &settlement) {
        for(auto planet: _planets) {
            if(planet.name() == planetName) {
                planet.addSettlement(settlement);
                return;
            }
        }
        _planets.push_back(Planet(planetName, settlement));
    };

    const std::string &name() const {
        return _name;
    }

private:
    double sqr(double val) const { return val * val; }

    std::string _name;
    PlanetList _planets;
    double _x;
    double _y;
    double _z;
};

class SystemLoader {
public:
    SystemList loadSettlements();

private:
    int32 getInt(std::istringstream &is, bool eol = false) const;

    double getDouble(std::istringstream &is, bool eol = false) const;

    bool getBool(std::istringstream &is, bool eol = false) const;
};
