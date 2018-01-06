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

#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include "Settings.h"

// Uncomment to enable debug for settings.
// #define SETTINGS_DEBUG 1

static const char *const kJournalPathKey = "restoreJournalPath";
static const char *const kThemeKey = "theme";

QString Settings::restoreJournalPath() {
    QString journalPath;
#ifdef Q_OS_OSX
    journalPath = QDir::homePath() + "/Library/Application Support/Frontier Developments/Elite Dangerous/";
#else
    journalPath = QDir::homePath()+"/Saved Games/Frontier Developments/Elite Dangerous/";
#endif
    auto settingsPath = QSettings().value(kJournalPathKey, "").toString();
    return settingsPath.isEmpty() ? journalPath : settingsPath;
}

void Settings::saveJournalPath(const QString &path) {
    QSettings().setValue(kJournalPathKey, path);
}

Theme::Id Settings::restoreTheme() {
    int theme = QSettings().value(kThemeKey, Theme::Default).toInt();
    if(theme >= 0 && theme < Theme::Settings) {
        return static_cast<Theme::Id>(theme);
    }
    return Theme::Default;
}

void Settings::saveTheme(Theme::Id theme) {
    QSettings().setValue(kThemeKey, theme);
}

void Settings::saveFilterSettings(int32 flags, int32 sizes, int32 threat, const QString &commander) {
    QSettings settings;
    settings.setValue("settlement/flags", flags);
    settings.setValue("settlement/sizes", sizes);
    settings.setValue("settlement/threat", threat);
    settings.setValue("settlement/commander", commander);
}

void Settings::restoreFilterSettings(int32 &flags, int32 &sizes, int32 &threat, QString &commander) {
    QSettings settings;
    flags = settings.value("settlement/flags", 0).toInt();
    sizes = settings.value("settlement/sizes", SettlementSizeSmall|SettlementSizeMedium|SettlementSizeLarge).toInt();
    threat = settings.value("settlement/threat", ThreatLevelLow).toInt();
    commander = settings.value("settlement/commander", flags).toString();\
}

int Settings::restore(const QString &key, int32 defaultValue) {
    bool isOk = false;
    int value = QSettings().value(key, defaultValue).toInt(&isOk);
#ifdef SETTINGS_DEBUG
    qDebug() <<  "GET"<<key << "=" <<value << "default ="<<defaultValue;
#endif
    return isOk ? value : defaultValue;
}

float Settings::restore(const QString &key, float defaultValue) {
    bool isOk = false;
    float value = QSettings().value(key, defaultValue).toFloat(&isOk);
#ifdef SETTINGS_DEBUG
    qDebug() << "GET"<< key << "=" <<value << "default ="<<defaultValue;
#endif
    return isOk ? value : defaultValue;
}

double Settings::restore(const QString &key, double defaultValue) {
    bool isOk = false;
    double value = QSettings().value(key, defaultValue).toDouble(&isOk);
#ifdef SETTINGS_DEBUG
    qDebug() << "GET"<< key << "=" <<value << "default ="<<defaultValue;
#endif
    return isOk ? value : defaultValue;
}

bool Settings::restore(const QString &key, bool defaultValue) {
    bool value = QSettings().value(key, defaultValue).toBool();
#ifdef SETTINGS_DEBUG
    qDebug() << "GET"<< key << "=" <<value << "default ="<<defaultValue;
#endif
    return value;
}

void Settings::save(const QString &key, const QVariant &value) {
#ifdef SETTINGS_DEBUG
    qDebug() <<  "SET"<<key << "=" <<value;
#endif
    QSettings().setValue(key, value);
}

QString Settings::restore(const QString &key, const QString &defaultValue) {
    auto value = QSettings().value(key, defaultValue).toString();
#ifdef SETTINGS_DEBUG
    qDebug() << "GET"<< key << "=" <<value << "default ="<<defaultValue;
#endif
    return value.isEmpty() ? defaultValue : value;
}

QString Settings::restoreSavePath() {
    auto defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    auto path = restore("file/savePath", defaultPath);
    return QFileInfo(path).exists() ? path : defaultPath;
}

void Settings::saveSavePath(const QString &fileName) {
    save("file/savePath", QFileInfo(fileName).canonicalPath());
}
