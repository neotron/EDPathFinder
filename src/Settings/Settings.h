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

#pragma once

#include <QString>
#include "Theme.h"
#include "System.h"

#define IS_SET(__FLAGS, __FLAG) (((__FLAGS) & (__FLAG)) == (__FLAG))
#define CHECKBOX(__BOX, __FLAGS, __FLAG) (__BOX)->setChecked(IS_SET(__FLAGS, __FLAG))
#define CHECKNAME(__NAME, __FLAGS, __FLAG) CHECKBOX(_ui->__NAME, __FLAGS, __FLAG)

#define SET_IF_CHECKED(VAL, CHECKBOX, FLAG) do {  if(_ui->CHECKBOX->isChecked()) {  (VAL) |= (FLAG); } } while(false)
#define RESTORE_VALUE(NAME, KEY) _ui->NAME->setValue(Settings::restore(KEY, _ui->NAME->value()))
#define RESTORE_CHECKED(NAME, KEY) _ui->NAME->setChecked(Settings::restore(KEY, _ui->NAME->isChecked()))
#define SAVE_VALUE(NAME, KEY) Settings::save(KEY, _ui->NAME->value())
#define SAVE_CHECKED(NAME, KEY) Settings::save(KEY, _ui->NAME->isChecked())

class Settings {
public:

    static QString restoreJournalPath();
    static void saveJournalPath(const QString &path);

    static Theme::Id restoreTheme();
    static void saveTheme(Theme::Id theme);

    static void saveFilterSettings(int32 flags, int32 sizes, int32 threat, const QString &commander);
    static void restoreFilterSettings(int32 &flags, int32 &sizes, int32 &threat, QString &commander);

    static QString restoreSavePath();
    static void saveSavePath(const QString &fileName);

    static void save(const QString &key, const QVariant &value);
    static int restore(const QString &key, int32 defaultValue);
    static QString restore(const QString &key, QString &defaultValue);
    static float restore(const QString &key, float defaultValue);
    static bool restore(const QString &key, bool defaultValue);
};
