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

#include <QPixmap>
#include "ImageLoader.h"
#include "AspectRatioPixmapLabel.h"

ImageLoader::~ImageLoader() {
     if(_reply) {
         _reply->abort();
     }
    delete _networkManager;
}

void ImageLoader::onNetworkReplyReceived(QNetworkReply *reply) {
    if(reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error in" << reply->url() << ":" << reply->errorString();
        return;
    }
    QVariant attribute = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(attribute.isValid()) {
        QUrl url = attribute.toUrl();
        //qDebug() << "must go to:" << url;
        return;
    }
    //qDebug() << "ContentType:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QByteArray data = reply->readAll();
    QPixmap    pixmap;
    pixmap.loadFromData(data);
    if(_pixmapLabel) {
        auto scalingLabel = dynamic_cast<AspectRatioPixmapLabel*>(_pixmapLabel);
        if(scalingLabel) {
            scalingLabel->setScaledPixmap(pixmap);
        } else {
            _pixmapLabel->setPixmap(pixmap);
        }
    }
}


void ImageLoader::startDownload(const QUrl &url) {
    QNetworkRequest request(url);
    _reply = _networkManager->get(request);
    qDebug() << "Downloading image URL"<<url;
}

ImageLoader::ImageLoader(QLabel *pixmapLabel) : _maxSize(QSize()), _networkManager(new QNetworkAccessManager(this)), _reply(nullptr), _pixmapLabel(pixmapLabel) {
    connect(_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReplyReceived(QNetworkReply *)));
}
