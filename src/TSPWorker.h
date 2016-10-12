//
// Created by David Hedbor on 10/10/16.
//

#ifndef ELITETSP_TSP_H
#define ELITETSP_TSP_H

#include <deque>
#include <QThread>
#include "System.h"
#include "constraint_solver/routing.h"

typedef std::vector<std::vector<QString>> RouteResultMatrix;

struct RouteResult {
    RouteResult(): ly(), route() {}

    QString ly;
    RouteResultMatrix route;
};

namespace operations_research {
    class TSPWorker : public QThread {
    Q_OBJECT

    public:
        TSPWorker(const std::deque<System> &systems, double x, double y, double z) : QThread(), _x(x), _y(y), _z(z), _systems(systems) {
        }

        virtual void run();

    signals:
        void taskCompleted(const RouteResult &route);

    private:
        int64 systemDistance(RoutingModel::NodeIndex from, RoutingModel::NodeIndex to);
        std::deque<System> _systems;
        double _x, _y, _z;

    };
};

using operations_research::TSPWorker;

#endif //ELITETSP_TSP_H
