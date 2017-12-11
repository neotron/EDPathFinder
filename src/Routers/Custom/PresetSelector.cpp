#include <src/Model/PresetEntry.h>
#include "PresetSelector.h"
#include "ui_PresetSelector.h"

#define ADD_ROW_STR(S) do { \
  auto ITEM = new QTableWidgetItem(S); \
  table->setItem(row, col++, ITEM); \
  ITEM->setFlags(ITEM->flags() ^ Qt::ItemIsEditable); \
  ITEM->setBackground(palette().alternateBase()); \
} while(false)

PresetSelector::PresetSelector(QWidget *parent, const PresetEntryList &presets)
    : QDialog(parent), _ui(new Ui::PresetSelector), _presets(presets)
{
    _ui->setupUi(this);
    auto table = _ui->tableWidget;
    int row = 0, col = 0;
    table->setColumnCount(4);
    QStringList headers;
    headers << "" << "Name" << "System" << "Notes";
    table->setHorizontalHeaderLabels(headers);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table->setSortingEnabled(true);
    if(!presets.empty()) {
        for(const auto &entry: presets) {
            col = 0;
            table->insertRow(row);
            auto item = new QTableWidgetItem();
            item->setBackground(palette().alternateBase());
            item->data(Qt::CheckStateRole);
            item->setCheckState(Qt::Unchecked);
            item->setData(Qt::UserRole, row);
            table->setItem(row, col++, item);
            ADD_ROW_STR(entry.shortDescription());
            ADD_ROW_STR(entry.systemName());
            ADD_ROW_STR(entry.details());
            row++;
        }
        setWindowTitle(QString("Add Preset Systems: %1").arg(presets[0].type()));
    }
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}

PresetSelector::~PresetSelector()
{
    delete _ui;
}

void PresetSelector::slotSelectAll() {
    for(int row = 0; row < _ui->tableWidget->rowCount(); row++) {
        _ui->tableWidget->item(row, 0)->setCheckState(Qt::Checked);
    }
}

void PresetSelector::slotClearSelection() {
    for(int row = 0; row < _ui->tableWidget->rowCount(); row++) {
        _ui->tableWidget->item(row, 0)->setCheckState(Qt::Unchecked);
    }
}

void PresetSelector::accept() {
    PresetEntryList selectedEntries;
    for(int row = 0; row < _ui->tableWidget->rowCount(); row++) {
        auto check = _ui->tableWidget->item(row, 0);
        if(check->checkState() == Qt::Checked) {
            bool hadRow = false;
            int presetRow = check->data(Qt::UserRole).toInt(&hadRow);
            if(hadRow) {
                selectedEntries.push_back(_presets[presetRow]);
            }
        }
    }
    emit didSelectEntries(selectedEntries);
    QDialog::accept();
}
