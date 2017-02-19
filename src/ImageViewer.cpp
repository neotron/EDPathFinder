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


#include <QLabel>
#include <QScrollBar>
#include <QCheckBox>

#include "ImageViewer.h"

#define MIN_SCALE_FACTOR 0.25
#define MAX_SCALE_FACTOR 1.0

ImageViewer::ImageViewer(QWidget *parent) : QScrollArea(parent), _imageLabel(nullptr), _flicker(), _scaleFactor(MAX_SCALE_FACTOR) {
    sharedInitialize();
    _flicker.activateOn(this);
}

void ImageViewer::sharedInitialize() {
    setBackgroundRole(QPalette::Dark);
}

void ImageViewer::zoomIn() {
    scaleImage(1.25);
}

void ImageViewer::zoomOut() {
    scaleImage(0.8);
}

void ImageViewer::fitToWindow() {
    const auto checkBox    = findSibling<QCheckBox *>("fitImageCheckbox");
    bool       fitToWindow = checkBox ? checkBox->isChecked() : true;

    setWidgetResizable(fitToWindow);
    if(!fitToWindow) {
        scaleImage(1.0);
    }

    updateZoomButtonEnabledStates();
}

void ImageViewer::updateZoomButtonEnabledStates() const {
    const auto checkBox    = findSibling<QCheckBox *>("fitImageCheckbox");
    bool       fitToWindow = checkBox ? checkBox->isChecked() : true;

    auto zoomIn = findSibling<QWidget*>("zoomInButton");
    if(zoomIn) {
        zoomIn->setEnabled(!fitToWindow && _scaleFactor < MAX_SCALE_FACTOR);
    }
    auto zoomOut = findSibling<QWidget*>("zoomOutButton");
    if(zoomOut) {
        zoomOut->setEnabled(!fitToWindow && _scaleFactor > MIN_SCALE_FACTOR);
    }
}

void ImageViewer::scaleImage(double factor) {
    Q_ASSERT(_imageLabel->pixmap());
    _scaleFactor = std::max(std::min(MAX_SCALE_FACTOR, _scaleFactor*factor), MIN_SCALE_FACTOR);
    auto newSize = _scaleFactor * _imageLabel->pixmap()->size();
    _imageLabel->resize(newSize);

    adjustScrollBar(horizontalScrollBar(), factor);
    adjustScrollBar(verticalScrollBar(), factor);
    updateZoomButtonEnabledStates();
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}

void ImageViewer::setPixmap(const QPixmap &pixmap) {
    if(!_imageLabel) {
        _imageLabel = findChild<AspectRatioPixmapLabel *>("pixmap");
        _imageLabel->setBackgroundRole(QPalette::Base);
    }
    _imageLabel->setPixmap(pixmap);
    fitToWindow();
    updateGeometry();
}
