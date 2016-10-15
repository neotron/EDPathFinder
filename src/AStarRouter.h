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

#pragma once

#include <QtGui>
#include <deps/PathFinder/src/PathFinder.h>
#include <deps/PathFinder/src/AStar.h>
#include <unordered_map>
#include "System.h"

class AStarRouter;
class AStarSystemNode;
class AStarCalculator;

typedef std::vector<AStarSystemNode *>  AStarSystemList;

class AStarResult {

public:


    AStarResult(): _valid(false) { }

    AStarResult(const AStarSystemList &solution);

    const SystemList &route() const {
        return _route;
    }

    double distance() const {
        return _distance;
    }


    bool valid() const {
        return _valid;
    }

private:
    SystemList _route;
    double _distance;
    bool _valid;
};

class AStarSystemNode : public AStarNode {
public:
    AStarSystemNode(AStarCalculator &calculator, const System &system) : AStarNode(), _system(system), _calculator(calculator) { }
    virtual ~AStarSystemNode() { }

    // Distance between this and another node, used by A* algorithm.
    virtual float distanceTo(AStarNode *node) const override {
        auto other = (AStarSystemNode *)node;
        return _system.position().distanceToPoint(other->_system.position());
    }

    const std::string &name() const { return _system.name(); }
    const QVector3D &position() const { return _system.position(); }

    virtual std::vector<std::pair<Node *, float>> &getChildren() override;

private:
    const System &_system;
    AStarCalculator &_calculator;
};

class AStarCalculator : public QObject {
    Q_OBJECT

public:
    AStarCalculator(const SystemList &systems, const System &start, const System &end, double jumprange, QObject *parent = Q_NULLPTR) : QObject(parent), _start(Q_NULLPTR), _end(Q_NULLPTR), _jumpRange(jumprange),_nodes() {
        cylinder(systems, start.position(), end.position(), 40.0);
    }

    virtual ~AStarCalculator();

    void cylinder(const SystemList &stars, QVector3D vec_from, QVector3D vec_to, float buffer);

    double jumpRange() const {
        return _jumpRange;
    }

    const AStarSystemList &systems() const {
        return _nodes;
    }

    AStarResult solve();

private:
    AStarSystemNode *_start, *_end;
    double _jumpRange;
    AStarSystemList _nodes;
};

class AStarRouter : QObject {
    Q_OBJECT

public:

    AStarRouter(QObject *parent = Q_NULLPTR) : QObject(parent), _systems(), _systemLookup() { }


    virtual ~AStarRouter() {

    }

    void addSystem(const System &system) {
        _systems.push_back(system);
        _systemLookup[lower(system.name())] = &_systems.back();
    }

    AStarResult calculateRoute(const std::string &begin, const std::string &end, double jumprange);

    System *getSystemByName(const std::string &name) {
        auto lowerName = lower(name);
        return _systemLookup.find(lowerName) != _systemLookup.end() ? _systemLookup[lowerName] : Q_NULLPTR;
    }

private:

    std::string lower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

    SystemList _systems;
    std::unordered_map<std::string, System *> _systemLookup;
};



