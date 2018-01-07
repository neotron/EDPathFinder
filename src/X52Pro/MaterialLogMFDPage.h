//
// Created by neotron on 2018-01-06.
//


#pragma once
#include "MFDPage.h"

#ifdef Q_OS_WIN

class MaterialLogMFDPage: public MFDPage {
public:
    MaterialLogMFDPage(QObject *parent);

    void updateWithEvent(const Event &ev) override;

    bool scrollWheelclick() override;
};

#endif
