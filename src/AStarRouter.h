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

#pragma once

#include <QtGui>
#include <deps/PathFinder/src/PathFinder.h>
#include <deps/PathFinder/src/AStar.h>
#include "System.h"

class AStarRouter;

class AStarSystemNode;

class AStarCalculator;

typedef std::vector<AStarSystemNode *> AStarSystemList;

class AStarResult {

public:


    AStarResult() : _valid(false) { }

    explicit AStarResult(const AStarSystemList &solution);

    const SystemList &route() const {
        return _route;
    }

    float distance() const {
        return _distance;
    }


    bool valid() const {
        return _valid;
    }

private:
    SystemList _route;
    float      _distance{};
    bool       _valid;
};

class AStarSystemNode : public AStarNode {
public:
    AStarSystemNode(AStarCalculator &calculator, const System &system)
            : AStarNode(), _system(system), _calculator(calculator) { }

    ~AStarSystemNode() override = default;

    // Distance between this and another node, used by A* algorithm.
    float distanceTo(AStarNode *node) const override {
        auto other = (AStarSystemNode *) node;
        return _system.position().distanceToPoint(other->_system.position());
    }

    const QString &name() const { return _system.name(); }

    const QVector3D &position() const { return _system.position(); }

    std::vector<std::pair<Node *, float>> &getChildren() override;

private:
    const System    &_system;
    AStarCalculator &_calculator;
};

class AStarCalculator : public QObject {
Q_OBJECT

public:
    AStarCalculator(const SystemList &systems, const System &start, const System &end, float jumprange,
                    QObject *parent = Q_NULLPTR) : QObject(parent), _start(Q_NULLPTR), _end(Q_NULLPTR),
                                                   _jumpRange(jumprange), _nodes() {
        cylinder(systems, start.position(), end.position(), 40.0);
    }

    ~AStarCalculator() override;

    void cylinder(const SystemList &stars, QVector3D vec_from, QVector3D vec_to, float buffer);

    float jumpRange() const {
        return _jumpRange;
    }

    const AStarSystemList &systems() const {
        return _nodes;
    }

    AStarResult solve();

private:
    AStarSystemNode *_start, *_end;
    float           _jumpRange;
    AStarSystemList _nodes;
};

class AStarRouter : public QAbstractItemModel {
Q_OBJECT

public:

    explicit AStarRouter(QObject *parent = Q_NULLPTR) : QAbstractItemModel(parent), _systems(), _systemLookup() { }


    ~AStarRouter() override = default;

    void addSystem(const System &system) {
        _systems.push_back(system);
        _systemLookup[system.name().toLower()] = &_systems.back();
    }

    AStarResult calculateRoute(const QString &begin, const QString &end, float jumprange);

    System *findSystemByName(const QString &name) {
        auto lowerName = name.toLower();
        return _systemLookup.contains(lowerName) ? _systemLookup[lowerName] : Q_NULLPTR;
    }

    const SystemList &systems() const {
        return _systems;
    }

    QModelIndex index(int row, int column, const QModelIndex &) const override;

    QModelIndex parent(const QModelIndex &) const override;

    int rowCount(const QModelIndex &) const override;

    int columnCount(const QModelIndex &) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void sortSystemList();
protected:
    friend class SystemLoader;
    void reserveSystemSpace(int size) {
        _systems.reserve(size);
    }

private:
    SystemList              _systems;
    QMap<QString, System *> _systemLookup;
};



