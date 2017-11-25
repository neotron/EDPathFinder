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
#include <QDebug>
#include <QTextToSpeech>
#include <QVoice>
#include "MessageToaster.h"
#include "NotificationMacOS.h"
#ifdef Q_OS_WIN
using namespace WinToastLib;
#endif


MessageToaster::MessageToaster(QObject *parent) : QObject(parent), _speech(new QTextToSpeech(this))
#ifdef Q_OS_WIN
                                                  , _isInitialized(false)
#endif
{
#ifdef Q_OS_WIN
    WinToast::instance()->setAppName(L"EDPathFinder");
    WinToast::instance()->setAppUserModelId(
            WinToast::configureAUMI(L"NeoTron", L"EDPathFinder", L"EDPathFinder", LPROJECT_VERSION));
    if (!WinToast::instance()->initialize()) {
        std::wcout << L"Error, could not initialize the lib!" << std::endl;
        return;
    }
    _isInitialized = true;

#endif
}

MessageToaster &MessageToaster::instance() {
    static MessageToaster *s_instance = nullptr;
    if(!s_instance) {
        s_instance = new MessageToaster(qApp);
    }
    return *s_instance;
}

void MessageToaster::sendMessage(const QString &title, const QString &message) {
    _speech->say(title + " "+ message);
#ifdef Q_OS_MAC
    NotificationMacOS::send(title, message);
#endif
#ifdef Q_OS_WIN
    if(!_isInitialized) {
        return;
    }
    WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text02);
    templ.setTextField(title.toStdWString(), WinToastTemplate::FirstLine);
    templ.setTextField(message.toStdWString(), WinToastTemplate::SecondLine);
    if (!WinToast::instance()->showToast(templ, this)) {
        std::wcout << L"Error: Could not launch your toast notification!" << std::endl;
    }
#endif
}
void MessageToaster::send(const QString &title, const QString &message) {
    MessageToaster::instance().sendMessage(title, message);
}
MessageToaster::~MessageToaster() = default;

#ifdef Q_OS_WIN

void MessageToaster::toastActivated() const {
}

void MessageToaster::toastActivated(int actionIndex) const {
}

void MessageToaster::toastDismissed(IWinToastHandler::WinToastDismissalReason state) const {
}

void MessageToaster::toastFailed() const {
}

#endif
