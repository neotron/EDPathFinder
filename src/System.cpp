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


#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "System.h"
#include "AStarRouter.h"
#include <QMap>

#define SETTLEMENT_TYPE_FIELD_COUNT 17
#define EXPECTED_FIELD_COUNT 27
#define READ_INT (*(it++)).toInt()
#define READ_CHAR static_cast<int8_t>((*(it++)).toShort())
#define READ_FLOAT (*(it++)).toFloat()
#define READ_BOOL (READ_INT == 1)
#define READ_STR (*(it++))
#define READ_URL QUrl(READ_STR)
#define SKIP_FIELD do { it++; } while(false)
#define READ_MATERIAL(FLAG) do { float val = READ_FLOAT; if(val > 0.000) { flags |= FLAG; materials[FLAG] = val; }; } while(false)
#define READ_URL_JPEG(X) QUrl X; do { \
    auto url = READ_STR; \
    if(url.length()) { \
        url += ".jpg"; \
        (X) = QUrl(url); \
    }\
} while(false)

void SystemLoader::run() {
    QFile distances(":/body_distances.json");
    if(!distances.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    auto distanceDoc = QJsonDocument::fromJson(distances.readAll());
    if(distanceDoc.isObject()) {
        _bodyDistances = distanceDoc.object();
    }
    loadSystemFromTextFile();
    loadValueSystemFromTextFile();
    loadSettlements();
    emit sortingSystems();
    _router->sortSystemList();
    emit systemsLoaded(_systems);
}


void SystemLoader::loadSystemFromTextFile() {
    auto start = QDateTime::currentDateTimeUtc();
    QStringList lines(QString(_bytes).split("\n"));
    int i = 0;
    for(const auto &qline: lines) {
        QStringList line = qline.split("\t");
        if(line.size() != 4) {
            continue;
        }
        auto it = line.begin();
        auto name = READ_STR;
        auto x = READ_FLOAT;
        auto y = READ_FLOAT;
        auto z = READ_FLOAT;
        _router->addSystem(System(name, x, y, z));
        if(!(i++ % 100)) {
            emit progress((int) (i / (float) lines.size() * 50));
        }
    }
    emit progress(50);
}


void SystemLoader::loadValueSystemFromTextFile() {
    auto start = QDateTime::currentDateTimeUtc();
    QStringList lines(QString(_valueBytes).split("\n"));
    int i = 0;
    for(const auto &qline: lines) {
        QStringList line = qline.split("\t");
        if(line.size() != 9) {
            continue;
        }
        auto it = line.begin();
        auto name = READ_STR;
        auto x = READ_FLOAT;
        auto y = READ_FLOAT;
        auto z = READ_FLOAT;

        QList<int8_t> numPlanets;
        numPlanets.append(READ_CHAR); // elw
        numPlanets.append(READ_CHAR); // ww
        numPlanets.append(READ_CHAR); // wwt
        numPlanets.append(READ_CHAR); // aw
        numPlanets.append(READ_CHAR); // tf

        System *current = _router->findSystemByName(name);
        if(current) {
            current->setNumPlanets(numPlanets);
        } else {
            auto system = System(name, x, y, z);
            system.setNumPlanets(numPlanets);
            _router->addSystem(system);
        }
        if(!(i++ % 100)) {
            emit progress(50 + (int) (i / (float) lines.size() * 50));
        }
    }
    emit progress(100);
}

void SystemLoader::loadSettlementTypes() {
    QFile systemData(":/basetypes.csv");
    if(!systemData.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QStringList lines(QString(systemData.readAll()).split("\n"));
    _settlementTypes.clear();
    for(const auto &qline: lines) {
        QStringList line = qline.split("\t");
        if(line.size() < SETTLEMENT_TYPE_FIELD_COUNT) {
            continue;
        }
        auto it = line.begin();
        auto sizeInt = READ_INT;
        SettlementSize size;
        switch(sizeInt) {
        case 3:
            size = SettlementSizeLarge;
            break;
        case 2:
            size = SettlementSizeMedium;
            break;
        default:
        case 1:
            size = SettlementSizeSmall;
            break;
        }
        auto layout = READ_STR;
        auto securityStr = READ_STR;
        ThreatLevel security;
        if(securityStr == "High") {
            security = ThreatLeveLHigh;
        } else if(securityStr == "Medium") {
            security = ThreatLevelMedium;
        } else {
            security = ThreatLevelLow;
        }
        SKIP_FIELD; // idx
        SKIP_FIELD; // size game
        SKIP_FIELD; // Model (i.e size string)
        SKIP_FIELD; // Variation
        SKIP_FIELD; // Variation continued
        SKIP_FIELD; // Type
        auto economy = READ_STR; // Military etc
        auto iconUrl = READ_URL;

        READ_URL_JPEG(showUrl);
        READ_URL_JPEG(coreFullUrl);
        READ_URL_JPEG(overviewUrl);
        READ_URL_JPEG(pathUrl);
        READ_URL_JPEG(overview3DUrl);
        READ_URL_JPEG(coreUrl);
        READ_URL_JPEG(satelliteUrl);

        auto settlementType = new SettlementType(size, security, economy);
        settlementType->addImage(SettlementType::IMAGE_BASE_ICON, iconUrl);
        settlementType->addImage(SettlementType::IMAGE_CORE, coreUrl.isValid() ? coreUrl : showUrl);
        settlementType->addImage(SettlementType::IMAGE_COREFULLMAP, coreFullUrl);
        settlementType->addImage(SettlementType::IMAGE_PATHMAP, pathUrl);
        settlementType->addImage(SettlementType::IMAGE_OVERVIEW, overviewUrl);
        settlementType->addImage(SettlementType::IMAGE_OVERVIEW3D, overview3DUrl);
        settlementType->addImage(SettlementType::IMAGE_SATELLITE, satelliteUrl);
        _settlementTypes[layout] = settlementType;
    }
}


void SystemLoader::loadSettlements() {
    loadSettlementTypes();

    QFile systemData(":/dbdump.csv");
    if(!systemData.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QStringList lines(QString(systemData.readAll()).split("\n"));
    lines.removeFirst(); // Header
    QMap<QString, System *> lookup;
    _systems.clear();
    _systems.reserve(lines.size());

    for(const auto &qline: lines) {
        QStringList line = qline.split("\t");
        if(line.size() < EXPECTED_FIELD_COUNT) {
            continue;
        }

        int32 flags = 0;

        auto it = line.begin();

        auto typeStr = READ_STR; // Settlement type
        auto type = _settlementTypes[typeStr];
        if(!type) {
            qDebug() << "Failed to load settlement type for" << typeStr << " - skipping.";
            continue;
        }
        auto system = READ_STR; // System name
        auto planet = READ_STR; // Planet Name
        auto name = READ_STR; // Settlement name
        SKIP_FIELD; // Map link that's just "map" in file
        if(READ_BOOL) { flags |= SettlementFlagsAnarchy; } // isAnarchy

        auto x = READ_FLOAT; // x coordinate
        auto y = READ_FLOAT; // y coordinate
        auto z = READ_FLOAT; // z coordinate

        SKIP_FIELD; // distance from origin, not used by app
        auto threat = static_cast<ThreatLevel>(1 << READ_INT); // threat level

        if(READ_BOOL) { flags |= SettlementFlagsCoreDataTerminal; }  // has coredata node
        if(READ_BOOL) { flags |= SettlementFlagsJumpClimbRequired; } // needs jumping

        SKIP_FIELD; // Commander who provided data
        SKIP_FIELD; // Notes/comments

        // Data material flags
        QMap<SettlementFlags, float> materials;
        READ_MATERIAL(SettlementFlagsUnusualEncryptedFiles);
        READ_MATERIAL(SettlementFlagsSpecializedLegacyFirmware);
        READ_MATERIAL(SettlementFlagsTaggedEncryptionCodes);
        READ_MATERIAL(SettlementFlagsModifiedConsumerFirmware);
        READ_MATERIAL(SettlementFlagsClassifiedScanDatabanks);
        READ_MATERIAL(SettlementFlagsCrackedIndustrialFirmware);
        READ_MATERIAL(SettlementFlagsOpenSymmetricKeys);
        READ_MATERIAL(SettlementFlagsSecurityFirmwarePatch);
        READ_MATERIAL(SettlementFlagsDivergentScanData);
        READ_MATERIAL(SettlementFlagsModifiedEmbeddedFirmware);
        READ_MATERIAL(SettlementFlagsClassifiedScanFragment);

        auto size = READ_STR;
        SKIP_FIELD; // idx

        Settlement settlement(name, flags, threat, type, materials);
        const auto distance = getDistance(system, planet);
        if(lookup.contains(system)) {
            lookup[system]->addSettlement(planet, settlement, distance);
        } else {
            Planet planetObj(planet, distance, settlement);
            System systemObj(system, planetObj, x, y, z);
            _systems.push_back(systemObj);
            lookup[system] = &_systems.last();

            if(!_router->findSystemByName(system)) {
                _router->addSystem(systemObj);
            }
        }
    }
}

void SystemLoader::dataDecompressed(const QByteArray &bytes) {
    _bytes = bytes;
    if(!_bytes.isEmpty() && !_valueBytes.isEmpty()) {
        start();
    }
}

void SystemLoader::valuableSystemDataDecompressed(const QByteArray &bytes) {
    _valueBytes = bytes;
    if(!_bytes.isEmpty() && !_valueBytes.isEmpty()) {
        start();
    }
}


QString System::formatDistance(int64 dist, bool trunc) {
    if(dist > 0) {
        return trunc
               ? QString("%1").arg(static_cast<int64>(dist / 10.0))
               : QString("%1.%2").arg(dist / 10).arg(dist % 10);
    } else {
        return "0.0";
    }
}

System::System(AStarSystemNode *system)
        : _name(system->name()), _position(system->position()) {}

void System::addSettlement(const QString &planetName, const Settlement &settlement, int distance) {
    for(auto &planet: _planets) {
        if(planet.name() == planetName) {
            planet.addSettlement(settlement);
            return;
        }
    }
    _planets.push_back(Planet(planetName, distance, settlement));
}

System::~System() = default;


System::System(const QJsonObject &jsonObject)
        : _name(jsonObject["name"].toString()), _planets(), _position(), _numPlanets() {
    auto coords = jsonObject["coords"].toObject();
    _position.setX((float) coords["x"].toDouble());
    _position.setY((float) coords["y"].toDouble());
    _position.setZ((float) coords["z"].toDouble());
}

SystemLoader::~SystemLoader() = default;

int SystemLoader::getDistance(const QString &system, const QString &planet) {
    auto systemValue = _bodyDistances.value(system);
    return systemValue.isObject() ? systemValue.toObject().value(planet).toInt(0) : 0;
}


const QString SettlementType::IMAGE_BASE_ICON = "Icon";
const QString SettlementType::IMAGE_SATELLITE = "Satellite Map";
const QString SettlementType::IMAGE_COREFULLMAP = "Core Full Map";
const QString SettlementType::IMAGE_OVERVIEW = "Overview Map";
const QString SettlementType::IMAGE_PATHMAP = "Datapoint Path Map";
const QString SettlementType::IMAGE_OVERVIEW3D = "3D Overview";
const QString SettlementType::IMAGE_CORE = "Core Map";

const QString System::formatPlanets() const {
    QStringList planets;
    if(_numPlanets.count() == ValuableBodyFlagsCount) {
        addSystemString(planets, ValuableBodyFlagsEW, "Earth-like World");
        addSystemString(planets, ValuableBodyFlagsWT, "Water World (TF)");
        addSystemString(planets, ValuableBodyFlagsWW, "Water World");
        addSystemString(planets, ValuableBodyFlagsAW, "Ammonia World");
        addSystemString(planets, ValuableBodyFlagsTF, "Other TF");
    }
    return planets.join("\n");
}

int32 System::estimatedValue() const {
    int32 value = 0;
    if(_numPlanets.count() == ValuableBodyFlagsCount) {
        value += 552 * _numPlanets[ValuableBodyFlagsEW];
        value += 370 * (_numPlanets[ValuableBodyFlagsWW] - _numPlanets[ValuableBodyFlagsWT]);
        value += 611 * _numPlanets[ValuableBodyFlagsWT];
        value += 281 * _numPlanets[ValuableBodyFlagsAW];
        value += 362 * _numPlanets[ValuableBodyFlagsTF];
    }
    return value;
}

void System::addSystemString(QStringList &list, ValuableBodyFlags type, QString name) const {
    int8_t numPlanets = _numPlanets[type];
    if(type == ValuableBodyFlagsWW) {
        numPlanets -= _numPlanets[ValuableBodyFlagsWT];
    }
    switch(numPlanets) {
    case 0:
        break;
    case 1:
        list.append(name);
        break;
    default:
        list.append(QString("%1x%2").arg(name).arg(numPlanets));
        break;
    }
}
