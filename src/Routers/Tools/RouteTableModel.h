//
//  Copyright (C) 2016-2017  David Hedbor <neotron@gmail.com>
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

#include <QAbstractTableModel>
#include "System.h"
#include "TSPWorker.h"
class QTableView;

class RouteTableModel : public QAbstractTableModel {
public:
    enum ResultType {
        ResultTypeSettlement,
        ResultTypeSystemsOnly,
        ResultTypeValuableSystems,
    };


    RouteTableModel(QObject *parent, const RouteResult &result);


    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;


    const RouteResult &result() const {
        return _result;
    }

    QString lastDistance(size_t row) const;
    QString totalDistance(size_t row) const;

    void setResultType(ResultType resultType) {
        _resultType = resultType;
    }

    ResultType resultType() const;

    static void exportTableViewToCSV(QTableView *table);
    static void exportTableViewToTabNewline(QTableView *table);
    static void exportTableView(QTableView *table, const QString &fileName, bool isCSV);

private:
    RouteResult _result;
    ResultType _resultType;
};
