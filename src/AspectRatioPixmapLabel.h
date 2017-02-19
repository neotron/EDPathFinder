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

#include <QWidget>
#include <QPixmap>

class AspectRatioPixmapLabel : public QWidget
{
    Q_OBJECT

public:

    virtual QSize sizeHint() const override ;


public:
    explicit AspectRatioPixmapLabel(QWidget *parent = 0);
    const QPixmap* pixmap() const;

public slots:
    void setPixmap(const QPixmap&);

protected:
    virtual void paintEvent(QPaintEvent *) override;

private:
    QPixmap _pixmap;
};
