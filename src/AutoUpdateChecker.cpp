//
//  Copyright (C) 2016  David Hedbor <neotron@gmail.com>
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
//

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QSettings>
#include "AutoUpdateChecker.h"
#include "Version.h"

AutoUpdateChecker::AutoUpdateChecker(QObject *parent)
        : QThread(parent), _mgr(nullptr) {}

void AutoUpdateChecker::run() {
    _mgr = new QNetworkAccessManager;
    auto response = _mgr->head(QNetworkRequest(RELEASE_URL));
    QEventLoop eLoop;
    connect(response, SIGNAL(finished()), &eLoop, SLOT(quit()));
    eLoop.exec(QEventLoop::ExcludeUserInputEvents);
    QVariant location = response->header(QNetworkRequest::LocationHeader);
    if(location.isValid()) {
        handleRedirectionUrl(location.toUrl());
    }

}

AutoUpdateChecker::~AutoUpdateChecker() {
    delete _mgr;
}

void AutoUpdateChecker::handleRedirectionUrl(const QUrl &url) {
    if(!url.isValid()) {
        return;
    }
    auto version = url.path().split('/').last();
    if(version.isEmpty()) {
        return;
    }
    auto ignoredVersion = QSettings().value(AUTO_UPDATE_VERSION_KEY).toString();
    if(ignoredVersion == version) {
        return;
    }

    Version current, newest(version);
    if(newest > current) {
        emit newVersionAvailable(newest);
    }
}
