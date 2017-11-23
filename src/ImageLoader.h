//
//  Copyright (C) 2016-2017  David Hedbor <neotron@gmail.com>
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

#pragma once
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QLabel>

class ImageLoader: public QObject {
Q_OBJECT

public:
    explicit ImageLoader(QWidget *pixmapHolder);
    ~ImageLoader() override;
    void startDownload(const QUrl &url);

    void setMaxSize(const QSize &maxSize) {
        _maxSize = maxSize;
    }

private slots:
    void onNetworkReplyReceived(QNetworkReply *reply);

private:
    QSize _maxSize;
    QNetworkAccessManager *_networkManager;
    QNetworkReply *_reply;
    QWidget *_pixmapHolder;

    void updateLabelWithPixmap(const QPixmap &pixmap) const;
};




