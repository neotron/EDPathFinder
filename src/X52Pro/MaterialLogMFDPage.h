//
// Created by neotron on 2018-01-06.
//


#pragma once
#include "MFDPage.h"
#include <QMap>

#ifdef Q_OS_WIN
class MaterialLogMFDPage: public MFDPage {
public:
    explicit MaterialLogMFDPage(QObject *parent);

    bool update(const JournalFile &journal, const Event &ev) override;

    bool scrollWheelclick() override;

private:
    QMap<QString,int> _materalDeltas{};
    QStringList _changeOrder;

    bool changeMaterial(const QString &materialName, int delta);

    void updateLines();
};

#endif
