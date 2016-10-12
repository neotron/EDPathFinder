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
    void updateFilters();

private:
    Ui::MainWindow *ui;

    void cleanupCheckboxes();


    void buildLookupMap();

    void loadSystems();
private:
    QMap<QString, SettlementFlags> _flagsLookup;

    std::deque<System> _systems;
    std::deque<System> _filteredSystems;
    int32 _matchingSettlementCount;
};

#endif // MAINWINDOW_H
