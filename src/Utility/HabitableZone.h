//
// Created by neotron on 2018-01-07.
//


#pragma once

#include <QtCore/QMap>

struct BCEntry {
    int K;
    double BC;
};
struct HabZone {
    int inner;
    int outer;
};

class HabitableZone {
public:
    static HabZone habitableZone(const QString &starClass, int temp, double absoluteMagnitude);

private:
    static QMap<QString,QList<BCEntry>> s_lookup;
};

