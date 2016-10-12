#include "RouteViewer.h"
#include "ui_RouteViewer.h"

RouteViewer::RouteViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RouteViewer)
{
    ui->setupUi(this);
}

RouteViewer::~RouteViewer()
{
    delete ui;
}
