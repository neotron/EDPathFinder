#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "System.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void createRoute();
    void routeCalculated();

private:
    Ui::MainWindow *ui;

    void cleanupCheckboxes();


    void buildLookupMap();

    void loadSystems();
private:
    QMap<QString, SettlementFlags> _flagsLookup;

    std::deque<System> _systems;

};

#endif // MAINWINDOW_H
