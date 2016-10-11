//
// Created by David Hedbor on 10/10/16.
//

#ifndef ELITETSP_TSP_H
#define ELITETSP_TSP_H

#include <deque>
#include <QThread>
#include "System.h"
#include "constraint_solver/routing.h"

namespace operations_research {
    class TSPWorker : public QThread {
    Q_OBJECT

    public:
        TSPWorker(const std::deque<System> &systems) : QThread(), _systems(systems) {
        }

        virtual void run();

    private:
        int64 systemDistance(RoutingModel::NodeIndex from, RoutingModel::NodeIndex to);
        std::deque<System> _systems;

    signals:
        void taskCompleted();
    };
};

using operations_research::TSPWorker;

#endif //ELITETSP_TSP_H
