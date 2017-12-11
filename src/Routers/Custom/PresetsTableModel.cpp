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

#include "PresetsTableModel.h"

PresetsTableModel::PresetsTableModel(QObject *parent, const PresetEntryList &result)
        : QAbstractTableModel(parent), _stops(result) { }

int PresetsTableModel::rowCount(const QModelIndex &) const {
    return _stops.size();
}

int PresetsTableModel::columnCount(const QModelIndex &) const {
    return 4;
}

QVariant PresetsTableModel::data(const QModelIndex &index, int role) const {
    auto col   = index.column();
    auto row   = index.row();

    switch(role) {
    case Qt::DisplayRole:
        switch(col) {
        case 0: return _stops[row].systemName();
        case 1: return _stops[row].type();
        case 2: return _stops[row].shortDescription();
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        return Qt::AlignLeft;
    default:
        return QVariant();
    }
}

QVariant PresetsTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
            case 0:
                return "System";
            case 1:
                return "Type";
            case 2:
                return "Name";
            default:
                return "";
            }
        }
    }
    return QVariant();
}
