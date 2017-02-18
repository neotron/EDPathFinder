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


// Based on Traveling Salesman Sample from Google Optimzation tools (tsp.cpp),
// which was released under the following licensing terms:
//
// Copyright 2010-2014 Google
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


#pragma once

#include <QList>
#include <QThread>
#include <constraint_solver/routing.h>
#include "System.h"
#include "AStarRouter.h"

typedef std::vector<std::vector<QString>> RouteResultMatrix;

class RouteSystemPlanetSettlement {

public:
    RouteSystemPlanetSettlement(const QString &systemName, const QString &planetName, int distance,
                                const Settlement &settlement)
            : _systemName(systemName), _planetName(planetName), _settlement(settlement), _distance(distance) { }

    const QString &systemName() const {
        return _systemName;
    }

    const QString &planetName() const {
        return _planetName;
    }

    const Settlement &settlement() const {
        return _settlement;
    }

    int distance() const {
        return _distance;
    }

private:
    const QString _systemName;
    const QString _planetName;
    const Settlement _settlement;
    int _distance;
};

class RouteResult {
public:

    RouteResult() : _route(), _totalDist(0) { }

    void addEntry(const System &system, const Planet &planet, const Settlement &settlement, int64 distance);

    const QString ly() const {
        return System::formatDistance(_totalDist);
    }

    const RouteResultMatrix &route() const {
        return _route;
    }

    const RouteSystemPlanetSettlement *getSettlementAtIndex(int index) const {
        if(index < 0 || static_cast<size_t>(index) >= _settlements.size()) {
            return nullptr;
        }
        return &_settlements[static_cast<size_t>(index)];
    }

    bool isValid() const {
        return route().size() != 0;
    }


    virtual ~RouteResult();

private:
    RouteResultMatrix _route;
    int64             _totalDist;
    std::vector<RouteSystemPlanetSettlement> _settlements;
};

namespace operations_research {
    class TSPWorker : public QThread {
    Q_OBJECT

    public:
        TSPWorker(SystemList systems, System *system, int maxSystemCount)
                : QThread(), _systems(systems), _origin(system), _maxSystemCount(maxSystemCount), _router(Q_NULLPTR),
                  numDist(0) { }


        virtual void run();


        void setRouter(AStarRouter *router) {
            _router = router;
        }

    signals:
        void taskCompleted(const RouteResult &route);

    private:
        void                    calculateDistanceMatrix();

        int64                   systemDistance(RoutingModel::NodeIndex from, RoutingModel::NodeIndex to);

        SystemList              _systems;
        System                  *_origin;
        int                     _maxSystemCount;
        AStarRouter             *_router;
        int                     numDist;
        QVector<QVector<int64>> _distanceMatrix;

        int64 calculateDistance(int from, int to);
    };
};

using operations_research::TSPWorker;

