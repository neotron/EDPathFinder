#include "RouteViewer.h"
#include "ui_RouteViewer.h"
#include "RouteTableModel.h"

RouteViewer::RouteViewer(const RouteResult &result, QWidget *parent) :
    QMainWindow(parent), _ui(new Ui::RouteViewer) {
    _ui->setupUi(this);
    _ui->tableView->setModel(new RouteTableModel(this, result));
    _ui->tableView->resizeColumnsToContents();
    _ui->tableView->resizeRowsToContents();
    _ui->tableView->adjustSize();
    setAttribute(Qt::WA_DeleteOnClose, true);
}

RouteViewer::~RouteViewer()
{
    delete _ui;
}

