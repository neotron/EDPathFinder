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
#include <QList>


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
