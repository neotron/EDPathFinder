//
// Created by David Hedbor on 10/11/16.
//

#ifndef ELITETSP_ROUTETABLEMODEL_H
#define ELITETSP_ROUTETABLEMODEL_H


#include <QAbstractTableModel>
#include "System.h"
#include "TSPWorker.h"

class RouteTableModel : public QAbstractTableModel {
public:
    RouteTableModel(QObject *parent, const RouteResult &result);


    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    RouteResult _result;
};


#endif //ELITETSP_ROUTETABLEMODEL_H
