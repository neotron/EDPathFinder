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

#include "WindowMenu.h"
QWidgetList WindowMenu::s_activeWindows = QWidgetList();
QList<WindowMenu*> WindowMenu::s_menus = QList<WindowMenu*>();

WindowMenu::~WindowMenu() {
    s_activeWindows.removeAll(_window);
    s_menus.removeAll(this);
    refreshWindows();
}

void WindowMenu::refreshWindows() const {
    for(auto &m: s_menus) {
        m->createWindowActions();
    }
}

WindowMenu::WindowMenu(QWidget *window, QWidget *parent, bool addClose) : QMenu("Window", parent), _window(window) {
    s_activeWindows.push_back(window);
    s_menus.push_back(this);
    if(addClose) {
        _close = new QAction("Close Window");
        connect(_close, &QAction::triggered, [=]() {
            window->close();
        });
        _close->setShortcuts(QKeySequence::Close);
        addAction(_close);
        addSeparator();
    }
    refreshWindows();
}

void WindowMenu::createWindowActions() {
    for(auto &a: _windowsActions) {
        removeAction(a);
        delete a;
    }
    _windowsActions.clear();
    std::sort(s_activeWindows.begin(), s_activeWindows.end(), [](QWidget *first, QWidget *last) {
        return first->windowTitle() < last->windowTitle();
    });
    for(auto &w: s_activeWindows) {
        QAction *a = new QAction(w->windowTitle(), this);
        if(_window == w) {
            a->setCheckable(true);
            a->setChecked(true);
        }
        connect(a, &QAction::triggered, [=]() {
            w->activateWindow();
            w->raise();
        });
        addAction(a);
        _windowsActions.push_back(a);
    }
}
