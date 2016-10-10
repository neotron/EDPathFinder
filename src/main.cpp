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
// Traveling Salesman Sample.
//
// This is a sample using the routing library to solve a Traveling Salesman
// Problem.
// The description of the problem can be found here:
// http://en.wikipedia.org/wiki/Travelling_salesman_problem.
// For small problems one can use the hamiltonian path library directly (cf
// graph/hamiltonian_path.h).
// The optimization engine uses local search to improve solutions, first
// solutions being generated using a cheapest addition heuristic.
// Optionally one can randomly forbid a set of random connections between nodes
// (forbidden arcs).

#include <memory>

#include "base/callback.h"
#include "base/commandlineflags.h"
#include "base/commandlineflags.h"
#include "base/integral_types.h"
#include "base/join.h"
#include "constraint_solver/routing.h"
#include "constraint_solver/routing_flags.h"
#include "base/random.h"
#include "System.h"

DEFINE_int32(tsp_random_forbidden_connections, 0, "Number of random forbidden connections.");
DEFINE_bool(tsp_use_deterministic_random_seed, false, "Use deterministic random seeds.");

std::deque<System> systems;

namespace operations_research {

// Random seed generator.
	int32 GetSeed() {
		if(FLAGS_tsp_use_deterministic_random_seed) {
			return ACMRandom::DeterministicSeed();
		} else {
			return ACMRandom::HostnamePidTimeSeed();
		}
	}

// Cost/distance functions.
	int64 MyDistance(RoutingModel::NodeIndex from, RoutingModel::NodeIndex to) {
		return systems[from.value()].distance(systems[to.value()]);
	}

	void Tsp() {
		RoutingModel routing((int) systems.size(), 1);
		routing.SetDepot(RoutingModel::NodeIndex(0));
		RoutingSearchParameters parameters = BuildSearchParametersFromFlags();
		// Setting first solution heuristic (cheapest addition).
		parameters.set_first_solution_strategy(FirstSolutionStrategy::PATH_CHEAPEST_ARC);

		routing.SetArcCostEvaluatorOfAllVehicles(NewPermanentCallback(MyDistance));
		// Solve, returns a solution if any (owned by RoutingModel).
		const Assignment *solution = routing.SolveWithParameters(parameters);
		if(solution != NULL) {
			// Solution cost.
			LOG(INFO) << "Total Distance: " << System::formatDistance(solution->ObjectiveValue()) << " ly";
			// Inspect solution.
			// Only one route here; otherwise iterate from 0 to routing.vehicles() - 1
			const int route_number = 0;
			std::string route;
			int nodeid;
			int64 dist = 0;
			int previd = 0;
			int64 totaldist = 0;
			for(int64 node = routing.Start(route_number);
				!routing.IsEnd(node);
				node = solution->Value(routing.NextVar(node))) {
				nodeid = routing.IndexToNode(node).value();
				const System &sys = systems[nodeid];
				if(nodeid > 0) {
					dist = sys.distance(systems[previd]);
					totaldist += dist;
				}
				previd = nodeid;
				auto settlements = sys.settlements();
				for(auto it = settlements.begin(); it != settlements.end(); ++it) {
					StrAppend(&route, sys.system(), "\t", sys.planet(), "\t", (*it).name(), "\t", System::formatDistance(dist), "\t", System::formatDistance(totaldist), "\n");
					dist = 0;
				}
			}
//      const int64 end = routing.End(route_number);
//       nodeid = routing.IndexToNode(end).value();
//      StrAppend(&route, systems[nodeid].system, " (", nodeid, ", ", end,
//		")\n");
			std::cerr << route;
		} else {
			LOG(INFO) << "No solution found.";
		}
	}
}  // namespace operations_research

int main(int argc, char **argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	SystemLoader loader;
	if(argc > 1) {
		systems = loader.loadSettlements(argv[1]);
		operations_research::Tsp();
	} else {
		printf("Missing argument: Settlements csv file. Format: System\tPlanet\tSettlement\tX\tY\tZ\n");
		return -1;
	}
	return 0;
}


