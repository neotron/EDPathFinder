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

#include "AStarRouter.h"

AStarResult::AStarResult(const AStarSystemList &solution) : _route(solution.size()), _valid(true) {
    double distance = 0;
    AStarSystemNode *last(0);
    size_t i = 0;
    for(auto asys: solution) {
        if(last) {
            distance += asys->distanceTo(last);
        }
        _route[i++] = System(asys);
        last = asys;
    }
    _distance = distance;
}


std::vector<std::pair<Node *, float>> &AStarSystemNode::getChildren() {
    if(m_children.size()) {
        return m_children;
    }
    // All systems have all others as children. sup yo.
    for(auto system: _calculator.systems()) {
        float dist = distanceTo(system);
        if(dist > 0.0f && dist < _calculator.jumpRange()) {
            addChild(system, dist);
        }
    }
//    qDebug() << name().c_str() << "->" << m_children.size();
    return m_children;
}


AStarResult AStarRouter::calculateRoute(const std::string &begin, const std::string &end, double jumprange) {
    System *beginSys = getSystemByName(begin);
    System *endSys = getSystemByName(end);
    if(beginSys && endSys) {
        AStarCalculator calculator(_systems, *beginSys, *endSys, jumprange);
        return calculator.solve();
    }
    return AStarResult();
}

AStarCalculator::~AStarCalculator() {
    for(auto sys: _nodes) {
        delete sys;
    }
    _nodes.clear();
}

void AStarCalculator::cylinder(const SystemList &stars, QVector3D vec_from, QVector3D vec_to, double buffer) {
    for(const auto &s: stars) {
        auto numerator = QVector3D::crossProduct(s.position() - vec_from, s.position() - vec_to).length();
        auto denominator = (vec_to - vec_from).length();
        auto dist = numerator / denominator;
        if(dist < buffer) {
            auto system = new AStarSystemNode(*this, s.name(), s.position());
            _nodes.push_back(system);
            if(s.position() == vec_from) {
                _start = system;
            } else if(s.position() == vec_to) {
                _end = system;
            }
        }
    }
}

AStarResult AStarCalculator::solve() {
    if(!_start || !_end) {
        return AStarResult();
    }
    //qDebug() << "Beginning to solve route - have" << _nodes.size() << "candidates: "<<_start->name().c_str()<<"to"<<_end->name().c_str();

    AStarSystemList solution;
    PathFinder<AStarSystemNode> finder;
    finder.setStart(*_start);
    finder.setGoal(*_end);
    bool result = finder.findPath<AStar>(solution);
    if(result) {
        //qDebug() << "Found path with " << solution.size() << " jumps.";
        AStarSystemNode *last(Q_NULLPTR);
        for(auto sys: solution) {
        //    qDebug() << "  " << sys->name().c_str() << "->" << (last ? last->distanceTo(sys) : 0.0);
            last = sys;
        }
        return AStarResult(solution);
    }
    return AStarResult();
}


