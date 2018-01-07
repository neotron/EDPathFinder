//
// Created by neotron on 2018-01-06.
//

#include "MaterialLogMFDPage.h"
#ifdef Q_OS_WIN
#include <deps/EDJournalQT/src/Event.h>

#endif

MaterialLogMFDPage::MaterialLogMFDPage(QObject *parent) : MFDPage(parent) {}

void MaterialLogMFDPage::updateWithEvent(const Event &ev) {
    if(ev.type() == EventTypeMaterialCollected) {

    }
}
//1234567890123456
//1.82e6K M0.62 R0
bool MaterialLogMFDPage::scrollWheelclick() {
    return MFDPage::scrollWheelclick();
}
