#include "RouteViewer.h"
#include "ui_RouteViewer.h"
#include "RouteTableModel.h"
#include <QDebug.h>
#include <QClipboard>

RouteViewer::RouteViewer(const RouteResult &result, QWidget *parent) :
    QMainWindow(parent), _ui(new Ui::RouteViewer) {
    _ui->setupUi(this);
    QTableView *table = _ui->tableView;
    table->setModel(new RouteTableModel(this, result));
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->adjustSize();
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setSelectionMode(QTableView::SingleSelection);
    QHeaderView *horizontalHeader = table->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::Stretch);
    setAttribute(Qt::WA_DeleteOnClose, true);

    connect(table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(copySelectedItem()));
}

RouteViewer::~RouteViewer()
{
    delete _ui;
}

void RouteViewer::copySelectedItem() {
    auto indices = _ui->tableView->selectionModel()->selectedIndexes();;
    if(indices.count() >= 1) {
        const auto &selectedIndex(indices[0]);
        if(selectedIndex.row() > 0) {
            const auto &cellValue = _ui->tableView->model()->data(selectedIndex).toString();
            if(cellValue.length()) {
                QApplication::clipboard()->setText(cellValue);
                _ui->statusbar->showMessage(QString("Copied system name `%1' to the system clipboard.").arg(cellValue));
            }
        }
    }
}


