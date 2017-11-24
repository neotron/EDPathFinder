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

#include "MissionTableModel.h"

MissionTableModel::MissionTableModel(QObject *parent, const QList<Mission> &result)
        : QAbstractTableModel(parent), _missions(result) { }

int MissionTableModel::rowCount(const QModelIndex &) const {
    return _missions.size();
}

int MissionTableModel::columnCount(const QModelIndex &) const {
    return 2;
}

QVariant MissionTableModel::data(const QModelIndex &index, int role) const {
    auto col   = index.column();
    auto row   = index.row();

    if(row < _missions.size() && col < 2) {
        switch(role) {
            case Qt::DisplayRole:
                return col == 0 ? _missions[row]._origin : _missions[row]._destination;
            case Qt::TextAlignmentRole:
                return col < 3 ? Qt::AlignLeft : Qt::AlignRight;
            default:
                return QVariant();
        }
    }
    return QVariant();
}

QVariant MissionTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
                case 0:
                    return "Origin System";
                case 1:
                    return "Desination System";
                default:
                    return "";
            }
        }
    }
    return QVariant();
}
