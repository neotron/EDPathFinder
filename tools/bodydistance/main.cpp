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
#include <src/System.h>
#include <src/AStarRouter.h>

int main() {
    AStarRouter  router;
    SystemLoader loader(&router);

    loader.loadSettlements();
    auto systems                                   = loader.systems();
    qDebug() << systems.size();

    QMap<QString, QMap<QString, QPair<QString,QString>>> bodyLookup;
    int                                  bodycount = 0;
    for(auto                             system: systems) {
        for(auto planet: system.planets()) {
            bodyLookup[system.name().toLower()][planet.name().toLower()] = QPair<QString,QString>(system.name(), planet.name());
        }
        bodycount++;
    }
    qDebug() << "Found" << bodycount << "bodies";
    QMap<int, QString> systemIdToName;
    QFile              file("systems_populated.jsonl");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Couldn't open file for reading.";
        return -1;
    }
    int found = 0;
    while(!file.atEnd()) {
        auto lineBytes = file.readLine();
        if(lineBytes.isEmpty()) {
            continue;
        }
        auto doc = QJsonDocument::fromJson(lineBytes);
        if(doc.isEmpty() || !doc.isObject()) {
            continue;
        }
        auto    jsonObject = doc.object();
        int     id         = jsonObject.value("id").toInt(0);
        QString name       = jsonObject.value("name").toString();
        if(id && !name.isEmpty()) {
            systemIdToName[id] = name.toLower();
            if(bodyLookup.contains(name.toLower())) {
                ++found;
            }
        }
    }
    qDebug() << "Loaded" << systemIdToName.size() << "system ids" << "and found" << found;


    file.close();
    QFile bodies("bodies.jsonl");
    if(!bodies.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Couldn't open file for reading.";
        return -1;
    }
    int systemmatch = 0;
    int                  i = 0;
    QList<QFuture<void>> futures;
    QMap<QString,QMap<QString,int>> distances;
    while(!bodies.atEnd()) {
        auto linebytes = bodies.readLine();
        if(linebytes.isEmpty()) {
            continue;
        }
        auto doc = QJsonDocument::fromJson(linebytes);
        if(!doc.isObject()) {
            continue;
        }
        auto jsonObject = doc.object();
        int  dist       = jsonObject.value("distance_to_arrival").toInt(0);
        if(!dist) {
            continue;
        }
        int id = jsonObject.value("system_id").toInt(0);
        if(!systemIdToName.contains(id)) {
            continue;
        }
        auto    system = systemIdToName[id];
        QString name   = jsonObject.value("name").toString().toLower();
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
