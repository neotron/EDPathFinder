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

#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>

#include "EDSMQueryExecutor.h"
#include "System.h"
#include <QJsonObject>

#define SYSTEM_QUERY_URL QString("https://www.edsm.net/api-v1/system?systemName=%1&coords=1")

EDSMQueryExecutor *EDSMQueryExecutor::systemCoordinateRequest(const QString &systemName) {
    auto queryString = SYSTEM_QUERY_URL.arg(QString(QUrl::toPercentEncoding(systemName)));
    return new EDSMQueryExecutor(QUrl(queryString), Coordinates, systemName);
}

void EDSMQueryExecutor::run() {
    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QVariant(int(QNetworkRequest::AlwaysNetwork)));
    request.setUrl(_url);
    if(!_mgr) {
        _mgr = new QNetworkAccessManager();
    }
    QNetworkReply *pReply = _mgr->get(request);
    connect(_mgr, SIGNAL(finished(QNetworkReply * )), this, SLOT(replyFinished(QNetworkReply * )));

    QEventLoop eLoop;
    QObject::connect(pReply, SIGNAL(finished()), &eLoop, SLOT(quit()));
    eLoop.exec(QEventLoop::ExcludeUserInputEvents);
}

void EDSMQueryExecutor::replyFinished(QNetworkReply *reply) {
    if(reply->error() == QNetworkReply::NoError && reply->isReadable()) {
        auto data     = reply->readAll();
        auto document = QJsonDocument::fromJson(data);
        if(document.isObject()) {
            emit coordinatesReceived(System(document.object()));
            delete reply;
            return;
        }
    }
    emit coordinateRequestFailed(_systemName);
    delete reply;
}

EDSMQueryExecutor::~EDSMQueryExecutor() {
    if(_mgr) { delete _mgr; }
}

EDSMQueryExecutor::EDSMQueryExecutor(const QUrl &url, RequestType requestType, const QString &systemName)
        : QThread(Q_NULLPTR), _mgr(nullptr),
          _requestType(requestType), _url(url), _systemName(systemName) {
}









