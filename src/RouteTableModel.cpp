//
// Created by David Hedbor on 10/11/16.
//

#include "RouteTableModel.h"
#include "TSPWorker.h"

RouteTableModel::RouteTableModel(QObject *parent, const RouteResult &result) : QAbstractTableModel(parent),
                                                                               _result(result) {}

int RouteTableModel::rowCount(const QModelIndex &parent) const {
    return (int) _result.route.size();
}

int RouteTableModel::columnCount(const QModelIndex &parent) const {
    return (int) _result.route[0].size();
}

QVariant RouteTableModel::data(const QModelIndex &index, int role) const {
    switch(role) {
        case Qt::DisplayRole:
            return _result.route[index.row()][index.column()];
        case Qt::TextAlignmentRole:
            return index.column() < 3 ? Qt::AlignLeft : Qt::AlignRight;
        default:
            return QVariant();
    }
}

QVariant RouteTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
                case 0: return "System";
                case 1: return "Planet";
                case 2: return "Settlement";
                case 3: return "Jump Size";
                case 4: return "Distance Travelled";
            }
        }
    }
    return QVariant();
}


