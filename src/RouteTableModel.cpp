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


#include "RouteTableModel.h"
#include "TSPWorker.h"

RouteTableModel::RouteTableModel(QObject *parent, const RouteResult &result) : QAbstractTableModel(parent),
                                                                               _result(result) {}

int RouteTableModel::rowCount(const QModelIndex &) const {
    return (int) _result.route().size();
}

int RouteTableModel::columnCount(const QModelIndex &) const {
    return (int) _result.route()[0].size();
}

QVariant RouteTableModel::data(const QModelIndex &index, int role) const {
    auto col = (size_t)index.column();
    auto row = (size_t)index.row();
    auto route = _result.route();
    if(row < route.size() && col < route[row].size()) {
        switch(role) {
            case Qt::DisplayRole:
                return route[row][col];
            case Qt::TextAlignmentRole:
                return col < 3 ? Qt::AlignLeft : Qt::AlignRight;
            default:
                return QVariant();
        }
    }
    return QVariant();
}

QVariant RouteTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
                case 0: return "System";
                case 1: return "Planet";
                case 2: return "Settlement";
                case 3: return "Distance";
                case 4: return "Total Distance";
                default: return "";
            }
        }
    }
    return QVariant();
}


