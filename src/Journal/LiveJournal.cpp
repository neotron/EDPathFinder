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

#include "Settings.h"
#include "LiveJournal.h"

LiveJournal::LiveJournal(QObject *parent) : QObject(parent), _watcher(new JournalWatcher(this)) {
    connect(_watcher, SIGNAL(onEvent(const JournalFile &, const Event &)),
            this, SLOT(handleEvent(const JournalFile &, const Event &)));
}

LiveJournal *LiveJournal::instance() {
    static LiveJournal * s_journal = nullptr;
    if(!s_journal) {
        s_journal = new LiveJournal(qApp);
    }
    return s_journal;
}

void LiveJournal::handleEvent(const JournalFile &file, const Event &event) {
    emit onEvent(file, event);
}

void LiveJournal::startWatching(const QDateTime &newerThanDate) {
    _watcher->watchDirectory(Settings::restoreJournalPath(), newerThanDate);

}

void LiveJournal::journalPathChanged(const QString &from, const QString &to) {
    _watcher->journalPathChanged(from, to);
}

