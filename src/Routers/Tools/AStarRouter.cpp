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

AStarResult::AStarResult(const AStarSystemList &solution) : _route(), _valid(true) {
    float distance = 0;

    AStarSystemNode *last(0);

    for(auto asys: solution) {
        if(last) {
            distance += asys->distanceTo(last);
        }
        _route.append(System(asys));
        last = asys;
    }
    _distance      = distance;
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


AStarResult AStarRouter::calculateRoute(const QString &begin, const QString &end, float jumprange) {
    System *beginSys = findSystemByName(begin);
    System *endSys   = findSystemByName(end);
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

// Return all systems in a cylinder formed between the two vectors
void AStarCalculator::cylinder(const SystemList &stars, QVector3D vec_from, QVector3D vec_to, float buffer) {
    auto bufferSquare = buffer * buffer;
    for(const auto &s: stars) {
        auto numerator = QVector3D::crossProduct(s.position() - vec_from, s.position() - vec_to).lengthSquared();
        auto denominator = (vec_to - vec_from).lengthSquared();
        auto dist = numerator / denominator;
        if(dist < bufferSquare) {
            auto systemNode = new AStarSystemNode(*this, s);
            _nodes.push_back(systemNode);
            if(s.position() == vec_from) {
                _start = systemNode;
            } else if(s.position() == vec_to) {
                _end = systemNode;
            }
        }
    }
    //qDebug() << "Cylinder of systems contain"<<_nodes.size()<<"nodes.";
}

AStarResult AStarCalculator::solve() {

    if(!_start || !_end) {
        return AStarResult();
    }
    //qDebug() << "Beginning to solve route - have" << _nodes.size() << "candidates: "<<_start->name().c_str()<<"to"<<_end->name().c_str();

    AStarSystemList             solution;
    PathFinder<AStarSystemNode> finder;
    finder.setStart(*_start);
    finder.setGoal(*_end);
    bool result = finder.findPath<AStar>(solution);

    return result ? AStarResult(solution) : AStarResult();
}


QVariant AStarRouter::data(const QModelIndex &index, int role) const {
    QMutexLocker lock(&_lock);
    if((role == Qt::EditRole || role == Qt::DisplayRole) && index.row() < (int) _systems.size() &&
       index.column() == 0) {
        // This is incredibly ugly but allows for lazy sorting.
        const_cast<AStarRouter*>(this)->sortSystemList();
        return _systems[index.row()].name();
    }
    return QVariant();
}

int AStarRouter::columnCount(const QModelIndex &) const {
    return 1;
}

int AStarRouter::rowCount(const QModelIndex &) const {
    QMutexLocker lock(&_lock);
    return (int) _systems.size();
}

QModelIndex AStarRouter::parent(const QModelIndex &) const {
    return {};
}

QModelIndex AStarRouter::index(int row, int column, const QModelIndex &) const {
    return createIndex(row, column);
}

void AStarRouter::sortSystemList() {
    QMutexLocker lock(&_lock);
    if(_isUnsorted) {
        _isUnsorted = false;
        beginResetModel();
        std::sort(_systems.begin(), _systems.end());
        _systemLookup.clear();
        for(auto &system: _systems) {
            _systemLookup[system.key()] = &system;
        }
        endResetModel();
    }
}

System *AStarRouter::findSystemByKey(const std::string &key) {
    auto found = _systemLookup.find(key);
    return found == _systemLookup.end() ? nullptr : found->second;
}

System *AStarRouter::findSystemByName(const QString &name) {
    auto lowerName = name.toLower();
    auto found = _systemLookup.find(lowerName.toStdString());
    return found == _systemLookup.end() ? nullptr : found->second;
}

void AStarRouter::addSystem(const System &system) {
    QMutexLocker lock(&_lock);
    _isUnsorted = true;
    _systems.push_back(system);
    _systemLookup[system.key()] = &_systems.back();
}
