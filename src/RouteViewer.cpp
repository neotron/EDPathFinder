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
#include "RouteViewer.h"
#include "ui_RouteViewer.h"


RouteViewer::RouteViewer(const RouteResult &result, QWidget *parent) : QMainWindow(parent), _ui(new Ui::RouteViewer), _iconLoader(nullptr), _imageLoader(nullptr) {
    _ui->setupUi(this);
    QTableView *table = _ui->tableView;
    _routeModel = new RouteTableModel(this, result);
    table->setModel(_routeModel);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->adjustSize();
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setSelectionMode(QTableView::SingleSelection);
    QHeaderView *horizontalHeader = table->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader->setStretchLastSection(true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    connect(table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(copySelectedItem()));
    table->selectRow(0);
}

RouteViewer::~RouteViewer() {
    delete _iconLoader;
    delete _imageLoader;
    delete _ui;
}

void RouteViewer::copySelectedItem() {
    updateSettlementInfo();
}


void RouteViewer::updateSettlementInfo() {
    auto       indices        = _ui->tableView->selectionModel()->selectedIndexes();
    const auto row            = indices[0].row();
    const auto settlementData = _routeModel->result().getSettlementAtIndex(row);
    const auto settlement     = settlementData->settlement();
    const auto settlementType = settlement.type();

    QApplication::clipboard()->setText(settlement.name());
    _ui->statusbar->showMessage(QString("Copied system name `%1' to the system clipboard.").arg(settlement.name()));
    _ui->lastDistanceLabel->setText(QString("%1 ly").arg(_routeModel->lastDistance((size_t)row)));
    _ui->startDistanceLabel->setText(QString("%1 ly").arg(_routeModel->totalDistance((size_t)row)));
    _ui->planetLabel->setText(settlementData->planetName());
    _ui->settlementLabel->setText(settlement.name());
    _ui->systemLabel->setText(settlementData->systemName());
    _ui->typeLabel->setText(settlementType->economy());
    switch(settlement.threatLevel()) {
        case ThreatLevelLow:
            _ui->threatLabel->setText("Low");
            break;
        case ThreatLevelRestrictedLongDistance:
            _ui->threatLabel->setText("Restricted");
            break;
        case ThreatLevelMedium:
            _ui->threatLabel->setText("Medium");
            break;
        case ThreatLeveLHigh:
            _ui->threatLabel->setText("High");
            break;
    }
    switch(settlement.size()) {
        case SettlementSizeSmall:
            _ui->sizeLabel->setText("Small");
            break;
        case SettlementSizeMedium:
            _ui->sizeLabel->setText("Medium");
            break;
        case SettlementSizeLarge:
            _ui->sizeLabel->setText("Large");
            break;
    }

    setFlag(settlement, "cdt", SettlementFlagsCoreDataTerminal);
    setFlag(settlement, "jump", SettlementFlagsJumpClimbRequired);
    setFlag(settlement, "csd", SettlementFlagsClassifiedScanDatabanks);
    setFlag(settlement, "csf", SettlementFlagsClassifiedScanFragment);
    setFlag(settlement, "cif", SettlementFlagsCrackedIndustrialFirmware);
    setFlag(settlement, "dsd", SettlementFlagsDivergentScanData);
    setFlag(settlement, "mcf", SettlementFlagsModifiedConsumerFirmware);
    setFlag(settlement, "mef", SettlementFlagsModifiedEmbeddedFirmware);
    setFlag(settlement, "osk", SettlementFlagsOpenSymmetricKeys);
    setFlag(settlement, "sfp", SettlementFlagsSecurityFirmwarePatch);
    setFlag(settlement, "slf", SettlementFlagsSpecializedLegacyFirmware);
    setFlag(settlement, "tec", SettlementFlagsTaggedEncryptionCodes);
    setFlag(settlement, "uef", SettlementFlagsUnusualEncryptedFiles);
    setFlag(settlement, "anarchy", SettlementFlagsAnarchy);

    delete _iconLoader;
    _iconLoader = new ImageLoader(_ui->settlementIcon);
    _iconLoader->startDownload(settlementType->iconUrl());

    _ui->largeImage->setPixmap(QPixmap(":/noimage.png"));
    delete _imageLoader;
    _imageLoader = nullptr;

    if(!settlementType->pathMapUrl().isEmpty()) {
        loadOverviewImage(settlementType->pathMapUrl());
    } else if(!settlementType->overviewUrl().isEmpty()){
        loadOverviewImage(settlementType->overviewUrl());
    } else if(!settlementType->coreUrl().isEmpty()) {
        loadOverviewImage(settlementType->coreUrl());
    } else if(!settlementType->showMapUrl().isEmpty()) {
        loadOverviewImage(settlementType->showMapUrl());
    } else if(!settlementType->coreFullMapUrl().isEmpty()) {
        loadOverviewImage(settlementType->coreFullMapUrl());
    } else if(!settlementType->overview3DUrl().isEmpty()) {
        loadOverviewImage(settlementType->overview3DUrl());
    }

}

void RouteViewer::setFlag(const Settlement &settlement, QString key, SettlementFlags flag) {
    auto label = _ui->centralwidget->findChild<QLabel*>(key);
    if(label) {
        bool hasFlag = (settlement.flags()&flag) == flag;
        label->setEnabled(hasFlag);
        QFont font(label->font());
        font.setBold(hasFlag);
        label->setFont(font);
    }
}

void RouteViewer::loadOverviewImage(const QUrl &url) {
    _imageLoader = new ImageLoader(_ui->largeImage);
    _imageLoader->setMaxSize(QSize(800, 800));
    _imageLoader->startDownload(url);
}





