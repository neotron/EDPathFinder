#include <src/Model/PresetEntry.h>
#include "PresetSelector.h"
#include "ui_PresetSelector.h"

#define ADD_ROW_STR(S) do { \
  auto ITEM = new QTableWidgetItem(S); \
  table->setItem(row, col++, ITEM); \
  ITEM->setFlags(ITEM->flags() ^ Qt::ItemIsEditable); \
  ITEM->setBackground(palette().alternateBase()); \
} while(false)

PresetSelector::PresetSelector(QWidget *parent, const PresetEntryList &presets) :
    QDialog(parent),
    _ui(new Ui::PresetSelector)
{
    _ui->setupUi(this);
    auto table = _ui->tableWidget;
    int row = 0, col = 0;
    table->setColumnCount(4);
    QStringList headers;
    headers << "Select" << "Name" << "System" << "Notes";
    table->setHorizontalHeaderLabels(headers);
    table->setSortingEnabled(true);
    for(const auto &entry: presets) {
        col = 0;
        table->insertRow(row);
        auto item = new QTableWidgetItem();
        item->setBackground(palette().alternateBase());
        item->data(Qt::CheckStateRole);
        item->setCheckState(Qt::Unchecked);
        table->setItem(row, col++, item);
        ADD_ROW_STR(entry.shortDescription());
        ADD_ROW_STR(entry.systemName());
        ADD_ROW_STR(entry.details());
        row++;
    }
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}

PresetSelector::~PresetSelector()
{
    delete _ui;
}
