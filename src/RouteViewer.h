#ifndef ROUTEVIEWER_H
#define ROUTEVIEWER_H

#include <QMainWindow>
#include <QItemSelection>
#include "System.h"
#include "TSPWorker.h"

namespace Ui {
class RouteViewer;
}

class RouteViewer : public QMainWindow {
Q_OBJECT

public:
    explicit RouteViewer(const RouteResult &result, QWidget *parent = 0);

    ~RouteViewer();
public slots:
    void copySelectedItem();
private:
    Ui::RouteViewer *_ui;
};

#endif // ROUTEVIEWER_H
