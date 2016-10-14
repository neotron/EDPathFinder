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

#pragma once

#include <QLineEdit>
#include <QKeyEvent>

class NoBeepLineEdit : public QLineEdit {

public:


    explicit NoBeepLineEdit(QWidget *parent) : QLineEdit(parent) {
    }

    explicit NoBeepLineEdit(const QString &string, QWidget *parent) : QLineEdit(string, parent) {
    }

    virtual ~NoBeepLineEdit() override {
    }

    void keyPressEvent(QKeyEvent *event) override {
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
            event->accept();
            emit editingFinished();
            clearFocus();
        } else {
            QLineEdit::keyPressEvent(event);
        }
    }
};
