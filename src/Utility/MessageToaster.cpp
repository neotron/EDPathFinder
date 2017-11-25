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

#include "MessageToaster.h"
#include "NotificationMacOS.h"

MessageToaster::MessageToaster(QObject *parent) : QObject(parent) {

}

MessageToaster &MessageToaster::instance() {
    static MessageToaster *s_instance = nullptr;
    if(!s_instance) {
        s_instance = new MessageToaster(qApp);
    }
    return *s_instance;
}

void MessageToaster::send(const QString &title, const QString &message) {
#ifdef Q_OS_MAC
    NotificationMacOS::send(title, message);
#endif
}

