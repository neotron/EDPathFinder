//
// Created by David Hedbor on 2017-05-07.
//

#pragma once

#include  <QString>
#include <QMainWindow>
#include <QCheckBox>
#include <QRadioButton>
#include <deps/EDJournalQT/src/JournalWatcher.h>

#include "System.h"
#include "AStarRouter.h"
#include "SystemEntryCoordinateResolver.h"
#include "TSPWorker.h"
#include "RouteViewer.h"
#include "CommanderInfo.h"


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

    virtual void openPreferences() {}

    virtual void saveSettings() const {}

};


template<class UIClass>
class AbstractBaseWindow : public BaseSlots {

public:
    AbstractBaseWindow(QWidget *parent, AStarRouter *router, SystemList *systems)
            : BaseSlots(parent), _ui(new UIClass()), _router(router), _systems(systems), _routingPending(false),
              _systemsOnly(false), _commanderInformation() {
        _ui->setupUi(this);
        connect(_ui->createRouteButton, SIGNAL(clicked()), this, SLOT(createRoute()));
        _systemResolver = new SystemEntryCoordinateResolver(this, _router, _ui->systemName, _ui->x, _ui->y, _ui->z);
        connect(_systemResolver, SIGNAL(systemLookupInitiated(const QString &)), this, SLOT(systemCoordinatesRequestInitiated(const QString &)));
        connect(_systemResolver, SIGNAL(systemLookupFailed(const QString &)), this, SLOT(systemCoordinatesRequestFailed(const QString &)));
        connect(_systemResolver, SIGNAL(systemLookupCompleted(const System &)), this, SLOT(updateSystemCoordinateDisplay(const System &)));

        connectCheckboxes();
    }

    virtual ~AbstractBaseWindow() { delete _ui; }

protected :
    virtual void systemCoordinatesRequestInitiated(const QString &systemName) {
        _ui->createRouteButton->setEnabled(false);
        showMessage(QString("Looking up coordinates for system: %1").arg(systemName));
    }

    virtual void systemCoordinatesRequestFailed(const QString &systemName) {
        showMessage(QString("Unknown origin system: %1").arg(systemName));
        _routingPending = false;
    }

    virtual void updateSystemCoordinateDisplay(const System &system) {
        showMessage(QString("Found coordinates for system: %1").arg(system.name()), 4000);
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
            _ui->centralWidget->setEnabled(false);
            onRouterCreated(workerThread);
        } else {
            _ui->statusBar->showMessage("No results found for your filters.", 10000);
        }

    }

    virtual void onRouterCreated(TSPWorker *worker) {
        worker->start();
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

    bool updateCommanderInfo(const JournalFile &file, const Event &ev, const QString &commander) {
        CommanderInfo info;
        if(_commanderInformation.contains(commander)) {
            info = _commanderInformation[commander];
        }

        if(ev.timestamp() > info._lastEventDate) {
            info._lastEventDate = ev.timestamp();
            info._system = file.system();
            _commanderInformation[commander] = info;
            if(_ui->filterCommander->findText(commander) < 0) {
                _ui->filterCommander->addItem(commander);
            }
            return true;
        }
        return false;
    }

    void updateCommanderAndSystem() {
        QString name;
        CommanderInfo info;
        for(auto commanderName: _commanderInformation.keys()) {
            auto commander = _commanderInformation[commanderName];
            if(commander._lastEventDate > info._lastEventDate) {
                info = commander;
                name = commanderName;
            }
        }
        if(name.isEmpty()) {
            _ui->commanderFilterGroup->setEnabled(false);
        } else {
            _ui->commanderFilterGroup->setEnabled(true);
            if(_ui->filterCommander->currentText() != name) {
                _ui->filterCommander->setCurrentText(name);
                updateFilters();
            }
            if(_ui->systemName->text() != info._system) {
                _ui->systemName->setText(info._system);
                _systemResolver->resolve(info._system);
            }
        }
    }

    UIClass *_ui;

    AStarRouter *_router;
    SystemList *_systems;
    SystemEntryCoordinateResolver *_systemResolver;
    SystemList _filteredSystems;
    bool _routingPending;
    bool _systemsOnly;

    QMap<QString,CommanderInfo> _commanderInformation;
};

