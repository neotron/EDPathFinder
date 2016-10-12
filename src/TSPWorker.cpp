//
// Created by David Hedbor on 10/10/16.
//

#include "System.h"
#include "constraint_solver/routing_flags.h"
#include "constraint_solver/routing.h"
#include "base/join.h"
#include "base/callback.h"
#include "TSPWorker.h"

namespace operations_research {

// Cost/distance functions.
    int64 TSPWorker::systemDistance(RoutingModel::NodeIndex from, RoutingModel::NodeIndex to) {
        return _systems[from.value()].distance(_systems[to.value()]);
    }

    void TSPWorker::run() {
        RoutingModel routing((int) _systems.size(), 1);
        routing.SetDepot(RoutingModel::NodeIndex(0));
        RoutingSearchParameters parameters = BuildSearchParametersFromFlags();

        // Setting first solution heuristic (cheapest addition).
        parameters.set_first_solution_strategy(FirstSolutionStrategy::PATH_CHEAPEST_ARC);
        routing.SetArcCostEvaluatorOfAllVehicles(NewPermanentCallback(this, &TSPWorker::systemDistance));

        // Solve, returns a solution if any (owned by RoutingModel).
        const Assignment *solution = routing.SolveWithParameters(parameters);

        // Populate result.
        RouteResult result;
        if(solution != NULL) {
            // Solution cost.
            result.ly = System::formatDistance(solution->ObjectiveValue());
            // Inspect solution.
            // Only one route here; otherwise iterate from 0 to routing.vehicles() - 1
            const int route_number = 0;
            int nodeid;
            int previd = 0;
            int64 dist = 0;
            int64 totaldist = 0;
            for(int64 node = routing.Start(route_number); !routing.IsEnd(node); node = solution->Value(routing.NextVar(node))) {
                nodeid = routing.IndexToNode(node).value();

                const System &sys = _systems[nodeid];

                if(nodeid > 0) {
                    dist = sys.distance(_systems[previd]);
                    totaldist += dist;
                }
                previd = nodeid;
                for(auto planet: sys.planets()) {
                    for(auto settlement: planet.settlements()) {
                        std::vector<QString> row(5);
                        row[0] = sys.name().c_str();
                        row[1] = planet.name().c_str();
                        row[2] = settlement.name().c_str();
                        row[3] = System::formatDistance(dist);
                        row[4] = System::formatDistance(totaldist);
                        result.route.emplace_back(row);
                    }
                }
            }
        } else {
            LOG(INFO) << "No solution found.";
        }
        emit taskCompleted(result);
    }
}
