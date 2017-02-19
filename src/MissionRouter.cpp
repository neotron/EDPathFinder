#include <QListView>
#include <QCompleter>
#include "MissionRouter.h"
#include "ui_MissionRouter.h"
#include "RouteViewer.h"
#include "EDSMQueryExecutor.h"

MissionRouter::MissionRouter(QWidget *parent, AStarRouter *router, const SystemList &systems)
        : QMainWindow(parent), _ui(new Ui::MissionRouter), _scanner(this), _router(router), _systems(systems),
          _currentModel(nullptr), _pendingLookups(), _routingPending(false), _customStops() {

    _ui->setupUi(this);
    refreshMissions();
    auto table = _ui->tableView;
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setSelectionMode(QTableView::SingleSelection);

    QCompleter *completer = new QCompleter(_router, this);
    completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    QListView *popup = (QListView *) completer->popup();
    popup->setBatchSize(10);
    popup->setLayoutMode(QListView::Batched);
    connect(completer, SIGNAL(activated(
                                      const QString &)), this, SLOT(addStop()));
    _ui->customSystem->setCompleter(completer);
}

MissionRouter::~MissionRouter() {
    delete _ui;
}

void MissionRouter::refreshMissions() {
    _ui->optimizeButton->setEnabled(false);
    _scanner.scanJournals();

    const auto comboBox = _ui->commanders;
    const auto selected = comboBox->currentText();
    comboBox->clear();
    for(auto cmdr: _scanner.commanders()) {
        auto missions = _scanner.commanderMission(cmdr);
        if(missions.size()) {
            comboBox->addItem(cmdr);
        }
    }
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
}

void MissionRouter::optimizeRoute() {
    if(!_currentModel || !_customStops.size()) {
        return;
    }
    _ui->optimizeButton->setEnabled(false);
    SystemList routeSystems;
    auto       cmdr       = _ui->commanders->currentText();
    auto       systemName = _scanner.commanderSystem(cmdr);
    System     *originSystem(nullptr);
    if(!systemName.isEmpty()) {
        originSystem = _router->getSystemByName(systemName);
        if(!originSystem) {
            // Need to fetch coordinates for origin.
            downloadSystemCoordinates(systemName);
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
        auto missionSystem = _router->getSystemByName(system._destination);
        if(!missionSystem) {
            downloadSystemCoordinates(system._destination);
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
    model->setSystemsOnly(true);
    refreshTableView(model);
}

void MissionRouter::downloadSystemCoordinates(const QString &systemName) {
    if(_pendingLookups.contains(systemName)) {
        return;
    }
    _pendingLookups << systemName;
    _ui->statusbar->showMessage(QString("Fetching system coordinates from EDSM..."), 10000);
    auto executor = EDSMQueryExecutor::systemCoordinateRequest(systemName);
    connect(executor, &QThread::finished, executor, &QObject::deleteLater);
    connect(executor, &EDSMQueryExecutor::coordinatesReceived, this, &MissionRouter::systemCoordinatesReceived);
    connect(executor, &EDSMQueryExecutor::coordinateRequestFailed, this,
            &MissionRouter::systemCoordinatesRequestFailed);
    executor->start();
}


void MissionRouter::systemCoordinatesRequestFailed(const QString &systemName) {
    showMessage(QString("Coordinate lookup failed for %1").arg(systemName));
    _pendingLookups.remove(systemName);
    _routingPending = false;
}

void MissionRouter::systemCoordinatesReceived(const System &system) {
    auto systemName = QString(system.name());
    _pendingLookups.remove(systemName);
    _router->addSystem(system);
    _router->sortSystemList();
    showMessage(QString("Found coordinates for system: %1").arg(systemName), 4000);
    if(_routingPending && !_pendingLookups.size()) {
        _routingPending = false;
        optimizeRoute();
    }
}


void MissionRouter::showMessage(const QString &message, int timeout) {
    _ui->statusbar->showMessage(message, timeout);
}

void MissionRouter::addStop() {
    const QString system = _ui->customSystem->text();
    if(!system.isEmpty()) {
        _customStops << system;
        updateMissionTable();
    }
}

void MissionRouter::clearCustom() {
    _customStops.clear();
    updateMissionTable();
}








