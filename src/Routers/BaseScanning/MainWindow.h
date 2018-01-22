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

#include <QMainWindow>
#include <QMap>
#include "System.h"
#include "CommanderInfo.h"
#include "SystemEntryCoordinateResolver.h"
#include "ui_MainWindow.h"
#include "AbstractBaseWindow.h"
#include "Version.h"

class RouteResult;
class AStarRouter;


class MainWindow : public AbstractBaseWindow<Ui::MainWindow> {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow() override;

    bool loading() const;

protected slots:
    void systemsLoaded(const SystemList &systems);


    void routeCalculated(const RouteResult &route) override;

    void updateFilters() override;

    void onEventGeneric(Journal::Event *event);

    void systemLoadProgress(int progress);
    void systemSortingProgress();

    void openMissionTool() override;
    void openExplorationTool() override;
    void openPreferences() override;
    void openBearingCalculator() override;

private slots:
    void showVersionUpdateDialog(const Version &newVersion);

private:

    void buildLookupMap();

    void loadCompressedData();

    void updateSliderParams(int size);

    const QString makeSettlementKey(const System &system, const Planet &planet, const Settlement &settlement) const;

    const QString makeSettlementKey(const QString &system, const QString &planet, const QString &settlement) const;

    void updateSettlementScanDate(const QString &commander, const QString &key, const QDateTime &timestamp);

    void updateSystemForCommander(const QString &commander);

    int distanceSliderValue() const;

    void restoreSettings();

    QMap<QString, SettlementFlags> _nameToFlagLookup;

    int32         _matchingSettlementCount;

    QMap<QString,QMap<QString,QDateTime>> _settlementDates;

    bool _loading;
    int _lastMaterialCount;

    void saveMainSettings(int32 settlementFlags, const QString &selectedCommander, int32 threatFilter, int32 settlementSizes) const;

    float updateProbabilityLabel() const;
};
