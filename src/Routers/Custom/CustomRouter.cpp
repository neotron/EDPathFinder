#include <QListView>
#include <QCompleter>
#include <QFileDialog>
#include <QMessageBox>
#include "RouteProgressAnnouncer.h"
#include "Settings.h"
#include "WindowMenu.h"
#include "CustomRouter.h"
#include "RouteViewer.h"
#include "EDSMQueryExecutor.h"

CustomRouter::CustomRouter(QWidget *parent, AStarRouter *router, const SystemList &systems)
        : QMainWindow(parent), _ui(new Ui::CustomRouter()), _scanner(this), _router(router), _systems(systems),
          _currentModel(nullptr), _routingPending(false), _customStops(), _progressAnnouncer(nullptr),
          _presetsManager(new PresetsManager(this)), _resolvers() {
    _ui->setupUi(this);
    _ui->menuBar->addMenu(new WindowMenu(this, _ui->menuBar));

    // Adding new systems
    auto systemResolver = new SystemEntryCoordinateResolver(this, _router, _ui->customSystem);
    connect(systemResolver, SIGNAL(systemLookupInitiated(const QString &)), this, SLOT(onSystemLookupInitiated(const QString &)));
    connect(systemResolver, SIGNAL(systemLookupFailed(const QString &)), this, SLOT(onSystemCoordinatesRequestFailed(const QString &)));
    connect(systemResolver, SIGNAL(systemLookupCompleted(const System &)), this, SLOT(onSystemCoordinatesReceivedCustom(const System &)));
    _resolvers.push_back(systemResolver);

    systemResolver = new SystemEntryCoordinateResolver(this, _router, _ui->originSystem, _ui->ox, _ui->oy, _ui->oz);
    connect(systemResolver, SIGNAL(systemLookupInitiated(const QString &)), this, SLOT(onSystemLookupInitiated(const QString &)));
    connect(systemResolver, SIGNAL(systemLookupFailed(const QString &)), this, SLOT(onSystemCoordinatesRequestFailed(const QString &)));
    connect(systemResolver, SIGNAL(systemLookupCompleted(const System &)), this, SLOT(onSystemCoordinatesReceived(const System &)));
    _resolvers.push_back(systemResolver);

    systemResolver = new SystemEntryCoordinateResolver(this, _router, _ui->destinationSystem, _ui->dx, _ui->dy, _ui->dz);
    connect(systemResolver, SIGNAL(systemLookupInitiated(const QString &)), this, SLOT(onSystemLookupInitiated(const QString &)));
    connect(systemResolver, SIGNAL(systemLookupFailed(const QString &)), this, SLOT(onSystemCoordinatesRequestFailed(const QString &)));
    connect(systemResolver, SIGNAL(systemLookupCompleted(const System &)), this, SLOT(onSystemCoordinatesReceived(const System &)));
    _resolvers.push_back(systemResolver);
    
    refreshMissions();
    auto table = _ui->tableView;
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setSelectionMode(QTableView::SingleSelection);
    setAttribute(Qt::WA_DeleteOnClose, true);

    _ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _presetsManager->addPresetsTo(_ui->menuBar);
    connect(_presetsManager, &PresetsManager::didSelectEntries, [=](const PresetEntryList &list) {
        for(const auto &preset: list) {
            _customStops << preset;
        }
        updateMissionTable();
    });
}

void CustomRouter::copySelectedItem() {
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

CustomRouter::~CustomRouter() {
    delete _ui;
}

void CustomRouter::refreshMissions() {
    _ui->optimizeButton->setEnabled(false);
    _scanner.scanJournals();

    const auto comboBox = _ui->commanders;
    const auto selected = comboBox->currentText();
    comboBox->clear();
    comboBox->addItems(_scanner.commanders());
    if(selected.isEmpty()) {
        changeCommander(_scanner.recentCommander());
    } else {
        comboBox->setCurrentText(selected);
    }
    updateMissionTable();
}

void CustomRouter::updateMissionTable() {
    auto cmdr = _ui->commanders->currentText();

    delete _progressAnnouncer;
    _progressAnnouncer = nullptr;

    if(cmdr.isEmpty() && _customStops.empty()) {
        _ui->tableView->setModel(nullptr);
        _currentModel = nullptr;
        return;
    }

    auto missionList = _scanner.commanderMission(cmdr);
    if(!_ui->includeMissionSystems->isChecked()) {
        missionList.clear();
    }
    PresetEntryList stops;
    for(auto &stop: _customStops) {
        stops.push_back(stop);
    }
    for(auto &mission: missionList) {
        PresetEntry entry(mission._destination);
        entry.setType("Mission");
        entry.setShortDescription("Starts in "+mission._origin);
        stops.push_back(entry);
    }
    _currentModel = new PresetsTableModel(this, stops);
    _ui->optimizeButton->setEnabled(true);
    refreshTableView(_currentModel);
    if(_ui->originSystem->text().isEmpty()) {
        _ui->originSystem->setText(_scanner.commanderSystem(cmdr));
    }
}

void CustomRouter::refreshTableView(QAbstractItemModel *model) const {
    auto table = _ui->tableView;
    table->setModel(model);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    QHeaderView *horizontalHeader = table->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader->setStretchLastSection(true);
    connect(table->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this,SLOT(copySelectedItem()));
}

void CustomRouter::optimizeRoute() {
    if(!(_currentModel || !_customStops.empty())) {
        return;
    }
    _ui->optimizeButton->setEnabled(false);
    SystemList routeSystems;
    auto       cmdr       = _ui->commanders->currentText();
    auto       systemName = _ui->originSystem->text();
    System     *originSystem(nullptr);
    if(!systemName.isEmpty()) {
        originSystem = _router->findSystemByName(systemName);
        if(!originSystem) {
            // Need to fetch coordinates for origin.
            _resolvers.first()->resolve(systemName);
            _routingPending = true;
            return;
        }
    }

    // Add systems from the custom route, and resolve any unresolved systems.
    for(const auto &stop: _currentModel->stops()) {
        auto missionSystem = _router->findSystemByName(stop.systemName());
        if(!missionSystem) {
            _resolvers.first()->resolve(stop.systemName());
            _routingPending = true;
            return;
        }
        auto system = System(missionSystem->name(), PlanetList(), missionSystem->position());
        system.setPresetEntry(stop);
        routeSystems.push_back(system);
    }

    _ui->statusbar->showMessage(QString("Resolving route for %1 systems...").arg(routeSystems.size()), 10000);

    // Create new system resolver.
    const auto tspWorker = new TSPWorker(routeSystems, originSystem, routeSystems.size());
    tspWorker->setSystemsOnly(true);
    tspWorker->setIsPresets(true);

    // Set optional destination system.
    auto destinationSystemName = _ui->destinationSystem->text();
    if(!destinationSystemName.isEmpty()) {
        auto destinationSystem = _router->findSystemByName(destinationSystemName);
        tspWorker->setDestination(destinationSystem);
    }


    TSPWorker *workerThread(tspWorker);
    // workerThread->setRouter(_router);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
    connect(workerThread, &TSPWorker::taskCompleted, this, &CustomRouter::routeCalculated);
    workerThread->start();
}

void CustomRouter::routeCalculated(const RouteResult &route) {
    if(!route.isValid()) {
        _ui->statusbar->showMessage("No solution found to the given route.", 10000);
        return;
    }
    _ui->statusbar->showMessage("Route calculation completed.", 10000);
    _ui->optimizeButton->setEnabled(true);
    auto model = new RouteTableModel(this, route);
    model->setResultType(RouteTableModel::ResultTypePresets);
    refreshTableView(model);
    _progressAnnouncer = new RouteProgressAnnouncer(this, model, _ui->tableView);
}

void CustomRouter::onSystemCoordinatesRequestFailed(const QString &systemName) {
    showMessage(QString("Coordinate lookup failed for %1").arg(systemName));
    _routingPending = false;

    _ui->centralwidget->setEnabled(resolversComplete());
}

void CustomRouter::onSystemCoordinatesReceivedCustom(const System &system) {
    if(!_routingPending) {
        _customStops << PresetEntry(system.name());
        updateMissionTable();
    }
    QTimer::singleShot(0, _ui->customSystem, SLOT(setFocus()));
    onSystemCoordinatesReceived(system);
}

void CustomRouter::onSystemCoordinatesReceived(const System &system) {
    showMessage(QString("Found coordinates for system: %1").arg(system.name()), 4000);
    if(resolversComplete()) {
        _ui->centralwidget->setEnabled(true);

        if(_routingPending) {
            _routingPending = false;
            optimizeRoute();
        }
        _ui->customSystem->setText("");
    }
}


void CustomRouter::showMessage(const QString &message, int timeout) {
    _ui->statusbar->showMessage(message, timeout);
}

void CustomRouter::clearCustom() {
    _customStops.clear();
    updateMissionTable();
}

void CustomRouter::onSystemLookupInitiated(const QString &system) {
    showMessage(QString("Looking up coordinates for system: %1").arg(system), 4000);
    _ui->centralwidget->setEnabled(false);
}

void CustomRouter::exportAsCSV() {
    RouteTableModel::exportTableViewToCSV(_ui->tableView);
}

void CustomRouter::addStop() {
    _resolvers.first()->resolve(_ui->customSystem->text());
}

void CustomRouter::exportAsTabNewline() {
    RouteTableModel::exportTableViewToTabNewline(_ui->tableView);
}

void CustomRouter::importSystems() {
    QString filters("Text files (*.txt);;All files (*.*)");
    QString defaultFilter("Text files (*.txt)");
    QString fileName = QFileDialog::getOpenFileName(this, "Import file", Settings::restoreSavePath(),
                                                    filters, &defaultFilter);
    if(fileName.isEmpty()) {
        return;
    }

    Settings::saveSavePath(fileName);

    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)) {
        QMessageBox messageBox;
        messageBox.critical(0,"Failed to open file","The file provided couldn't be opened for reading. Try another file");
        messageBox.setFixedSize(300,200);
        messageBox.show();
        return;
    }

    QTextStream stream(&file);
    bool appended = false;
    for(auto line = stream.readLine(); !line.isNull(); line = stream.readLine()) {
        auto components = line.split("\t");
        if(!components.isEmpty()) {
            const QString &trimmed = components[0].trimmed();
            if (!trimmed.isEmpty() && trimmed != "System") {
                auto entry = PresetEntry(trimmed);
                entry.setType("Imported");
                _customStops << entry;
                appended = true;
            }
        }
    }
    if(appended) {
        updateMissionTable();
    }
}

void CustomRouter::changeCommander(const QString &cmdr) {
    auto system = _scanner.commanderSystem(cmdr);
    _ui->originSystem->setText(system);
    _resolvers[1]->resolve(system);
}

bool CustomRouter::resolversComplete() {
    for(const auto &resolver: _resolvers) {
        if(!resolver->isComplete()) {
            return false;
        }
    }
    return true;
}
