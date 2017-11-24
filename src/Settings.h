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

#define SET_IF_CHECKED(VAL, CHECKBOX, FLAG) do {  if(_ui->CHECKBOX->isChecked()) {  VAL |= FLAG; } } while(false)
#define RESTORE_VALUE(NAME, KEY) _ui->NAME->setValue(Settings::get(KEY, _ui->NAME->value()))
#define RESTORE_CHECKED(NAME, KEY) _ui->NAME->setChecked(Settings::get(KEY, _ui->NAME->isChecked()))
#define SAVE_VALUE(NAME, KEY) Settings::set(KEY, _ui->NAME->value())
#define SAVE_CHECKED(NAME, KEY) Settings::set(KEY, _ui->NAME->isChecked())

class Settings {
public:

    static QString journalPath();
    static void setJournalPath(const QString &path);

    static Theme::Id theme();
    static void setTheme(Theme::Id theme);

    static void setFilterSettings(int32 flags, int32 sizes, int32 threat, const QString &commander);
    static void getFilterSettings(int32 &flags, int32 &sizes, int32 &threat, QString &commander);

    static void set(const QString &key, const QVariant &value);

    static int get(const QString &key, int32 defaultValue);
    static float get(const QString &key, float defaultValue);
    static bool get(const QString &key, bool defaultValue);
};
