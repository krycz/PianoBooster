/****************************************************************************

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

    This file is part of the PianoBooster application

    PianoBooster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PianoBooster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PianoBooster.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/

#include <qtutilities/settingsdialog/qtsettings.h>
#include <qtutilities/resources/resources.h>

#include <QApplication>

#include <cstdio>
#include <cstdlib>

#include "QtWindow.h"
#include "resources/config.h"

int main(int argc, char *argv[]) {
    // instantiate app and apply sensible default settings
    SET_QT_APPLICATION_INFO;
    QGuiApplication::setDesktopFileName(QStringLiteral(PROJECT_NAME));
    auto app = QApplication(argc, argv);
    auto qtSettings = QtUtilities::QtSettings();
    qtSettings.disableNotices();
    qtSettings.apply();

    // print version
    const auto argList = QCoreApplication::arguments();
    for (const auto &arg : argList){
        if (arg == QLatin1String("--version")) {
            fprintf(stdout, "pianobooster " APP_VERSION "\n");
            return EXIT_SUCCESS;
        }
    }

    // show main window and execute app
    auto window = QtWindow();
    window.show();
    const auto ret = app.exec();
    closeLogs();
    return ret;
}
