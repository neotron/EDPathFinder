//
// Created by neotron on 2018-01-06.
//

#pragma once
#include "MFDPage.h"

#ifdef Q_OS_WIN

class ScanMFDPage: public MFDPage {
public:
    explicit ScanMFDPage(QObject *parent);

    bool update(const JournalFile &journal, const Event &ev) override;

    bool scrollWheelclick() override;

    static QString shorthandBodyType(const QString &bodyType);

    static double calculateGravity(const Event &ev);

    static QString atmosphereShort(const QString &atm);

    static QString mediumBodyType(const QString &bodyType);

private:
    int _currentEntry{};
    QList<QStringList> _history{};

    void processScanEvent(const Event &ev);
};

#endif
