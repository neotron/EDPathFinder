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

#include "AspectRatioPixmapLabel.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

AspectRatioPixmapLabel::AspectRatioPixmapLabel(QWidget *parent) : QWidget(parent) {
    setBackgroundRole(QPalette::Dark);
}

void AspectRatioPixmapLabel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    if(_pixmap.isNull()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QSize pixSize = _pixmap.size();
    pixSize.scale(size(), Qt::KeepAspectRatio);

    QPixmap scaledPix = _pixmap.scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPoint pt(0, (size().height()-pixSize.height())/2);
    painter.drawPixmap(pt, scaledPix);
}


QSize AspectRatioPixmapLabel::sizeHint() const {
    return _pixmap.size();
}

const QPixmap *AspectRatioPixmapLabel::pixmap() const {
    return &_pixmap;
}

void AspectRatioPixmapLabel::setPixmap(const QPixmap &pixmap) {
    _pixmap = pixmap;
    repaint();
}


