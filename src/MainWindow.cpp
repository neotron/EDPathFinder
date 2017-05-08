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
#include <QCompleter>
#include <QListView>
#include "MainWindow.h"
#include "QCompressor.h"
#include "MissionRouter.h"
#include "ValueRouter.h"

MainWindow::MainWindow(QWidget *parent)
        : AbstractBaseWindow(parent, new AStarRouter(), new SystemList()),
          _journalWatcher(new JournalWatcher(this)), _settlementDates() {
    buildLookupMap();
    loadCompressedData();
    _ui->centralWidget->setEnabled(false);
    _ui->menuBar->setEnabled(false);
    connect(_journalWatcher, SIGNAL(onEvent(const JournalFile &, const Event &)),
            this, SLOT(handleEvent( const JournalFile &, const Event &)));
    _ui->filterCommander->setInsertPolicy(QComboBox::InsertAlphabetically);
    _ui->distanceSlider->setMaximum(10000);
    _ui->distanceSlider->setValue(10000);
}

MainWindow::~MainWindow() {
    delete _router;
    delete _journalWatcher;
    delete _systemResolver;
}

void MainWindow::buildLookupMap() {
    _flagsLookup["cdt"] = SettlementFlagsCoreDataTerminal;
    _flagsLookup["jump"] = SettlementFlagsJumpClimbRequired;
    _flagsLookup["csd"] = SettlementFlagsClassifiedScanDatabanks;
    _flagsLookup["csf"] = SettlementFlagsClassifiedScanFragment;
    _flagsLookup["cif"] = SettlementFlagsCrackedIndustrialFirmware;
    _flagsLookup["dsd"] = SettlementFlagsDivergentScanData;
    _flagsLookup["mcf"] = SettlementFlagsModifiedConsumerFirmware;
    _flagsLookup["mef"] = SettlementFlagsModifiedEmbeddedFirmware;
    _flagsLookup["osk"] = SettlementFlagsOpenSymmetricKeys;
    _flagsLookup["sfp"] = SettlementFlagsSecurityFirmwarePatch;
    _flagsLookup["slf"] = SettlementFlagsSpecializedLegacyFirmware;
    _flagsLookup["tec"] = SettlementFlagsTaggedEncryptionCodes;
    _flagsLookup["uef"] = SettlementFlagsUnusualEncryptedFiles;
    _flagsLookup["anarchy"] = SettlementFlagsAnarchy;
}

void MainWindow::routeCalculated(const RouteResult &route) {
    AbstractBaseWindow::routeCalculated(route);
    RouteViewer *viewer = new RouteViewer(route);
    viewer->show();
}

void MainWindow::updateFilters() {
    int32 settlementFlags = 0;
    QList<QCheckBox *> checkboxes = findChildren<QCheckBox *>();
    bool jumpsExcluded = false;

    int maxDistance = distanceSliderValue();
    const auto distanceFilterChecked = _ui->distanceCheckbox->isChecked();
    _ui->distanceSlider->setEnabled(distanceFilterChecked);
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

    auto selectedCommander = _ui->filterCommander->currentText();
    auto commanders = _settlementDates.keys();
    auto visitedSettlements = QMap<QString, QDateTime>();
    if(commanders.size()) {
        commanders.sort();
        for(auto commanderName: commanders) {
            if(_ui->filterCommander->findText(commanderName) < 0) {
                _ui->filterCommander->addItem(commanderName);
            }
        }
        if(!selectedCommander.isEmpty()) {
            _ui->filterCommander->setCurrentText(selectedCommander);
            if(_ui->filterVisited->isChecked()) {
                visitedSettlements = _settlementDates[selectedCommander];
            }
        }
    }
    updateSystemForCommander(selectedCommander);

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
    auto filteredDate = QDateTime().currentDateTime().addDays(-14); // two weeks.

    for(const auto &system : *_systems) {
        PlanetList matchingPlanets;
        for(const auto &planet: system.planets()) {
            if(_ui->unknownDistance->isChecked() && !planet.distance()) {
                continue;
            }
            if(distanceFilterChecked && maxDistance && planet.distance() && planet.distance() > maxDistance) {
                continue;
            }
            SettlementList matchingSettlements;
            for(auto settlement: planet.settlements()) {
                auto settlementKey = makeSettlementKey(system, planet, settlement);
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
                matchingPlanets.push_back(Planet(planet.name(), planet.distance(), matchingSettlements));
            }
        }
        if(matchingPlanets.size()) {
            _filteredSystems.push_back(System(system.name(), matchingPlanets, system.position()));
        }
    }
    auto numSystems = (int) _filteredSystems.size();
    updateSliderParams(numSystems);

    _matchingSettlementCount = matches;
    _ui->statusBar->showMessage(QString("Filter matches %1 settlements in %2 systems.").arg(_matchingSettlementCount)
                                                                                       .arg(_filteredSystems.size()));
}

void MainWindow::loadCompressedData() {
    showMessage("Loading known systems...", 0);
    QFile file(":/systems.txt.gz");
    if(!file.open(QIODevice::ReadOnly)) { return; }
    auto compressor = new QCompressor(file.readAll());

    QFile file2(":/valuable-systems.csv.gz");
    if(!file2.open(QIODevice::ReadOnly)) { return; }
    auto compressor2 = new QCompressor(file2.readAll());


    SystemLoader *loader = new SystemLoader(_router);

    connect(compressor, &QThread::finished, compressor, &QObject::deleteLater);
    connect(compressor2, &QThread::finished, compressor2, &QObject::deleteLater);
    connect(loader, &QThread::finished, compressor, &QObject::deleteLater);
    connect(loader, SIGNAL(progress(int)), this, SLOT(systemLoadProgress(int)));
    connect(loader, SIGNAL(sortingSystems()), this, SLOT(systemSortingProgress()));
    connect(loader, SIGNAL(systemsLoaded(const SystemList &)), this, SLOT(systemsLoaded(const SystemList &)));
    connect(compressor, SIGNAL(complete(const QByteArray &)), loader, SLOT(dataDecompressed(const QByteArray &)));
    connect(compressor2, SIGNAL(complete(const QByteArray &)), loader, SLOT(valuableSystemDataDecompressed(const QByteArray &)));

    compressor->start();
    compressor2->start();
}

void MainWindow::systemsLoaded(const SystemList &systems) {
    _systems->clear();
    _systems->append(systems);
    _ui->systemCountSlider->setMinimum(1);
    _ui->systemCountSlider->setSingleStep(1);
    updateSliderParams(_systems->size());
    updateFilters();
    _ui->centralWidget->setEnabled(true);
    _ui->menuBar->setEnabled(true);
    // Start monitoring.  Things changed in the last 16 days  - we need 14 days for expire.
    auto newerThanDate = QDateTime::currentDateTime().addDays(-16);
    _loading = true;
    _journalWatcher->watchDirectory(journalDirectory(), newerThanDate);
    _loading = false;
    updateCommanderAndSystem();
}

const QString MainWindow::journalDirectory() {
    QString journalPath;
#ifdef Q_OS_OSX
    journalPath = QDir::homePath() + "/Library/Application Support/Frontier Developments/Elite Dangerous/";
#else
    journalPath = QDir::homePath()+"/Saved Games/Frontier Developments/Elite Dangerous/";
#endif
    return journalPath;
}

void MainWindow::updateSliderParams(int size) {
    auto max = qMin(100, size);
    _ui->systemCountSlider->setMaximum(max);
    _ui->systemCountSlider->setValue(max);
    _ui->systemCountLabel->setText(QString::number(max));
}

int MainWindow::distanceSliderValue() const {
    auto value = _ui->distanceSlider->value();
    value = std::min(100 + (int) (pow(2, log(value) * 2.5)) / 100, 85000);
    _ui->distanceLabel->setText(QString("%1 ls").arg(value));
    return value;
}

void MainWindow::handleEvent(const JournalFile &journal, const Event &event) {
    //sqDebug() << "Got event"<<event.obj();
    const QString &commander = journal.commander();
    if(commander.isEmpty()) {
        return;
    }
    switch(event.type()) {
    case EventTypeDatalinkScan: {
        auto settlementName = journal.settlement();
        if(settlementName.isEmpty()) {
            return;
        }
        if(settlementName.endsWith("+")) {
            auto parts = settlementName.split(" ");
            parts.removeLast();
            settlementName = parts.join(" ");
        }

        auto settlementKey = makeSettlementKey(journal.system(), journal.body(), settlementName);
        auto shortSettlementKey = makeSettlementKey(journal.system(), "", settlementName);
        updateSettlementScanDate(commander, settlementKey, event.timestamp());
        if(settlementKey != shortSettlementKey) {
            updateSettlementScanDate(commander, shortSettlementKey, event.timestamp());
        }
        updateFilters();
        break;
    }
    case EventTypeLocation:
    case EventTypeFSDJump:
        if(updateCommanderInfo(journal, event, commander)  && !_loading) {
            updateCommanderAndSystem();
        }
        break;
    default:
        break; // Be quiet
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

void MainWindow::systemLoadProgress(int progress) {
    showMessage(QString("Loading known systems (%1%)...").arg(progress), 0);
}

void MainWindow::systemSortingProgress() {
    showMessage("Loading known systems (100%). Sorting...", 0);
}

void MainWindow::updateSystemForCommander(const QString &commander) {
    if(_loading) {
        return;
    }
    _ui->commanderFilterGroup->setEnabled(true);

    CommanderInfo info = _commanderInformation[commander];
    if(_ui->systemName->text() != info._system) {
        _ui->systemName->setText(info._system);
        _systemResolver->resolve(info._system);
    }
}

void MainWindow::openMissionTool() {
    auto tool = new MissionRouter(this, _router, *_systems);
    tool->show();
}

void MainWindow::openExplorationTool() {
    auto tool = new ValueRouter(this, _router, _systems);
    tool->show();
}


