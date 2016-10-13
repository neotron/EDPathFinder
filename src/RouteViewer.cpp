#include "RouteViewer.h"
#include "ui_RouteViewer.h"
#include "RouteTableModel.h"

RouteViewer::RouteViewer(const RouteResult &result, QWidget *parent) :
    QMainWindow(parent), _ui(new Ui::RouteViewer) {
    _ui->setupUi(this);
    QTableView *table = _ui->tableView;
    table->setModel(new RouteTableModel(this, result));
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->adjustSize();
    QHeaderView *horizontalHeader = table->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::Stretch);
    setAttribute(Qt::WA_DeleteOnClose, true);
}

RouteViewer::~RouteViewer()
{
    delete _ui;
}

