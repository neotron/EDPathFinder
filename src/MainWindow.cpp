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
#include <QMessageBox>
#include "MainWindow.h"
#include "QCompressor.h"
#include "MissionRouter.h"
#include "ValueRouter.h"
#include "AutoUpdateChecker.h"
#include "buildnumber.h"
#include "Preferences.h"
#include "Settings.h"

#define IS_SET(__FLAGS, __FLAG) (((__FLAGS) & (__FLAG)) == (__FLAG))
#define CHECKBOX(__BOX, __FLAGS, __FLAG) (__BOX)->setChecked(IS_SET(__FLAGS, __FLAG))
#define CHECKNAME(__NAME, __FLAGS, __FLAG) CHECKBOX(_ui->__NAME, __FLAGS, __FLAG)

#define SET_IF_CHECKED(VAL, CHECKBOX, FLAG) do {  if(_ui->CHECKBOX->isChecked()) {  VAL |= FLAG; } } while(false)


MainWindow::MainWindow(QWidget *parent)
        : AbstractBaseWindow(parent, new AStarRouter(), new SystemList()),
          _journalWatcher(new JournalWatcher(this)), _settlementDates(), _lastMaterialCount(0) {
    int32 flags, sizes, threat;
    QString commander;
    Settings::getFilterSettings(flags, sizes, threat, commander);

    buildLookupMap();
    loadCompressedData();

    QList<QCheckBox *> checkboxes = findChildren<QCheckBox *>();
    for(auto &checkbox : checkboxes) {
        auto flag = _nameToFlagLookup.find(checkbox->objectName());
        if(flag != _nameToFlagLookup.end()) {
            CHECKBOX(checkbox, flags, *flag);
        }
    }

    CHECKNAME(noSec, threat, ThreatLevelLow);
    CHECKNAME(mediumSec, threat, ThreatLevelMedium);
    CHECKNAME(highSec, threat, ThreatLeveLHigh);
    CHECKNAME(restrictedSec, threat, ThreatLevelRestrictedLongDistance);

    CHECKNAME(smallSize, sizes, SettlementSizeSmall);
    CHECKNAME(mediumSize, sizes, SettlementSizeMedium);
    CHECKNAME(largeSize, sizes, SettlementSizeLarge);

    _ui->centralWidget->setEnabled(false);
    _ui->menuBar->setEnabled(false);
    connect(_journalWatcher, SIGNAL(onEvent(const JournalFile &, const Event &)),
            this, SLOT(handleEvent( const JournalFile &, const Event &)));
    _ui->filterCommander->setInsertPolicy(QComboBox::InsertAlphabetically);
    _ui->distanceSlider->setMaximum(10000);
    _ui->distanceSlider->setValue(10000);

    _ui->minMats->setToolTip("Exclude settlements that can't provide at least this many of your wanted materials.");
    _ui->dropProbability->setToolTip("Exclude matched materials if the probability quotient is lower than this. Note that some materials never have very high probability.");

}

MainWindow::~MainWindow() {
    delete _router;
    delete _journalWatcher;
    delete _systemResolver;
}

void MainWindow::buildLookupMap() {
    _nameToFlagLookup["cdt"] = SettlementFlagsCoreDataTerminal;
    _nameToFlagLookup["jump"] = SettlementFlagsJumpClimbRequired;
    _nameToFlagLookup["csd"] = SettlementFlagsClassifiedScanDatabanks;
    _nameToFlagLookup["csf"] = SettlementFlagsClassifiedScanFragment;
    _nameToFlagLookup["cif"] = SettlementFlagsCrackedIndustrialFirmware;
    _nameToFlagLookup["dsd"] = SettlementFlagsDivergentScanData;
    _nameToFlagLookup["mcf"] = SettlementFlagsModifiedConsumerFirmware;
    _nameToFlagLookup["mef"] = SettlementFlagsModifiedEmbeddedFirmware;
    _nameToFlagLookup["osk"] = SettlementFlagsOpenSymmetricKeys;
    _nameToFlagLookup["sfp"] = SettlementFlagsSecurityFirmwarePatch;
    _nameToFlagLookup["slf"] = SettlementFlagsSpecializedLegacyFirmware;
    _nameToFlagLookup["tec"] = SettlementFlagsTaggedEncryptionCodes;
    _nameToFlagLookup["uef"] = SettlementFlagsUnusualEncryptedFiles;
}

void MainWindow::routeCalculated(const RouteResult &route) {
    AbstractBaseWindow::routeCalculated(route);
    auto viewer = new RouteViewer(route, this);
    viewer->show();
}

void MainWindow::updateFilters() {
    float minProbability = _ui->dropProbability->value()/10.0f;
    _ui->dropProbabilityLabel->setText(QString("%1").arg(minProbability));

    int32 settlementFlags = 0;
    QList<QCheckBox *> checkboxes = findChildren<QCheckBox *>();
    bool jumpsExcluded = false;
    int maxDistance = distanceSliderValue();
    const auto distanceFilterChecked = _ui->distanceCheckbox->isChecked();
    _ui->distanceSlider->setEnabled(distanceFilterChecked);
    QList<SettlementFlags> materialFilters;
    for(auto &checkbox : checkboxes) {
        if(checkbox->isChecked()) {
            auto flag = _nameToFlagLookup.find(checkbox->objectName());
            if(flag != _nameToFlagLookup.end()) {
                settlementFlags |= *flag;
                if(*flag == SettlementFlagsJumpClimbRequired) {
                    jumpsExcluded = true;
                } else {
                    materialFilters.push_back(*flag);
                }
            }
        }
    }
    if(materialFilters.size() != _lastMaterialCount) {
        _lastMaterialCount = materialFilters.size();
        _ui->minMats->setMaximum(_lastMaterialCount);
        _ui->minMats->setValue(_lastMaterialCount);
    }

    auto selectedCommander = _ui->filterCommander->currentText();
    auto commanders = _settlementDates.keys();
    auto visitedSettlements = QMap<QString, QDateTime>();
    if(!commanders.empty()) {
        commanders.sort();
        for(const auto &commanderName: commanders) {
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
    SET_IF_CHECKED(threatFilter, restrictedSec, ThreatLevelRestrictedLongDistance);
    SET_IF_CHECKED(threatFilter, mediumSec, ThreatLevelMedium);
    SET_IF_CHECKED(threatFilter, noSec, ThreatLevelLow);
    SET_IF_CHECKED(threatFilter, highSec, ThreatLeveLHigh);

    int32 settlementSizes = 0;
    SET_IF_CHECKED(settlementSizes, smallSize, SettlementSizeSmall);
    SET_IF_CHECKED(settlementSizes, mediumSize, SettlementSizeMedium);
    SET_IF_CHECKED(settlementSizes, largeSize, SettlementSizeLarge);

    Settings::setFilterSettings(settlementFlags, settlementSizes, threatFilter, selectedCommander);

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
            for(const auto &settlement: planet.settlements()) {
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
                int matchingMaterials = 0;
                for(const auto &flag: materialFilters) {
                    if(IS_SET(settlement.flags(), flag) && settlement.materialProbability(flag) >= minProbability) {
                        matchingMaterials++;
                    }
                }
                if(matchingMaterials < _ui->minMats->value()) {
                    continue;
                }
                if((settlementSizes & settlement.size()) != settlement.size()) {
                    continue;
                }
                if((threatFilter & settlement.threatLevel()) != settlement.threatLevel()) {
                    continue;
                }
                if(jumpsExcluded && IS_SET(settlement.flags(), SettlementFlagsJumpClimbRequired)) {
                    continue;
                }
                matchingSettlements.push_back(settlement);
                ++matches;
            }
            if(!matchingSettlements.empty()) {
                matchingPlanets.push_back(Planet(planet.name(), planet.distance(), matchingSettlements));
            }
        }
        if(!matchingPlanets.empty()) {
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


    auto loader = new SystemLoader(_router);

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
    _journalWatcher->watchDirectory(Settings::journalPath(), newerThanDate);
    _loading = false;
    updateCommanderAndSystem();

    auto updater(new AutoUpdateChecker(this));
    connect(updater, &QThread::finished, updater, &QObject::deleteLater);
    connect(updater, SIGNAL(newVersionAvailable(const Version &)), this, SLOT(showVersionUpdateDialog(const Version &)));
    updater->start();
}

void MainWindow::showVersionUpdateDialog(const Version &newVersion) {
    const QString title = "Update Available.";
    const QString message = QString("Version %1 is available for download. You are currently running "
                                            "version %2. \n\nDo you want to download the new version?")
            .arg(newVersion.toString()).arg(PROJECT_VERSION);

    auto reply = QMessageBox::question(this, "Update Available!", message,
                                       QMessageBox::Ignore|QMessageBox::No|QMessageBox::Yes, QMessageBox::Yes);
    switch(reply) {
    case QMessageBox::Yes:
        QDesktopServices::openUrl(RELEASE_URL);
        break;
    case QMessageBox::Ignore:
        QSettings().setValue(AUTO_UPDATE_VERSION_KEY, newVersion.toString());
        break;
    default:
        break; // no-op
    }
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
    if(_loading || commander.isEmpty()) {
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

void MainWindow::openPreferences() {
    auto prefs = new Preferences(this);
    connect(prefs, SIGNAL(journalPathUpdated(const QString &, const QString &)),
            _journalWatcher, SLOT(journalPathChanged(const QString &, const QString &)));
    prefs->show();
}


