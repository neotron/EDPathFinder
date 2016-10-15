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


#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "System.h"
#include "AStarRouter.h"

SystemList SystemLoader::loadSettlements(AStarRouter *router) {
    SystemList systems;
    QFile systemData(":/dbdump.csv");
    if(!systemData.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return systems;
    }

    QTextStream in(&systemData);
    std::unordered_map<std::string, System *> lookup;
    for(QString qline = in.readLine(); !qline.isNull(); qline = in.readLine()) {
        std::string line = qline.toStdString();
        //  1: Type
        //  2: System
        //  3: Body
        //  4: Name
        //  5: is anarchy
        //  6-8: x, y, z
        //  9: distance(ly) (ignore)
        // 10: threat level	0-3
        // 11: Core Data Terminal present
        // 12: jump climb required
        // 13: cmdr (source)
        // 14: special notes
        // 15: UEF
        // 16: SLF
        // 17: TEC
        // 18: MCF
        // 19: CSD
        // 20: CIF
        // 21: OSK
        // 22: SFP
        // 23: DSD
        // 24: MEF
        // 25: CSF
        // 26: size
        // 27: idx
        std::istringstream lis(line);
        std::string type, system, planet, name, sizeStr, dump;
        double x, y, z;
        int32 flags = 0;
        SettlementSize size;
        ThreatLevel threat;

        std::getline(lis, type, '\t'); // 1
        std::getline(lis, system, '\t'); // 2
        std::getline(lis, planet, '\t'); // 3
        std::getline(lis, name, '\t'); // 4
        if(getBool(lis)) { // gov/is anarchy
            flags |= SettlementFlagsAnarchy;
        }
        x = getDouble(lis);
        y = getDouble(lis);
        z = getDouble(lis);
        std::getline(lis, dump, '\t'); // distance - ignore
        threat = (ThreatLevel) getInt(lis); // threat level
        if(getBool(lis)) { flags |= SettlementFlagsCoreDataTerminal; }
        if(getBool(lis)) { flags |= SettlementFlagsJumpClimbRequired; }
        std::getline(lis, dump, '\t'); // Commander - ignore
        std::getline(lis, dump, '\t'); // Notes - ignore
        if(getBool(lis)) { flags |= SettlementFlagsUnusualEncryptedFiles; }
        if(getBool(lis)) { flags |= SettlementFlagsSpecializedLegacyFirmware; }
        if(getBool(lis)) { flags |= SettlementFlagsTaggedEncryptionCodes; }
        if(getBool(lis)) { flags |= SettlementFlagsModifiedConsumerFirmware; }
        if(getBool(lis)) { flags |= SettlementFlagsClassifiedScanDatabanks; }
        if(getBool(lis)) { flags |= SettlementFlagsCrackedIndustrialFirmware; }
        if(getBool(lis)) { flags |= SettlementFlagsOpenSymmetricKeys; }
        if(getBool(lis)) { flags |= SettlementFlagsSecurityFirmwarePatch; }
        if(getBool(lis)) { flags |= SettlementFlagsDivergentScanData; }
        if(getBool(lis)) { flags |= SettlementFlagsModifiedEmbeddedFirmware; }
        if(getBool(lis)) { flags |= SettlementFlagsClassifiedScanFragment; }
        std::getline(lis, sizeStr, '\t');
        if(sizeStr == "Large") {
            size = SettlementSizeLarge;
        } else if(sizeStr == "Medium") {
            size = SettlementSizeMedium;
        } else {
            size = SettlementSizeSmall;
        }
        Settlement settlement(name, size, threat, flags);

        if(lookup.count(system)) {
            lookup[system]->addSettlement(planet, settlement);
        } else {
            Planet planetObj(planet, settlement);
            System systemObj(system, planetObj, x, y, z);
            systems.push_back(systemObj);
            lookup[system] = &systems.back();

            if(!router->getSystemByName(system)) {
                router->addSystem(systemObj);
            }
        }
    }
    std::cerr << "Loaded " << systems.size() << " systems." << std::endl;
    return systems;
}

int32 SystemLoader::getInt(std::istringstream &is, bool eol) const {
    std::string valStr;
    std::getline(is, valStr, (char) (eol ? '\n' : '\t'));
    int32 val;
    std::stringstream ss(valStr);
    ss >> val;
    return val;
}

double SystemLoader::getDouble(std::istringstream &is, bool eol) const {
    std::string valStr;
    std::getline(is, valStr, (char) (eol ? '\n' : '\t'));
    double val;
    std::stringstream ss(valStr);
    ss >> val;
    return val;
}

bool SystemLoader::getBool(std::istringstream &is, bool eol) const {
    std::string valStr;
    std::getline(is, valStr, (char) (eol ? '\n' : '\t'));
    return valStr == "1";
}

QString System::formatDistance(int64 dist) {
    if(dist > 0) {
        return QString("%1.%2").arg(dist / 10).arg(dist % 10);
    } else {
        return "-";
    }
}

System::System(AStarSystemNode *system) : _name(system->name()), _position(system->position()) {}

void System::addSettlement(const std::string &planetName, const Settlement &settlement) {
    for(auto planet: _planets) {
        if(planet.name() == planetName) {
            planet.addSettlement(settlement);
            return;
        }
    }
    _planets.push_back(Planet(planetName, settlement));
}
