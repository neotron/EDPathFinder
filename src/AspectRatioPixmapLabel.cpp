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

AspectRatioPixmapLabel::AspectRatioPixmapLabel(QWidget *parent) : QLabel(parent) {
}

void AspectRatioPixmapLabel::updatePixmap() {
    if(!_actualPixmap.isNull()) {
        QLabel::setPixmap(_actualPixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}


void AspectRatioPixmapLabel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updatePixmap();
}

void AspectRatioPixmapLabel::setScaledPixmap(const QPixmap &pixmap) {
    _actualPixmap = pixmap;
    updatePixmap();
}
