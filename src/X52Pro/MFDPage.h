//
// Created by neotron on 2018-01-06.
//


#pragma once
#include <QObject>
#ifdef Q_OS_WIN
#include "windows.h"

class Event;

class MFDPage : public QObject {
    Q_OBJECT
public:

    MFDPage(QObject *parent);

    QString textForLine(size_t line) const;
    bool stepLine(bool up);

    void setLines(const QStringList &lines);
    DWORD numLines() const;

    virtual void updateWithEvent(const Event &ev) {};
    virtual bool scrollWheelclick() { return false; }

protected:
    size_t _currentLine{};
    QStringList _lines{};
};


#endif
