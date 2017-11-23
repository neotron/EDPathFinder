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
#include "Settings.h"
#include "Theme.h"

static const char *const kJournalPathKey = "journalPath";
static const char *const kThemeKey = "theme";

QString Settings::journalPath() {
    QString journalPath;
#ifdef Q_OS_OSX
    journalPath = QDir::homePath() + "/Library/Application Support/Frontier Developments/Elite Dangerous/";
#else
    journalPath = QDir::homePath()+"/Saved Games/Frontier Developments/Elite Dangerous/";
#endif
    auto settingsPath = QSettings().value(kJournalPathKey, "").toString();
    return settingsPath.isEmpty() ? journalPath : settingsPath;
}

void Settings::setJournalPath(const QString &path) {
    QSettings().setValue(kJournalPathKey, path);
}

Theme::Id Settings::theme() {
    int theme = QSettings().value(kThemeKey, Theme::Default).toInt();
    if(theme >= 0 && theme < Theme::Settings) {
        return static_cast<Theme::Id>(theme);
    }
    return Theme::Default;
}

void Settings::setTheme(Theme::Id theme) {
    QSettings().setValue(kThemeKey, theme);
}

