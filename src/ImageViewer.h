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
//

#pragma once

#include <QScrollArea>
#include <QApplication>
#include "AspectRatioPixmapLabel.h"
#include "FlickCharm.h"

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class ImageViewer : public QScrollArea
{
    Q_OBJECT

public:
    ImageViewer(QWidget *parent);

    ImageViewer(QScrollAreaPrivate &dd, QWidget *parent) : QScrollArea(dd, parent) {
        sharedInitialize();
    }

    void setPixmap(const QPixmap &pixmap);

private slots:

    void zoomIn();
    void zoomOut();
    void fitToWindow();

private:
    template<typename T>
    T findSibling(const QString &aName) const  {
        QObject *p      = parent();
        return p->findChild<T>(aName);
    }


public:
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    AspectRatioPixmapLabel *_imageLabel;
    FlickCharm _flicker;
    double _scaleFactor;

    void sharedInitialize();

    void updateZoomButtonEnabledStates() const;
};
