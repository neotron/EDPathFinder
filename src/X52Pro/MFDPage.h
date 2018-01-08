//
// Created by neotron on 2018-01-06.
//


#pragma once
#include <QObject>

#ifdef Q_OS_WIN
#include "windows.h"

class Event;
class JournalFile;

class MFDPage : public QObject {
    Q_OBJECT
public:

    MFDPage(QObject *parent);

    QString textForLine(size_t line) const;
    bool stepLine(bool up);

    void setLines(const QStringList &lines);
    DWORD numLines() const;

    virtual bool update(const JournalFile &journal, const Event &ev) { return false; };
    virtual bool scrollWheelclick() {
        if(_currentLine > 0) {
            _currentLine = 0;
            return true;
        }
        return false;
    }

protected:
    int _currentLine{};
    QStringList _lines{};
};


#endif
