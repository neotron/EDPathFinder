#include <QRadioButton>
#include "ValueRouter.h"
#include "ValuablePlanetRouteViewer.h"


ValueRouter::~ValueRouter() {
}

ValueRouter::ValueRouter(QWidget *parent, AStarRouter *router, SystemList *systems)
        : AbstractBaseWindow(parent, router, systems) {
    _systemsOnly = true;
}

void ValueRouter::updateFilters() {

    int8 flags = ValuableBodyFlagsNone;
    if(_ui->elw->isChecked()) { flags |= ValuableBodyFlagsEW; }
    if(_ui->ww->isChecked())  { flags |= ValuableBodyFlagsWW; }
    if(_ui->wwt->isChecked()) { flags |= ValuableBodyFlagsWT; }
    if(_ui->aw->isChecked())  { flags |= ValuableBodyFlagsAW; }
    if(_ui->tf->isChecked())  { flags |= ValuableBodyFlagsTF; }

    _filteredSystems.clear();
    auto filteredDate = QDateTime().currentDateTime().addDays(-14); // two weeks.

    for(const auto &system : _router->systems()) {
        if((system.valueFlags() & flags) != ValuableBodyFlagsNone) {
            _filteredSystems.append(system);
        }
    }
    qDebug() << "Filtering done: " << _filteredSystems.count();
}

void ValueRouter::routeCalculated(const RouteResult &route) {
    AbstractBaseWindow::routeCalculated(route);
    auto viewer = new ValuablePlanetRouteViewer(route);
    viewer->show();
}
