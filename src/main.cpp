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
#include <QApplication>
#include <QDebug>
#include "MainWindow.h"
Q_DECLARE_METATYPE(RouteResult);

// namespace operations_research

int main(int argc, char **argv) {
	qDebug() << QT_VERSION_STR;
	qRegisterMetaType<RouteResult>();
	QApplication a(argc, argv);

	MainWindow w;
	w.show();

	return a.exec();
}


