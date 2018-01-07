//
// Created by neotron on 2018-01-06.
//

#include "MFDPage.h"
#ifdef Q_OS_WIN
#include <QDebug>

QString MFDPage::textForLine(size_t line) const {
    // Calculate actual line using current offset.
    auto actualLine = _currentLine + line;
    if(actualLine >= _lines.size()) {
        actualLine -= _lines.size();
    }
    // Check for out of bounds
    if(actualLine >= _lines.size()) { return {}; }

    return _lines[static_cast<int>(actualLine)];
}

bool MFDPage::stepLine(bool up) {
    if(_lines.size() <= 3) {
        return false; // No scrolling if it already fits.
    }
    if(up) {
        if(_currentLine == 0) {
            _currentLine = _lines.size() - 1U;
        } else {
            _currentLine--;
        }
    } else {
        _currentLine = (_currentLine + 1)%_lines.size();
    }
    return true;
}

void MFDPage::setLines(const QStringList &lines) {
    _currentLine = 0;
    _lines = lines;
}

DWORD MFDPage::numLines() const {
    return static_cast<DWORD>(_lines.count());
}

MFDPage::MFDPage(QObject *parent) : QObject(parent) {}

#endif

