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

#include <QStringList>
#include "Version.h"
#include "buildnumber.h"

Version::Version(): _version(PROJECT_VERSION), _versionInt(0) {
    parseVersion();
}

Version::Version(const QString &version): _version(version), _versionInt(0) {
    parseVersion();
}

void Version::parseVersion() {
    auto components = _version.split(".");
    _versionInt = 0;

    for(int i = 0; i < 4; i++) {
        int val = 0;
        if(i < components.size()) {
            val = components[i].toInt();
        }
        _versionInt += val;
        _versionInt <<= 8;
    }
}
