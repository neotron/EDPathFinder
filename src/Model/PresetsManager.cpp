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

#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QMenuBar>
#include <QDialog>

#include "PresetsManager.h"
#include "PresetSelector.h"
PresetEntryList PresetsManager::loadFile(const QString &fileName) {
    PresetEntryList entries;
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        return entries;
    }

    auto jsonData = file.readAll();
    QJsonParseError error;
    auto preset = QJsonDocument::fromJson(jsonData, &error);
    if(error.error != QJsonParseError::NoError) {
        qDebug() << "Parse error: ["<<error.offset <<"]"<< error.errorString();
    }
    if(!preset.isNull() && preset.isArray()) {
        for(const auto obj: preset.array()) {
            if(!obj.isObject()) {
                continue;
            }
            PresetEntry entry(obj.toObject());
            if(entry.isValid()) {
                entries.push_back(entry);
            }
        }
    }
    return entries;
}

const PresetEntryList & PresetsManager::loadPreset(CustomPreset preset) {
    if(!_presets.contains(preset)) {
        loadPresetFile(preset);
    }
    return _presets[preset];
}

void PresetsManager::loadPresetFile(CustomPreset preset) {
    QString fileName;
    switch(preset) {
    case CustomPresetEngineers:
        fileName = "engineers";
        break;
    case CustomPresetGenerationShips:
        fileName = "generation";
        break;
    case CustomPresetINRA:
        fileName = "INRA";
        break;
    case CustomPresetCrashSites:
        fileName = "crashsites";
        break;
    }
    _presets[preset] = loadFile(":/presets/"+fileName+".json");
}

void PresetsManager::addPresetsTo(QMenuBar *menuBar) {
    if(menuBar == nullptr) { return; }
    auto menu = new QMenu("Presets", menuBar);
    auto presetNames = _presetNames.keys();
    presetNames.sort(Qt::CaseInsensitive);
    for(const auto &name: presetNames) {
        auto action = new QAction(name, menu);
        action->connect(action, &QAction::triggered, [=]() {
            auto selector = new PresetSelector(nullptr, loadPreset(_presetNames[name]));
            selector->setModal(true);
            selector->open();
        });
        menu->addAction(action);
    }
    menuBar->addMenu(menu);
}

PresetsManager::PresetsManager() {
    _presetNames["Engineers"] = CustomPresetEngineers;
    _presetNames["Crash Sites"] = CustomPresetCrashSites;
    _presetNames["INRA"] = CustomPresetINRA;
    _presetNames["Generation Ships"] = CustomPresetGenerationShips;
}
