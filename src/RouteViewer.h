#ifndef ROUTEVIEWER_H
#define ROUTEVIEWER_H

#include <QMainWindow>

namespace Ui {
class RouteViewer;
}

class RouteViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit RouteViewer(QWidget *parent = 0);
    ~RouteViewer();

private:
    Ui::RouteViewer *ui;
};

#endif // ROUTEVIEWER_H
