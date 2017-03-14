//
//  Copyright (C) 2017  David Hedbor <neotron@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <QListView>
#include "SystemEntryCoordinateResolver.h"
#include "EDSMQueryExecutor.h"


SystemEntryCoordinateResolver::SystemEntryCoordinateResolver(QObject *parent, AStarRouter *router, QLineEdit *lineEdit)
        : QObject(parent), _router(router), _lineEdit(lineEdit), _pendingLookups() {

    QCompleter *completer = new QCompleter(_router, this);
    completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    QListView *popup = (QListView *) completer->popup();
    popup->setBatchSize(10);
    popup->setLayoutMode(QListView::Batched);
    connect(completer, SIGNAL(activated(const QString &)), this, SLOT(onCompletion(const QString &)));
    _lineEdit->setCompleter(completer);
    connect(_lineEdit, SIGNAL(editingFinished()), this, SLOT(onEntry()));
}



void SystemEntryCoordinateResolver::downloadSystemCoordinates(const QString &systemName) {
    if(_pendingLookups.contains(systemName)) {
        return;
    }
    _pendingLookups << systemName.toLower();
    auto executor = EDSMQueryExecutor::systemCoordinateRequest(systemName);
    connect(executor, &QThread::finished, executor, &QObject::deleteLater);
    connect(executor, &EDSMQueryExecutor::coordinatesReceived, this, &SystemEntryCoordinateResolver::systemCoordinatesReceived);
    connect(executor, &EDSMQueryExecutor::coordinateRequestFailed, this,
            &SystemEntryCoordinateResolver::systemCoordinatesRequestFailed);
    executor->start();
    emit systemLookupInitiated(systemName);

}


void SystemEntryCoordinateResolver::systemCoordinatesRequestFailed(const QString &systemName) {
    _pendingLookups.remove(systemName.toLower());
    emit systemLookupFailed(systemName);
}

void SystemEntryCoordinateResolver::systemCoordinatesReceived(const System &system) {
    auto systemName = QString(system.name().toLower());
    _pendingLookups.remove(systemName);
    _router->addSystem(system);
    _router->sortSystemList();
    emit systemLookupCompleted(system);
}


void SystemEntryCoordinateResolver::onCompletion(const QString &systemName) {
    resolve(systemName);
}

void SystemEntryCoordinateResolver::onEntry() {
    resolve(_lineEdit->text());
}

void SystemEntryCoordinateResolver::resolve(const QString &systemName) {
    if(systemName.isEmpty()) {
        return;
    }
    auto system = _router->findSystemByName(systemName);
    if(system) {
        emit systemLookupCompleted(*system);
    } else {
        downloadSystemCoordinates(systemName);
    }
}

bool SystemEntryCoordinateResolver::isComplete() const {
    return !_pendingLookups.size();
}







