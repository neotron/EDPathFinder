//
// Created by David Hedbor on 10/13/16.
//

#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>

#include "EDSMQueryExecutor.h"
#include <QJsonObject>

#define SYSTEM_QUERY_URL QString("http://www.edsm.net/api-v1/system?systemName=%1&coords=1")

EDSMQueryExecutor *EDSMQueryExecutor::systemCoordinateRequest(const QString &systemName) {
    auto queryString = SYSTEM_QUERY_URL.arg(QString(QUrl::toPercentEncoding(systemName)));
    return new EDSMQueryExecutor(QUrl(queryString), Coordinates);
}

void EDSMQueryExecutor::run() {
    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QVariant(int(QNetworkRequest::AlwaysNetwork)));
    request.setUrl(_url);
    _mgr = new QNetworkAccessManager();
    QNetworkReply *pReply = _mgr->get(request);
    connect(_mgr, SIGNAL(finished(QNetworkReply * )), this, SLOT(replyFinished(QNetworkReply * )));

    QEventLoop eLoop;
    QObject::connect(pReply, SIGNAL(finished()), &eLoop, SLOT(quit()));
    eLoop.exec(QEventLoop::ExcludeUserInputEvents);
}

void EDSMQueryExecutor::replyFinished(QNetworkReply *reply) {
    if(reply->error() == QNetworkReply::NoError && reply->isReadable()) {
        auto data = reply->readAll();
        auto document = QJsonDocument::fromJson(data);
        if(document.isObject()) {
            QJsonObject coords = document.object()["coords"].toObject();
            auto x = coords["x"].toDouble();
            auto z = coords["z"].toDouble();
            auto y = coords["y"].toDouble();
            emit coordinatesReceived(x, y, z);
            reply->deleteLater();
            return;
        }
    }
    emit coordinateRequestFailed();
    reply->deleteLater();
}

EDSMQueryExecutor::~EDSMQueryExecutor() {
    if(_mgr) { _mgr->deleteLater(); }
}

EDSMQueryExecutor::EDSMQueryExecutor(const QUrl &url, RequestType requestType)
        : QThread(Q_NULLPTR), _mgr(nullptr), _url(url), _requestType(requestType)
{

}









