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

#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include "Theme.h"
#include "Settings.h"


QPalette Theme::s_defaultPalette = QPalette();

void Theme::applyDark() {
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::Base, QColor(15, 15, 15));
    darkPalette.setColor(QPalette::Disabled, QPalette::AlternateBase, QColor(33, 33, 33));
    darkPalette.setColor(QPalette::Disabled, QPalette::ToolTipBase, Qt::gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::ToolTipText, Qt::gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, Qt::gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::Button, QColor(33, 33, 33));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::Link, QColor(22, 110, 198));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(22, 110, 198));

    qApp->setPalette(darkPalette);
    QString styleSheet(qApp->styleSheet());
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #1a427a; border: 1px solid #7f7f7f; }");
}

void Theme::applyDefault() {
    QString theme;
#ifdef Q_OS_MAC
    theme = "Macintosh";
#else
    theme = "Windows";
#endif
    qApp->setStyle(QStyleFactory::create(theme));
    qApp->setPalette(s_defaultPalette);
    qApp->setStyleSheet("");
}

void Theme::applyTheme(Id theme) {
    switch(theme) {
        case Default:
            applyDefault();
            break;
        case Dark:
            applyDark();
            break;
        case Settings:
            applyTheme(Settings::restoreTheme());
            break;
    }
}
