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
#include <src/Utility/WindowMenu.h>
#include "RouteProgressAnnouncer.h"
#include "RouteViewer.h"
#include "ui_RouteViewer.h"


#define MAP_LEGEND_TEXT "Map Legend"

RouteViewer::RouteViewer(const RouteResult &result, QWidget *parent) : QMainWindow(parent), _ui(new Ui::RouteViewer), _iconLoader(nullptr), _imageLoader(nullptr) {
    _ui->setupUi(this);
    QTableView *table = _ui->tableView;
    _routeModel = new RouteTableModel(this, result);
    setWindowTitle(QString("Settlement Route (%1, %2 hops)").arg(result.route()[0][0])
                           .arg(result.route().size()));
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
    connect(table->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(copySelectedItem()));
    table->selectRow(0);
    _ui->menubar->addMenu(new WindowMenu(this, _ui->menubar));

    // auto-deleted when parent is deleted.
    new RouteProgressAnnouncer(this, _routeModel, table);
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
    auto index = _ui->tableView->selectionModel()->currentIndex();
    const auto row = index.row();
    const auto settlementData = _routeModel->result().getSettlementAtIndex(row);
    if(!settlementData) {
        return;
    }
    const auto settlement = settlementData->settlement();
    const auto settlementType = settlement.type();

    QApplication::clipboard()->setText(settlementData->systemName());
    _ui->statusbar->showMessage(
            QString("Copied system name `%1' to the system clipboard.").arg(settlementData->systemName()));
    _ui->lastDistanceLabel->setText(QString("%1 ly").arg(_routeModel->lastDistance((size_t) row)));
    _ui->startDistanceLabel->setText(QString("%1 ly").arg(_routeModel->totalDistance((size_t) row)));
    _ui->planetLabel->setText(settlementData->planetName());
    _ui->settlementLabel->setText(settlement.name());
    _ui->systemLabel->setText(settlementData->systemName());
    _ui->typeLabel->setText(settlementType->economy());
    _ui->entryDistanceLabel->setText(settlementData->distance() > 0 ?
                                     QString("%1 ls").arg(settlementData->distance()) : "N/A");

    switch(settlement.threatLevel()) {
    case ThreatLevelLow:
        _ui->threatLabel->setText("None");
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
    case ThreatLevelUnknown:
        _ui->threatLabel->setText("Unknown");
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
    case SettlementSizeUnknown:
        _ui->sizeLabel->setText("Unknown");
        break;

    }
    switch(settlementType->securityLevel()) {
    case ThreatLevelUnknown:
    case ThreatLevelRestrictedLongDistance:
    case ThreatLevelLow:
        _ui->securityLabel->setText("Low");
        break;
    case ThreatLevelMedium:
        _ui->securityLabel->setText("Medium");
        break;
    case ThreatLeveLHigh:
        _ui->securityLabel->setText("High");
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
    _iconLoader->startDownload(settlementType->imageNamed(SettlementType::IMAGE_BASE_ICON));

    // _ui->largeImage->setPixmap(QPixmap(":/noimage.png"));
    delete _imageLoader;
    _imageLoader = nullptr;

    auto images = settlementType->imageTitles();
    images.append(MAP_LEGEND_TEXT);
    _ui->imageList->clear();
    _ui->imageList->addItems(images);

    QString preferredMap = images[0];
    if(images.contains(SettlementType::IMAGE_PATHMAP)) {
        preferredMap = SettlementType::IMAGE_PATHMAP;
    } else if(images.contains(SettlementType::IMAGE_CORE)) {
        preferredMap = SettlementType::IMAGE_CORE;
    } else if(images.contains(SettlementType::IMAGE_COREFULLMAP)) {
        preferredMap = SettlementType::IMAGE_COREFULLMAP;
    } else if(images.contains(SettlementType::IMAGE_OVERVIEW)) {
        preferredMap = SettlementType::IMAGE_OVERVIEW;
    } else if(images.contains(SettlementType::IMAGE_SATELLITE)) {
        preferredMap = SettlementType::IMAGE_SATELLITE;
    }
    _ui->imageList->setCurrentText(preferredMap);
    loadOverviewImage(settlementType->imageNamed(preferredMap));
}

void RouteViewer::setFlag(const Settlement &settlement, QString key, SettlementFlags flag) {
    auto label = _ui->centralwidget->findChild<QLabel*>(key);
    auto problabel = _ui->centralwidget->findChild<QLabel*>(key+"Prob");

    if(label) {
        bool hasFlag = (settlement.flags()&flag) == flag;
        label->setEnabled(hasFlag);
        if(problabel) {
            problabel->setText(hasFlag ? QString("%1").arg(settlement.materialProbability(flag)) : "");
        }
    }

}

void RouteViewer::loadOverviewImage(const QUrl &url) {
    _imageLoader = new ImageLoader(_ui->largeImage);
    _imageLoader->setMaxSize(QSize(800, 800));
    _imageLoader->startDownload(url);
}

void RouteViewer::loadSelectedImage(const QString &image) {
    if(image == MAP_LEGEND_TEXT) {
        QPixmap legend(":/legend.jpg");
        _ui->largeImage->setPixmap(legend);
    } else {
        auto       indices        = _ui->tableView->selectionModel()->selectedIndexes();
        const auto row            = indices[0].row();
        const auto settlementData = _routeModel->result().getSettlementAtIndex(row);
        const auto settlementType = settlementData->settlement().type();

        loadOverviewImage(settlementType->imageNamed(image));
    }
}

void RouteViewer::exportAsCSV() {
    RouteTableModel::exportTableViewToCSV(_ui->tableView);
}

void RouteViewer::exportAsTabNewline() {
    RouteTableModel::exportTableViewToTabNewline(_ui->tableView);
}
