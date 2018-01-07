//
// Created by neotron on 2018-01-06.
//

#include "MaterialLogMFDPage.h"
#ifdef Q_OS_WIN
#include <QDebug>
#include "Materials.h"
#include <deps/EDJournalQT/src/JournalFile.h>

#endif

MaterialLogMFDPage::MaterialLogMFDPage(QObject *parent) : MFDPage(parent) {
    updateLines();
}

bool MaterialLogMFDPage::update(const JournalFile &journal, const Event &ev) {
    bool didChange = false;
    switch(ev.type()) {
    case EventTypeMaterialCollected:
        didChange = changeMaterial(ev.string("Name"), ev.integer("Count"));
        break;
    case EventTypeMaterialDiscarded:
        didChange = changeMaterial(ev.string("Name"), -ev.integer("Count"));
        break;
    case EventTypeSynthesis:
    default:
        break;
    }

    if(didChange) {
        updateLines();
    }
    return didChange;
}

//1234567890123456
//1.82e6K M0.62 R0
bool MaterialLogMFDPage::scrollWheelclick() {
    for(auto &id: _materalDeltas.keys()) {
        _materalDeltas[id] = 0;
    }
    updateLines();
    return true;
}

bool MaterialLogMFDPage::changeMaterial(const QString &materialName, int delta) {
    if(!delta) { return false; }

    auto material = Materials::material(materialName);
    if(!material.isValid()) {
        return false;
    }
    _materalDeltas[materialName] += delta;
    _changeOrder.removeAll(materialName);
    _changeOrder.insert(0, materialName);
    return true;
}

void MaterialLogMFDPage::updateLines() {
    _currentLine = 0;
    if(_changeOrder.isEmpty()) {
        // We have nothing yet.
        _lines.append("Material Deltas");
        _lines.append("awaiting changes");
        _lines.append("click to reset");
    } else {
        _lines.clear();
        for(const auto &material: _changeOrder) {

            auto mat = Materials::material(material);
            // 1234567890123456
            // 123456789012 XXX
            // BICO VR ENC XXXX
            // BICO R  RAW XXXX
            // BANN VC MAN XXXX
            // 1234567890123456
            // Raw++++ BnNb 293
            // Man++   Ir     8

            QString line = QString("%1 %2%3 %4")
                    .arg(mat.abbreviation(), -4)
                    .arg(mat.typeName())
                    .arg(mat.rarityName())
                    .arg(_materalDeltas[material], 3);
            _lines.append(line);
        }
    }
}
