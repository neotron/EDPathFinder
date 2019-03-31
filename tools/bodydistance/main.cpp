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

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "SystemLoader.h"
#include "AStarRouter.h"

int main() {
    AStarRouter router;
    SystemLoader loader(&router);

    loader.loadSettlements();
    auto systems = loader.systems();
    qDebug() << systems.size();

    QMap<QString, QMap<QString, QPair<QString, QString>>> bodyLookup;
    int bodycount = 0;
    int systemcount = 0;
    for(auto system: systems) {
        if(!system.hasPlanets()){ continue; }
        for(auto planet: system.planets()) {
            bodyLookup[system.name().toLower()][planet.name().toLower()] = QPair<QString, QString>(system.name(),
                                                                                                   planet.name());
            bodycount++;
        }
        systemcount++;
    }
    qDebug() << "Found" << bodycount << "bodies in "<< systemcount<<"systems.";
    QMap<int, QString> systemIdToName;
    QFile file("../data/systems-populated.jsonl");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Couldn't open systems file for reading.";
        return -1;
    }
    int found = 0;
    int numLines = 0;
    while(!file.atEnd()) {
        auto lineBytes = file.readLine();
        if(lineBytes.isEmpty()) {
            continue;
        }
        auto doc = QJsonDocument::fromJson(lineBytes);
        if(doc.isEmpty() || !doc.isObject()) {
            continue;
        }
        auto jsonObject = doc.object();
        int id = jsonObject.value("id").toInt(0);
        QString name = jsonObject.value("name").toString();
        if(id && !name.isEmpty()) {
            systemIdToName[id] = name.toLower();
            if(bodyLookup.contains(name.toLower())) {
                ++found;
            }
        }
    }
    qDebug() << "Loaded" << systemIdToName.size() << "system ids" << "and found" << found;


    file.close();
    QFile bodies("../data/bodies.json");
    if(!bodies.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Couldn't open file for reading.";
        return -1;
    }
    int systemmatch = 0;
    int i = 0;
    QList<QFuture<void>> futures;
    QMap<QString, QMap<QString, int>> distances;
    qint64 numReadBytes(0);
    while(!bodies.atEnd()) {
        auto linebytes = bodies.readLine();
        numReadBytes += linebytes.size() + 1;
        if(!(++numLines % 10000)) {
            fprintf(stderr, "\rLines parsed: %7d (%3d%%)", numLines,
                    (int) (numReadBytes / (double) bodies.size() * 100));
            fflush(stderr);
        }

        if(linebytes.length() < 2) {
            continue;
        }
        linebytes.truncate(linebytes.length()-2);
        auto doc = QJsonDocument::fromJson(linebytes);
        if(!doc.isObject()) {
            continue;
        }
        auto jsonObject = doc.object();
        auto dist = static_cast<int>(ceil(jsonObject.value("distanceToArrival").toDouble(0)));
        if(!dist) {
            continue;
        }
        int id = jsonObject.value("systemId").toInt(0);
        if(!systemIdToName.contains(id)) {
            continue;
        }
        auto system = systemIdToName[id];
        QString name = jsonObject.value("name").toString().toLower();
        name.replace(system + " ", "");
        if(bodyLookup.contains(system)) {
            systemmatch++;
            if(bodyLookup[system].contains(name)) {
                auto info = bodyLookup[system][name];
                distances[info.first][info.second] = dist;
                ++i;
            }
        }
    }
    QJsonDocument outputDocument;
    QJsonObject rootObject;
    for(auto system: distances.keys()) {
        QJsonObject systemObject;
        auto sysmap = distances[system];
        for(auto planet: sysmap.keys()) {
            systemObject.insert(planet, QJsonValue(sysmap[planet]));
        }
        rootObject.insert(system, systemObject);
    }
    outputDocument.setObject(rootObject);
    fprintf(stderr, "\rLines parsed: %-7d            \n", numLines);


    std::cout << QString(outputDocument.toJson(QJsonDocument::Compact)).toStdString() << std::endl;

#if 0
    for(auto s: bodyLookup.keys()) {
        for(auto p: bodyLookup[s].keys()) {
            if(!bodyLookup[s][p]) {
                qDebug() << "--- MISSING"<<s<<p;
            }
        }
    }
#endif
//    qDebug() << "Looked at" << i << "bodies today and found"<<systemmatch<<"matching systems";
    return 0;
}
