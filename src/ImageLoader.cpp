////
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

#include <QPixmap>
#include <QStandardPaths>
#include <QNetworkDiskCache>
#include "ImageLoader.h"
#include "AspectRatioPixmapLabel.h"
#include "ImageViewer.h"

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
        QUrl url     = attribute.toUrl();
        auto request = QNetworkRequest(url);
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        _reply = _networkManager->get(request);
        return;
    }
    //qDebug() << "ContentType:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QByteArray data = reply->readAll();
    QPixmap    pixmap;
    pixmap.loadFromData(data);
    updateLabelWithPixmap(pixmap);
}

void ImageLoader::updateLabelWithPixmap(const QPixmap &pixmap) const {
    if(_pixmapHolder) {
        auto scalingLabel = dynamic_cast<ImageViewer*>(_pixmapHolder);
        auto pixmapLabel = dynamic_cast<QLabel*>(_pixmapHolder);
        if(scalingLabel) {
            scalingLabel->setPixmap(pixmap);
            //scalingLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        } else if(pixmapLabel) {
            pixmapLabel->setPixmap(pixmap);
        }
    }
}


void ImageLoader::startDownload(const QUrl &url) {
    auto data = _networkManager->cache()->data(url);
    if(data) {
        QPixmap pixmap;
        pixmap.loadFromData(data->readAll());
        if(!pixmap.isNull()) {
            updateLabelWithPixmap(pixmap);
            return;
        }
    }

    QNetworkRequest request(url);
    _reply = _networkManager->get(request);
}

ImageLoader::ImageLoader(QWidget *pixmapHolder) : _maxSize(QSize()), _networkManager(new QNetworkAccessManager(this)), _reply(nullptr), _pixmapHolder(pixmapHolder) {
    connect(_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReplyReceived(QNetworkReply *)));

    auto cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation).append("/imagecache");
    auto cache = new QNetworkDiskCache();
    cache->setCacheDirectory(cachePath);
    _networkManager->setCache(cache);
}
