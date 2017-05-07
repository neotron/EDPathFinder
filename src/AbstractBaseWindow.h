//
// Created by David Hedbor on 2017-05-07.
//

#pragma once

#include  <QString>
#include <QMainWindow>
#include <QCheckBox>
#include <QRadioButton>
#include "System.h"

#include "AStarRouter.h"
#include "SystemEntryCoordinateResolver.h"
#include "TSPWorker.h"
#include "RouteViewer.h"


class BaseSlots : public QMainWindow {
Q_OBJECT


public:
    explicit BaseSlots(QWidget *parent)
            : QMainWindow(parent) {
    }

protected slots:

    virtual void systemCoordinatesRequestFailed(const QString &) {}

    virtual void systemCoordinatesRequestInitiated(const QString &) {}

    virtual void updateSystemCoordinateDisplay(const System &) {}

    virtual void updateFilters() {}

    virtual void createRoute() {}

    virtual void routeCalculated(const RouteResult &) {}

    virtual void openMissionTool() {}

    virtual void openExplorationTool() {}
};


template<class UIClass>
class AbstractBaseWindow : public BaseSlots {

public:
    AbstractBaseWindow(QWidget *parent, AStarRouter *router, SystemList *systems)
            : BaseSlots(parent), _ui(new UIClass()), _router(router), _systems(systems), _routingPending(false), _systemsOnly(false) {
        _ui->setupUi(this);
        connect(_ui->createRouteButton, SIGNAL(clicked()), this, SLOT(createRoute()));
        _systemResolver = new SystemEntryCoordinateResolver(this, _router, _ui->systemName);

        connect(_systemResolver, SIGNAL(systemLookupInitiated(
                                                const QString &)), this, SLOT(systemCoordinatesRequestInitiated(
                                                                                      const QString &)));
        connect(_systemResolver, SIGNAL(systemLookupFailed(
                                                const QString &)), this, SLOT(systemCoordinatesRequestFailed(
                                                                                      const QString &)));
        connect(_systemResolver, SIGNAL(systemLookupCompleted(
                                                const System &)), this, SLOT(updateSystemCoordinateDisplay(
                                                                                     const System &)));

        connectCheckboxes();
    }

    virtual ~AbstractBaseWindow() { delete _ui; }

protected :
    void systemCoordinatesRequestInitiated(const QString &systemName) {
        showMessage(QString("Looking up coordinates for system: %1").arg(systemName));
    }

    void systemCoordinatesRequestFailed(const QString &systemName) {
        showMessage(QString("Unknown origin system: %1").arg(systemName));
        _ui->systemName->setEnabled(true);
        _routingPending = false;
    }

    void updateSystemCoordinateDisplay(const System &system) {
        _ui->x->setText(QString::number(system.x()));
        _ui->y->setText(QString::number(system.y()));
        _ui->z->setText(QString::number(system.z()));
        _ui->systemName->setText(system.name());
        showMessage(QString("Found coordinates for system: %1").arg(_ui->systemName->text()), 4000);
        _ui->createRouteButton->setEnabled(!_routingPending);
        if(_routingPending) {
            _routingPending = false;
            createRoute();
        }
    }

    void showMessage(const QString &message, int timeout = 10000) const {
        _ui->statusBar->showMessage(message, timeout);
    }

    void connectCheckboxes() {
        for(auto checkbox: findChildren<QCheckBox *>()) {
            connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(updateFilters()));
        }

        for(auto radio: findChildren<QRadioButton *>()) {
            connect(radio, SIGNAL(toggled(bool)), this, SLOT(updateFilters()));
        }

        connect(_ui->filterCommander, SIGNAL(activated(const QString &)), this, SLOT(updateFilters()));
    }

    virtual void createRoute() {
        if(_filteredSystems.size() > 0) {
            auto systemName = _ui->systemName->text();
            auto originSystem = _router->findSystemByName(systemName);
            if(!originSystem) {
                // Need to fetch coordinates for origin.
                _systemResolver->resolve(systemName);
                _routingPending = true;
                return;
            }
            auto routeSize = _ui->systemCountSlider->value();
            updateSystemCoordinateDisplay(*originSystem);
            showMessage(QString("Calculating route with %1 systems starting at %2...").arg(routeSize).arg(originSystem->name()),0);
            _ui->createRouteButton->setEnabled(false);
            TSPWorker *workerThread(new TSPWorker(_filteredSystems, originSystem, routeSize));
            workerThread->setSystemsOnly(_systemsOnly);
            // workerThread->setRouter(_router);
            connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
            connect(workerThread, &TSPWorker::taskCompleted, this, &AbstractBaseWindow::routeCalculated);
            workerThread->start();
            _ui->centralWidget->setEnabled(false);
        } else {
            _ui->statusBar->showMessage("No results found for your filters.", 10000);
        }
    }

    virtual void routeCalculated(const RouteResult &route) {
        _ui->centralWidget->setEnabled(true);
        if(!route.isValid()) {
            _ui->statusBar->showMessage("No solution found to the given route.", 10000);
            return;
        }
        _ui->statusBar->showMessage("Route calculation completed.", 10000);
        _ui->createRouteButton->setEnabled(true);
    }


    UIClass *_ui;

    AStarRouter *_router;
    SystemList *_systems;
    SystemEntryCoordinateResolver *_systemResolver;
    SystemList _filteredSystems;
    bool _routingPending;
    bool _systemsOnly;
};

