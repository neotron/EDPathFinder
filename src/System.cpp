//
// Created by David Hedbor on 10/7/16.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "System.h"

std::deque<System, std::allocator<System>> SystemLoader::loadSettlements(char *path) {
	std::deque<System> systems;
	std::ifstream is(path);
	std::map<std::string, System *> lookup;
	do {
		std::string line;
		std::getline(is, line, '\n');
		if(!is.eof()) {
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
			std::string type, system, planet, name, dump;
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
			size = (SettlementSize) getInt(lis);

			Settlement settlement(name, size, threat, flags);

			if(lookup.count(system)) {
				lookup[system]->addSettlement(settlement);
			} else {
				System systemObj(system, planet, settlement, x, y, z);
				systems.push_back(systemObj);
				lookup[system] = &systems.back();
			}
		}
	} while(!is.eof());
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

std::string System::formatDistance(int64 dist) {
	std::stringstream ss;
	if(dist > 0) {
		ss << dist / 100 << "." << dist % 100;
		return ss.str();
	} else {
		return "-";
	}
}
