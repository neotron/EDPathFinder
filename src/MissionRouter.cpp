#include <QListView>
#include <QCompleter>
#include "MissionRouter.h"
#include "RouteViewer.h"
#include "EDSMQueryExecutor.h"

MissionRouter::MissionRouter(QWidget *parent, AStarRouter *router, const SystemList &systems)
        : QMainWindow(parent), _ui(new Ui::MissionRouter), _scanner(this), _router(router), _systems(systems),
          _currentModel(nullptr), _routingPending(false), _customStops(), _systemResolver(nullptr) {
    _ui->setupUi(this);
    refreshMissions();
    auto table = _ui->tableView;
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setSelectionMode(QTableView::SingleSelection);
    setAttribute(Qt::WA_DeleteOnClose, true);

    _systemResolver = new SystemEntryCoordinateResolver(this, _router, _ui->customSystem);

    connect(_systemResolver, SIGNAL(systemLookupInitiated(const QString &)), this, SLOT(onSystemLookupInitiated(const QString &)));
    connect(_systemResolver, SIGNAL(systemLookupFailed(const QString &)), this, SLOT(onSystemCoordinatesRequestFailed(const QString &)));
    connect(_systemResolver, SIGNAL(systemLookupCompleted(const System &)), this, SLOT(onSystemCoordinatesReceived(const System &)));
}

void MissionRouter::copySelectedItem() {
    auto routeModel = dynamic_cast<RouteTableModel *>(_ui->tableView->model());
    if(!routeModel) {
        return;
    }
    auto row   = static_cast<size_t>(_ui->tableView->selectionModel()->currentIndex().row());
    auto route = routeModel->result().route();
    if(row >= route.size()) {
        return;
    }
    auto name = route[row][0];
    QApplication::clipboard()->setText(name);
    _ui->statusbar->showMessage(QString("Copied system name `%1' to the system clipboard.").arg(name));
}

MissionRouter::~MissionRouter() {
    delete _ui;
    delete _systemResolver;
}

void MissionRouter::refreshMissions() {
    _ui->optimizeButton->setEnabled(false);
    _scanner.scanJournals();

    const auto comboBox = _ui->commanders;
    const auto selected = comboBox->currentText();
    comboBox->clear();
    comboBox->addItems(_scanner.commanders());
    comboBox->setCurrentText(selected);
    updateMissionTable();
}

void MissionRouter::updateMissionTable() {
    auto cmdr = _ui->commanders->currentText();

    if(cmdr.isEmpty() && !_customStops.size()) {
        _ui->tableView->setModel(nullptr);
        _currentModel = nullptr;
        return;
    }

    auto missionList = _scanner.commanderMission(cmdr);
    if(!_ui->includeMissionSystems->isChecked()) {
        missionList.clear();
    }
    for(auto system: _customStops) {
        missionList << Mission(system, "Custom");
    }
    _currentModel = new MissionTableModel(this, missionList);
    _ui->optimizeButton->setEnabled(true);
    refreshTableView(_currentModel);
}

void MissionRouter::refreshTableView(QAbstractItemModel *model) const {
    auto table = _ui->tableView;
    table->setModel(model);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    QHeaderView *horizontalHeader = table->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader->setStretchLastSection(true);
    connect(table->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this,SLOT(copySelectedItem()));

}

void MissionRouter::optimizeRoute() {
    if(!(_currentModel || _customStops.size())) {
        return;
    }
    _ui->optimizeButton->setEnabled(false);
    SystemList routeSystems;
    auto       cmdr       = _ui->commanders->currentText();
    auto       systemName = _scanner.commanderSystem(cmdr);
    System     *originSystem(nullptr);
    if(!systemName.isEmpty()) {
        originSystem = _router->findSystemByName(systemName);
        if(!originSystem) {
            // Need to fetch coordinates for origin.
            _systemResolver->resolve(systemName);
            _routingPending = true;
            return;
        }
    }
    QSet<QString> visitedSystems;
    visitedSystems.insert(systemName);
    for(auto system: _currentModel->missions()) {
        if(visitedSystems.contains(system._destination)) {
            continue;
        }
        visitedSystems.insert(system._destination);
        auto missionSystem = _router->findSystemByName(system._destination);
        if(!missionSystem) {
            _systemResolver->resolve(system._destination);
            _routingPending = true;
            return;
        }
        routeSystems.push_back(System(missionSystem->name(), PlanetList(), missionSystem->position()));
    }
    routeSystems.push_back(*originSystem);

    const auto tspWorker = new TSPWorker(routeSystems, originSystem, routeSystems.size());
    tspWorker->setSystemsOnly(true);
    TSPWorker *workerThread(tspWorker);
    // workerThread->setRouter(_router);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
    connect(workerThread, &TSPWorker::taskCompleted, this, &MissionRouter::routeCalculated);
    workerThread->start();
    //_ui->centralWidget->setEnabled(false);
}

void MissionRouter::routeCalculated(const RouteResult &route) {
    if(!route.isValid()) {
        _ui->statusbar->showMessage("No solution found to the given route.", 10000);
        return;
    }
    _ui->statusbar->showMessage("Route calculation completed.", 10000);
    _ui->optimizeButton->setEnabled(true);
    RouteTableModel *model = new RouteTableModel(this, route);
    model->setResultType(RouteTableModel::ResultTypeSystemsOnly);
    refreshTableView(model);
}

void MissionRouter::onSystemCoordinatesRequestFailed(const QString &systemName) {
    showMessage(QString("Coordinate lookup failed for %1").arg(systemName));
    _routingPending = false;
    _ui->centralwidget->setEnabled(_systemResolver->isComplete());
}

void MissionRouter::onSystemCoordinatesReceived(const System &system) {
    showMessage(QString("Found coordinates for system: %1").arg(system.name()), 4000);
    if(!_routingPending) {
        _customStops << system.name();
        updateMissionTable();
    }
    if(_systemResolver->isComplete()) {
        _ui->centralwidget->setEnabled(true);

        if(_routingPending) {
            _routingPending = false;
            optimizeRoute();
        }
        _ui->customSystem->setText("");
    }
}


void MissionRouter::showMessage(const QString &message, int timeout) {
    _ui->statusbar->showMessage(message, timeout);
}

void MissionRouter::clearCustom() {
    _customStops.clear();
    updateMissionTable();
}

void MissionRouter::onSystemLookupInitiated(const QString &system) {
    showMessage(QString("Looking up coordinates for system: %1").arg(system), 4000);
    _ui->centralwidget->setEnabled(false);
}

