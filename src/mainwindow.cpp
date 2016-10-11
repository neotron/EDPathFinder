#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tsp.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(ui->createRouteButton, SIGNAL(clicked()), this, SLOT(createRoute()));

    cleanupCheckboxes();
    buildLookupMap();
    loadSystems();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::cleanupCheckboxes() {
    QList<QCheckBox *> layouts = findChildren<QCheckBox *>();
    int width = 0;
            foreach(QCheckBox *checkbox, layouts) { width = qMax(checkbox->width(), width); }
            foreach(QCheckBox *checkbox, layouts) { checkbox->setMinimumWidth(width); }
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

void MainWindow::routeCalculated() {
    ui->statusBar->showMessage("Route calculated.", 10000);
    ui->createRouteButton->setEnabled(true);
}

void MainWindow::createRoute() {
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

    int32 matches = 0;
    std::deque<System> matchingSystems;
    for(auto &it : _systems) {
        bool found = false;
        std::deque<Settlement> matchingSettlements;
        for(auto sit: it.settlements()) {
            if((sit.flags() & settlementFlags) != settlementFlags) {
                continue;
            }
            if(jumpsExcluded && (sit.flags()&SettlementFlagsJumpClimbRequired) == SettlementFlagsJumpClimbRequired) {
                continue;
            }
            matchingSettlements.push_back(sit);
            ++matches;
        }
        if(matchingSettlements.size()) {
            matchingSystems.push_back(System(it.system(), it.planet(), matchingSettlements, it.x(), it.y(), it.y()));
        }
    }
    qDebug() << "Out of " << _systems.size() << ", "<<matchingSystems.size()<<" systems matched the filter.";
    if(matchingSystems.size() > 0) {
        ui->statusBar->showMessage(QString("Calculating route with %1 settlements in %2 systems...").arg(matches).arg(matchingSystems.size()));
        ui->createRouteButton->setEnabled(false);
        TSPWorker *workerThread(new TSPWorker(matchingSystems));
        connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
        connect(workerThread, &TSPWorker::taskCompleted, this, &MainWindow::routeCalculated);
        workerThread->start();
    } else {
        ui->statusBar->showMessage("No settlements found that matches your filters.", 10000);
    }
}

void MainWindow::loadSystems() {
    SystemLoader loader;
   	_systems = loader.loadSettlements();

}








