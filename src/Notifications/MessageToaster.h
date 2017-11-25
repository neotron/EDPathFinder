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

#include <QObject>
#include <QCoreApplication>
#ifdef Q_OS_WIN
#include "buildnumber.h"
#define NOMINMAX
#include "deps/WinToast/src/wintoastlib.h"
#endif
class QTextToSpeech;

class MessageToaster: public QObject
#ifdef Q_OS_WIN
        , protected WinToastLib::IWinToastHandler
#endif
{

public:
    static void send(const QString &title, const QString &message);

    static MessageToaster &instance();

    ~MessageToaster() override;

protected:
#ifdef Q_OS_WIN

    void toastActivated() const override;

    void toastActivated(int actionIndex) const override;

    void toastDismissed(WinToastDismissalReason state) const override;

    void toastFailed() const override;

#endif
private:
    void sendMessage(const QString &title, const QString &message);
    explicit MessageToaster(QObject *parent);
    QTextToSpeech *_speech;

#ifdef Q_OS_WIN

    bool _isInitialized;
#endif
};

