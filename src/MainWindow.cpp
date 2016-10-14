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
#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "TSPWorker.h"
#include "RouteViewer.h"
#include "EDSMQueryExecutor.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow) {
    _ui->setupUi(this);
    connect(_ui->createRouteButton, SIGNAL(clicked()), this, SLOT(createRoute()));
    _ui->x->setValidator( new QDoubleValidator(-50000, 50000, 5, this) );
    _ui->y->setValidator( new QDoubleValidator(-50000, 50000, 5, this) );
    _ui->z->setValidator( new QDoubleValidator(-50000, 50000, 5, this) );
    connect(_ui->systemName, SIGNAL(editingFinished()), this, SLOT(updateSystemCoordinates()));

    cleanupCheckboxes();
    buildLookupMap();
    loadSystems();

}

MainWindow::~MainWindow() {
    delete _ui;
}

void MainWindow::cleanupCheckboxes() {
    QList<QCheckBox *> checkboxes = findChildren<QCheckBox *>();
    int width = 0;
    for(auto checkbox: checkboxes) {
        connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(updateFilters()));
        width = qMax(checkbox->width(), width);
    }
    for(auto checkbox: checkboxes) {
        checkbox->setMinimumWidth(width);
    }
    auto radios = findChildren<QRadioButton *>();
    for(auto radio: radios) {
         connect(radio, SIGNAL(toggled(bool)), this, SLOT(updateFilters()));
     }
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
    if(!route.route.size()) {
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
        _ui->statusBar->showMessage(QString("Calculating route with %1 settlements in %2 systems...").arg(_matchingSettlementCount).arg(_filteredSystems.size()));
        _ui->createRouteButton->setEnabled(false);
        TSPWorker *workerThread(new TSPWorker(_filteredSystems, _ui->x->text().toDouble(), _ui->y->text().toDouble(), _ui->z->text().toDouble(), _ui->systemCountSlider->value()));
        connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
        connect(workerThread, &TSPWorker::taskCompleted, this, &MainWindow::routeCalculated);
        workerThread->start();
    } else {
        _ui->statusBar->showMessage("No settlements found that matches your filters.", 10000);
    }
}

void MainWindow::loadSystems() {
    SystemLoader loader;
   	_systems = loader.loadSettlements();
    _ui->systemCountSlider->setMinimum(0);
    _ui->systemCountSlider->setSingleStep(1);
    _ui->systemCountSlider->setMaximum((int) _systems.size());
    _ui->systemCountSlider->setValue((int)_systems.size());
    updateFilters();
}

void MainWindow::updateFilters() {
    int32 settlementFlags = 0;
    QList<QCheckBox *> checkboxes = findChildren<QCheckBox *>();
    bool jumpsExcluded = false;
    for(auto &checkbox : checkboxes)  {
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

    ThreatLevel maxThreatLevel(ThreatLevelLow);
    if(_ui->restrictedSec->isChecked()) {
        maxThreatLevel = ThreatLevelRestrictedLongDistance;
    } else if(_ui->mediumSec->isChecked()) {
        maxThreatLevel = ThreatLevelMedium;
    } else if(_ui->highSec->isChecked()) {
        maxThreatLevel = ThreatLeveLHigh;
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
    for(const auto &system : _systems) {
        bool found = false;
        std::deque<Planet> matchingPlanets;
        for(const auto &planet: system.planets()) {
            std::deque<Settlement> matchingSettlements;
            for(auto settlement: planet.settlements()) {
                if((settlement.flags() & settlementFlags) != settlementFlags) {
                    continue;
                }
                if((settlementSizes & settlement.size()) != settlement.size()) {
                    continue;
                }
                if(settlement.threatLevel() > maxThreatLevel) {
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
            _filteredSystems.push_back(System(system.name(), matchingPlanets, system.x(), system.y(), system.y()));
        }
    }
    _ui->systemCountSlider->setMaximum((int) _filteredSystems.size());
    _ui->systemCountLabel->setText(QString::number(_ui->systemCountSlider->value()));

    _matchingSettlementCount = matches;
    _ui->statusBar->showMessage(QString("Filter matches %1 settlements in %2 systems.").arg(_matchingSettlementCount).arg(_filteredSystems.size()));
}

void MainWindow::updateSystemCoordinates() {
    const QString systemName(_ui->systemName->text());
    if(!systemName.length()) {
        return;
    }
    _ui->x->setText("-");
    _ui->x->setEnabled(false);
    _ui->y->setText("-");
    _ui->y->setEnabled(false);
    _ui->z->setText("-");
    _ui->z->setEnabled(false);
    auto executor = EDSMQueryExecutor::systemCoordinateRequest(systemName);
    connect(executor, &QThread::finished, executor, &QObject::deleteLater);
    connect(executor, &EDSMQueryExecutor::coordinatesReceived, this, &MainWindow::systemCoordinatesReceived);
    executor->start();
}

void MainWindow::systemCoordinatesReceived(double x, double y, double z) {
    _ui->x->setText(QString::number(x));
    _ui->y->setText(QString::number(y));
    _ui->z->setText(QString::number(z));
}














