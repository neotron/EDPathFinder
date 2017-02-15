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
//

#include <QDebug>
#include <QCheckBox>
#include <QRadioButton>
#include <QCompleter>
#include <QListView>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "TSPWorker.h"
#include "RouteViewer.h"
#include "EDSMQueryExecutor.h"
#include "QCompressor.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow), _routingPending(false),
                                          _router(new AStarRouter(this)), _pendingLookups(),
                                          _journalWatcher(new JournalWatcher(this)), _settlementDates() {
    _ui->setupUi(this);
    connect(_ui->createRouteButton, SIGNAL(clicked()), this, SLOT(createRoute()));
    connect(_ui->systemName, SIGNAL(editingFinished()), this, SLOT(updateSystemCoordinates()));
    cleanupCheckboxes();
    buildLookupMap();
    loadCompressedData();
    _ui->centralWidget->setEnabled(false);
    connect(_journalWatcher, SIGNAL(onEvent(
                                            const JournalFile &, const Event &)), this, SLOT(handleEvent(
                                                                                                     const JournalFile &, const Event &)));
}

MainWindow::~MainWindow() {
    delete _ui;
    delete _router;
    delete _journalWatcher;
}

void MainWindow::cleanupCheckboxes() {
    for(auto checkbox: findChildren<QCheckBox *>()) {
        connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(updateFilters()));
    }

    for(auto radio: findChildren<QRadioButton *>()) {
        connect(radio, SIGNAL(toggled(bool)), this, SLOT(updateFilters()));
    }

    connect(_ui->filterCommander, SIGNAL(activated(
                                                 const QString &)), this, SLOT(updateFilters()));
}

void MainWindow::buildLookupMap() {
    _flagsLookup["cdt"]     = SettlementFlagsCoreDataTerminal;
    _flagsLookup["jump"]    = SettlementFlagsJumpClimbRequired;
    _flagsLookup["csd"]     = SettlementFlagsClassifiedScanDatabanks;
    _flagsLookup["csf"]     = SettlementFlagsClassifiedScanFragment;
    _flagsLookup["cif"]     = SettlementFlagsCrackedIndustrialFirmware;
    _flagsLookup["dsd"]     = SettlementFlagsDivergentScanData;
    _flagsLookup["mcf"]     = SettlementFlagsModifiedConsumerFirmware;
    _flagsLookup["mef"]     = SettlementFlagsModifiedEmbeddedFirmware;
    _flagsLookup["osk"]     = SettlementFlagsOpenSymmetricKeys;
    _flagsLookup["sfp"]     = SettlementFlagsSecurityFirmwarePatch;
    _flagsLookup["slf"]     = SettlementFlagsSpecializedLegacyFirmware;
    _flagsLookup["tec"]     = SettlementFlagsTaggedEncryptionCodes;
    _flagsLookup["uef"]     = SettlementFlagsUnusualEncryptedFiles;
    _flagsLookup["anarchy"] = SettlementFlagsAnarchy;
}

void MainWindow::routeCalculated(const RouteResult &route) {
    _ui->centralWidget->setEnabled(true);
    if(!route.isValid()) {
        _ui->statusBar->showMessage("No solution found to the given route.", 10000);
        return;
    }
    _ui->statusBar->showMessage("Route calculation completed.", 10000);
    _ui->createRouteButton->setEnabled(true);

    RouteViewer *viewer = new RouteViewer(route);
    viewer->show();
}

void MainWindow::createRoute() {
    if(_filteredSystems.size() > 0) {
        auto systemName   = _ui->systemName->text();
        auto originSystem = _router->getSystemByName(systemName);
        if(!originSystem) {
            // Need to fetch coordinates for origin.
            downloadSystemCoordinates(systemName);
            _routingPending = true;
            return;
        }
        auto routeSize = _ui->systemCountSlider->value();
        updateSystemCoordinateDisplay(*originSystem);
        showMessage(
                QString("Calculating route with %1 systems starting at %2...").arg(routeSize).arg(originSystem->name()),
                0);
        _ui->createRouteButton->setEnabled(false);
        TSPWorker *workerThread(new TSPWorker(_filteredSystems, originSystem, routeSize));
//        workerThread->setRouter(&_router);
        connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
        connect(workerThread, &TSPWorker::taskCompleted, this, &MainWindow::routeCalculated);
        workerThread->start();
        _ui->centralWidget->setEnabled(false);
    } else {
        _ui->statusBar->showMessage("No settlements found that matches your filters.", 10000);
    }
}

void MainWindow::updateFilters() {
    int32              settlementFlags = 0;
    QList<QCheckBox *> checkboxes      = findChildren<QCheckBox *>();
    bool jumpsExcluded = false;

    for(auto &checkbox : checkboxes) {
        if(checkbox->isChecked()) {
            auto flag = _flagsLookup.find(checkbox->objectName());
            if(flag != _flagsLookup.end()) {
                if(*flag == SettlementFlagsJumpClimbRequired) {
                    jumpsExcluded = true;
                } else {
                    settlementFlags |= *flag;
                }
            }
        }
    }

    auto commanders = _settlementDates.keys();
    auto visitedSettlements = QMap<QString, QDateTime>();
    if(commanders.size()) {
        commanders.sort();
        auto selectedCommander = _ui->filterCommander->currentText();
        _ui->filterCommander->clear();
        _ui->filterCommander->addItems(commanders);
        if(!selectedCommander.isEmpty()) {
            _ui->filterCommander->setCurrentText(selectedCommander);
            if(_ui->filterVisited->isChecked()) {
                visitedSettlements = _settlementDates[selectedCommander];
            }
        }
    }

    int32 threatFilter = ThreatLevelUnknown;
    if(_ui->restrictedSec->isChecked()) {
        threatFilter |= ThreatLevelRestrictedLongDistance;
    }
    if(_ui->mediumSec->isChecked()) {
        threatFilter |= ThreatLevelMedium;
    }
    if(_ui->highSec->isChecked()) {
        threatFilter |= ThreatLeveLHigh;
    }
    if(_ui->noSec->isChecked()) {
        threatFilter |= ThreatLevelLow;
    }

    int32 settlementSizes = 0;

    if(_ui->smallSize->isChecked()) {
        settlementSizes |= SettlementSizeSmall;
    }
    if(_ui->mediumSize->isChecked()) {
        settlementSizes |= SettlementSizeMedium;
    }
    if(_ui->largeSize->isChecked()) {
        settlementSizes |= SettlementSizeLarge;
    }


    int32 matches = 0;
    _filteredSystems.clear();
    auto filteredDate         = QDateTime().currentDateTime().addDays(-14); // two weeks.

    for(const auto &system : _systems) {
        PlanetList     matchingPlanets;
        for(const auto &planet: system.planets()) {
            SettlementList matchingSettlements;
            for(auto       settlement: planet.settlements()) {
                auto      settlementKey = makeSettlementKey(system, planet, settlement);
                QDateTime scanDate;
                if(visitedSettlements.contains(settlementKey)) {
                    scanDate = visitedSettlements[settlementKey];
                } else {
                    settlementKey = makeSettlementKey(system, Planet(), settlement);
                    if(visitedSettlements.contains(settlementKey)) {
                        scanDate = visitedSettlements[settlementKey];
                    }
                }
                if(scanDate > filteredDate) {
                    continue;
                }
                if((settlement.flags() & settlementFlags) != settlementFlags) {
                    continue;
                }
                if((settlementSizes & settlement.size()) != settlement.size()) {
                    continue;
                }
                if((threatFilter & settlement.threatLevel()) != settlement.threatLevel()) {
                    continue;
                }
                if(jumpsExcluded &&
                   (settlement.flags() & SettlementFlagsJumpClimbRequired) == SettlementFlagsJumpClimbRequired) {
                    continue;
                }
                matchingSettlements.push_back(settlement);
                ++matches;
            }
            if(matchingSettlements.size()) {
                matchingPlanets.push_back(Planet(planet.name(), matchingSettlements));
            }
        }
        if(matchingPlanets.size()) {
            _filteredSystems.push_back(System(system.name(), matchingPlanets, system.position()));
        }
    }
    auto           numSystems = (int) _filteredSystems.size();
    updateSliderParams(numSystems);

    _matchingSettlementCount = matches;
    _ui->statusBar->showMessage(QString("Filter matches %1 settlements in %2 systems.").arg(_matchingSettlementCount)
                                                                                       .arg(_filteredSystems.size()));
}

void MainWindow::updateSystemCoordinates() {
    const QString systemName(_ui->systemName->text());
    if(!systemName.length()) {
        return;
    }
    auto system = _router->getSystemByName(systemName);
    if(!system) {
        downloadSystemCoordinates(systemName);
    } else {
        updateSystemCoordinateDisplay(*system);
    }
}

void MainWindow::downloadSystemCoordinates(const QString &systemName) {
    if(_pendingLookups.contains(systemName)) {
        return;
    }
    _pendingLookups << systemName;
    _ui->statusBar->showMessage(QString("Fetching system coordinates from EDSM..."), 10000);
    auto executor = EDSMQueryExecutor::systemCoordinateRequest(systemName);
    connect(executor, &QThread::finished, executor, &QObject::deleteLater);
    connect(executor, &EDSMQueryExecutor::coordinatesReceived, this, &MainWindow::systemCoordinatesReceived);
    connect(executor, &EDSMQueryExecutor::coordinateRequestFailed, this, &MainWindow::systemCoordinatesRequestFailed);
    executor->start();
    _ui->x->setText("-");
    _ui->y->setText("-");
    _ui->z->setText("-");
    _ui->systemName->setEnabled(false);
    _ui->createRouteButton->setEnabled(false);
}

void MainWindow::systemCoordinatesRequestFailed() {
    auto systemName = _ui->systemName->text();
    showMessage(QString("Unknown origin system: %1").arg(systemName));
    _pendingLookups.remove(systemName);
    _ui->systemName->setEnabled(true);
    _routingPending = false;
}


void MainWindow::systemCoordinatesReceived(const System &system) {
    updateSystemCoordinateDisplay(system);
    _ui->systemName->setEnabled(true);
    auto systemName = QString(system.name());
    _pendingLookups.remove(systemName);
    _ui->systemName->setText(systemName);
    _router->addSystem(system);
    _router->sortSystemList();
    showMessage(QString("Found coordinates for system: %1").arg(_ui->systemName->text()), 4000);
    if(_routingPending) {
        _routingPending = false;
        createRoute();
    }
}

void MainWindow::updateSystemCoordinateDisplay(const System &system) const {
    _ui->x->setText(QString::number(system.x()));
    _ui->y->setText(QString::number(system.y()));
    _ui->z->setText(QString::number(system.z()));
    _ui->createRouteButton->setEnabled(!_routingPending);
    _ui->systemName->setText(system.name());
}

void MainWindow::loadCompressedData() {
    showMessage("Loading known systems...", 0);
    QFile file(":/systems.json.gz");
    if(!file.open(QIODevice::ReadOnly)) { return; }
    QByteArray   blob       = file.readAll();
    auto         compressor = new QCompressor(blob);
    SystemLoader *loader    = new SystemLoader(_router);

    connect(compressor, &QThread::finished, compressor, &QObject::deleteLater);
    connect(loader, &QThread::finished, compressor, &QObject::deleteLater);
    connect(loader, SIGNAL(systemsLoaded(
                                   const SystemList &)), this, SLOT(systemsLoaded(
                                                                            const SystemList &)));
    connect(compressor, SIGNAL(complete(
                                       const QByteArray &)), loader, SLOT(dataDecompressed(
                                                                                  const QByteArray &)));

    compressor->start();
}

void MainWindow::systemsLoaded(const SystemList &systems) {
    _systems = std::move(systems);
    _ui->systemCountSlider->setMinimum(1);
    _ui->systemCountSlider->setSingleStep(1);
    updateSliderParams(_systems.size());
    updateFilters();
    _ui->centralWidget->setEnabled(true);

    QCompleter *completer = new QCompleter(_router, this);
    completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    QListView *popup = (QListView *) completer->popup();
    popup->setBatchSize(10);
    popup->setLayoutMode(QListView::Batched);
    connect(completer, SIGNAL(activated(
                                      const QString &)), this, SLOT(updateSystemCoordinates()));
    _ui->systemName->setCompleter(completer);

    // Start monitoring.
    auto newerThanDate = QDateTime::currentDateTime()
            .addDays(-16); // Things changed in the last 16 days  - we need 14 days for expire.
#ifdef Q_OS_OSX
    _journalWatcher
            ->watchDirectory(QDir::homePath() + "/Library/Application Support/Frontier Developments/Elite Dangerous/",
                             newerThanDate);
#else
    _journalWatcher->watchDirectory(QDir::homePath()+"/Saved Games/Frontier Developments/Elite Dangerous/", newerThanDate);
#endif
}

void MainWindow::showMessage(const QString &message, int timeout) {
    _ui->statusBar->showMessage(message, timeout);
}

void MainWindow::updateSliderParams(int size) {
    auto max = qMin(100, size);
    _ui->systemCountSlider->setMaximum(max);
    _ui->systemCountSlider->setValue(max);
    _ui->systemCountLabel->setText(QString::number(max));
}

void MainWindow::handleEvent(const JournalFile &journal, const Event &event) {
    if(event.type() == EventTypeDatalinkScan) {
        auto settlementName = journal.settlement();
        if(settlementName.isEmpty()) {
            return;
        }
        if(settlementName.endsWith("+")) {
            auto parts = settlementName.split(" ");
            parts.removeLast();
            settlementName = parts.join(" ");
        }

        auto settlementKey      = makeSettlementKey(journal.system(), journal.body(), settlementName);
        auto shortSettlementKey = makeSettlementKey(journal.system(), "", settlementName);
        updateSettlementScanDate(journal.commander(), settlementKey, event.timestamp());
        if(settlementKey != shortSettlementKey) {
            updateSettlementScanDate(journal.commander(), shortSettlementKey, event.timestamp());
        }
        updateFilters();
    }
}

void MainWindow::updateSettlementScanDate(const QString &commander, const QString &key, const QDateTime &timestamp) {
    if(_settlementDates[commander][key] < timestamp) {
        _settlementDates[commander][key] = timestamp;
    }
}

const QString MainWindow::makeSettlementKey(const System &system, const Planet &planet,
                                            const Settlement &settlement) const {
    return QString("%1:%2:%3").arg(system.name()).arg(planet.name()).arg(settlement.name()).toLower();
}

const QString MainWindow::makeSettlementKey(const QString &system, const QString &planet,
                                            const QString &settlement) const {
    // Planets from log comes with a prefix of the star, get rid of it.
    auto parts = planet.split(system + " ");
    if(parts.size() > 1) {
        parts.removeFirst();
    }
    return QString("%1:%2:%3").arg(system).arg(parts.join("")).arg(settlement).toLower();
}






















