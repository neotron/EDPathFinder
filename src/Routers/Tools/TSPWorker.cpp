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


#include <QDebug>
#include <QtConcurrent>
#pragma warning(push, 0)
#include <ortools/constraint_solver/routing_flags.h>
#pragma warning(pop)
#include "System.h"
#include "TSPWorker.h"

namespace operations_research {

// Cost/distance functions.
    int64 TSPWorker::systemDistance(RoutingModel::NodeIndex from, RoutingModel::NodeIndex to) {
        return _distanceMatrix[from.value()][to.value()];
    }

    int64 TSPWorker::calculateDistance(int from, int to) {
        auto &fromSystem = _systems[from];
        auto &toSystem = _systems[to];
        ++_numDist;
        if(_router) {
            AStarResult result = _router->calculateRoute(fromSystem.name(), toSystem.name(), 15.0f);
            if(result.valid()) {
                return (int64) result.route().size() * 1000 + fromSystem.distance(toSystem);
            } else {
                return INT64_MAX;
            }
        }
        return fromSystem.distance(toSystem);
    }

    void TSPWorker::calculateDistanceMatrix() {
        auto sz = _systems.size();
        _distanceMatrix.resize(sz);
        typedef QPair<QFuture<int64>, QPair<int, int>> FuturePair;
        QList<FuturePair> futures;

        for(int from = 0; from < sz; from++) {
            _distanceMatrix[from].fill(-1, sz);
        }

        for(int from = 0; from < sz; from++) {
            for(int to = 0; to < sz; to++) {
                int64 dist = 0;
                if(from != to) {
                    if(_distanceMatrix[to][from] == -1) {
                        auto future = QtConcurrent::run(this, &TSPWorker::calculateDistance, from, to);
                        auto destPair = QPair<int, int>(from, to);
                        futures.push_back(FuturePair(future, destPair));
                        _distanceMatrix[to][from] = -2;
                        _distanceMatrix[from][to] = -2;
                        continue;
                    }
                } else {
                    _distanceMatrix[from][to] = dist;
                }
            }
        }
        qDebug() << "Waiting for completion of" << futures.size() << "futures";
        for(auto &pair: futures) {
            pair.first.waitForFinished();
            auto a = pair.second.first;
            auto b = pair.second.second;
            _distanceMatrix[a][b] = _distanceMatrix[b][a] = pair.first.result();
        }
    }

    void TSPWorker::cylinder(QVector3D vec_from, QVector3D vec_to, float buffer) {

        auto bufferSquare = buffer * buffer;
        typedef QPair<System,float> SystemDist;
        QList<SystemDist> filteredSystems;
        auto originDestDist = vec_from.distanceToPoint(vec_to)+buffer;

        for(const auto &s: _systems) {
            auto numerator = QVector3D::crossProduct(s.position() - vec_from, s.position() - vec_to).lengthSquared();
            auto denominator = (vec_to - vec_from).lengthSquared();
            auto dist = numerator / denominator;
            if(dist < bufferSquare
               && s.position().distanceToPoint(vec_from) < originDestDist
               && s.position().distanceToPoint(vec_to) < originDestDist) {
                filteredSystems.push_back(SystemDist(s, dist));
            }
        }

        // qDebug() << "Cylinder of systems contain"<<filteredSystems.size()<<"nodes.";
        std::sort(filteredSystems.begin(), filteredSystems.end(), [] (const SystemDist& a, const SystemDist &b) {
            return a.second < b.second;
        });

        _systems.clear();
        for(int i = 0; i < filteredSystems.count() && i < _maxSystemCount; i++) {
            _systems.push_back(filteredSystems[i].first);
        }
    }

    void TSPWorker::run() {
        System *startingSystem = _origin;
        if(!startingSystem) {
            startingSystem = &_systems[0];
        }
        QTime timer;
        timer.start();
        if(_destination) {
            if(!_isPresets) {
                cylinder(startingSystem->position(), _destination->position(), 200);
            }
            _systems.push_back(*_destination);
        } else {
            std::sort(_systems.begin(), _systems.end(), [startingSystem](const System &a, const System &b) {
                return a.distance(*startingSystem) < b.distance((*startingSystem));
            });
            if(_maxSystemCount < _systems.size()) {
                _systems.erase(_systems.begin() + _maxSystemCount, _systems.end());
            }
        }

        // Calculate the closest system
        if(_origin && (!_systems.size() || _origin->name() != _systems[0].name())) {
            _systems.push_front(*_origin);
        }
        //qDebug() << "Sorting and resizing took " << timer.elapsed();
        timer.restart();
        calculateDistanceMatrix();
        //qDebug() << "Matrix calculation took " << timer.elapsed();
        timer.restart();
        RoutingModel routing((int) _systems.size(), 1, RoutingModel::NodeIndex(0));
        auto parameters = BuildSearchParametersFromFlags();

        // Setting first solution heuristic (cheapest addition).
        parameters.set_first_solution_strategy(FirstSolutionStrategy::AUTOMATIC);
        parameters.set_time_limit_ms(1000);
        //parameters.set_solution_limit(35);
        //parameters.set_log_search(true);
        routing.SetArcCostEvaluatorOfAllVehicles(NewPermanentCallback(this, &TSPWorker::systemDistance));

        if(_destination) {
            auto endNode = RoutingModel::NodeIndex(_systems.size() - 1);
            for(int i = 1; i < _systems.size() - 1; i++) {
                routing.AddPickupAndDelivery(RoutingModel::NodeIndex(i), endNode);
            }
        }

        // Solve, returns a solution if any (owned by RoutingModel).
        auto solution = routing.SolveWithParameters(parameters);
        //qDebug() << "RoutingTools took " << timer.elapsed();

        // Populate result.
        RouteResult result;
        if(solution) {
            // Inspect solution.
            // Only one route here; otherwise iterate from 0 to routing.vehicles() - 1
            const int route_number = 0;
            int nodeid;
            int previd = 0;
            int64 dist = 0;

            for(int64 node = routing.Start(route_number); !routing.IsEnd(node);
                node = solution->Value(routing.NextVar(node))) {
                nodeid = routing.IndexToNode(node).value();

                const auto &sys = _systems[nodeid];
                const auto &prevSystem = _systems[previd];

                if(nodeid > 0) {
                    dist = sys.distance(prevSystem);
                }

                previd = nodeid;
                if(_isPresets) {
                    result.addEntryWithPresets(sys, dist);
                } else  if(_systemsOnly) {
                    result.addEntry(sys, dist);
                } else {
                    if(sys.planets().empty()) {
                        continue;
                    }
                    for(const auto &planet: sys.planets()) {
                        for(const auto &settlement: planet.settlements()) {
                            result.addEntry(sys, planet, settlement, dist);
                            dist = 0;
                        }
                    }
                }
            }
        }
        emit taskCompleted(result);
    }

    void TSPWorker::setIsPresets(bool isPresets) {
        _isPresets = isPresets;
    }

    void TSPWorker::setDestination(System *destination) {
        _destination = destination;
    }

    void TSPWorker::setSystemsOnly(bool systemsOnly) {
        _systemsOnly = systemsOnly;
    }

    void TSPWorker::setRouter(AStarRouter *router) {
        _router = router;
    }
}

void RouteResult::addEntry(const System &system, int64 distance) {
    _totalDist += distance;
    int32 estimatedValue = system.estimatedValue();
    _totalValue += estimatedValue;
    std::vector<QString> row(4);
    row[0] = system.name();
    row[1] = QString("%1 / %2").arg(System::formatDistance(distance, true)).arg(System::formatDistance(_totalDist, true));
    row[2] = system.formatPlanets();
    row[3] = QString("%1k / %2M").arg(estimatedValue).arg(_totalValue/1000);
    _route.emplace_back(row);
}

void RouteResult::addEntry(const System &system, const Planet &planet, const Settlement &settlement, int64 distance) {
    _totalDist += distance;
    std::vector<QString> row(6);
    row[0] = system.name();
    row[1] = planet.name();
    row[2] = settlement.name();
    row[3] = System::formatDistance(distance, true);
    row[4] = System::formatDistance(_totalDist, true);
    _route.emplace_back(row);

    auto routeSettlement = RouteSystemPlanetSettlement(system.name(), planet.name(), planet.distance(), settlement);
    _settlements.emplace_back(routeSettlement);
}

void RouteResult::addEntryWithPresets(const System &system, int64 distance) {
    _totalDist += distance;
    std::vector<QString> row(5);
    row[0] = system.name();
    row[1] = QString("%1 / %2").arg(System::formatDistance(distance, true)).arg(System::formatDistance(_totalDist, true));

    const auto &preset = system.presetEntry();
    if(preset.isValid()) {
        row[2] = preset.type();
        row[3] = preset.shortDescription();
        row[4] = preset.details();
    } else if(!_route.size()) {
        row[2] = "Origin";
        row[3] = "Starting System";
    } else {
        row[2] = "Destination";
    }
    _route.emplace_back(row);
}

RouteResult::~RouteResult() = default;
