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


#include <QApplication>
#include <QDebug>
#include <QSplashScreen>
#include <src/Widgets/SplashScreen.h>
#include "MainWindow.h"
#include "Theme.h"

Q_DECLARE_METATYPE(RouteResult);
Q_DECLARE_METATYPE(SystemList);
Q_DECLARE_METATYPE(Version);

// namespace operations_research

//#define USE_SPLASH
int main(int argc, char *argv[]) {
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QCoreApplication::setOrganizationName("NeoTron Software");
    QCoreApplication::setApplicationName("EDPathFinder");

    qRegisterMetaType<RouteResult>();
    qRegisterMetaType<SystemList>();
    qRegisterMetaType<Version>();
    QApplication a(argc, argv);

    Theme::s_defaultPalette = a.palette();
    Theme::applyTheme();

    QPixmap pixmap(":/iconsplash.png");
    MainWindow mainWindow;
    QIcon icon(pixmap);
    mainWindow.setWindowIcon(icon);
#ifdef USE_SPLASH
    SplashScreen *splash = new SplashScreen(pixmap);
    splash->connectListener(&mainWindow);
    splash->show();
    while(mainWindow.loading()) {
        a.processEvents(QEventLoop::AllEvents, 5);
    }
#endif
    mainWindow.show();
#ifdef USE_SPLASH
    splash->finish(&mainWindow);
    delete splash;
#endif
    return a.exec();
}


