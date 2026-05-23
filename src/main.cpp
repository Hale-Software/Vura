/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <QApplication>
#include <QMessageBox>
#include <QDebug>

//#include <QBreakpadHandler.h>

#include "widgets/mainwindow.h"
#include <util/singleinstance.h>
#include <constants.h>

#include <config.h>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Set application information
    QCoreApplication::setApplicationName(VURA_PRODUCT_NAME);
    QCoreApplication::setOrganizationName(VURA_COMPANY_NAME);
    QCoreApplication::setApplicationVersion(VURA_VERSION_CANONICAL);


    // Set Windows Crash Handler
#ifdef Q_OS_WIN
    bool winCrashHandler = true;

    QString defaultCrashFileLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/crashes";
    if (VURA_BUILD_TYPE == "Debug") {
        defaultCrashFileLocation = constants::ApplicationDebugFolder + "/crashes";
    }

    if (!QDir(defaultCrashFileLocation).exists()) {
        if (!QDir().mkpath(defaultCrashFileLocation)) {
            QMessageBox::critical(nullptr, "Vura Error", "Failed to configure Windows crash handler directory.");
            winCrashHandler = false;
        }
    }

//    if (winCrashHandler) {
//        QBreakpadInstance.setDumpPath(defaultCrashFileLocation);

//    } else {
//        qCritical() << "Failed to initialize the Windows Crash Handler";
//    }

#endif


    // Prevent many instances of the app from launching.
    // hasPrevious() also forwards any path argument to the running instance
    // before returning, so the running window will open it automatically.
    const QString instanceName = "com.hale-software.vura";
    SingleInstance instance;
    if (SingleInstance::hasPrevious(instanceName, argc, argv)) {
        return EXIT_SUCCESS;
    }

    instance.listen(instanceName);

    // Create and show the main window.
    MainWindow mainWindow;
    mainWindow.setWindowTitle(QString::fromUtf8(VURA_PRODUCT_NAME) + " " + QString::fromUtf8(VURA_VERSION_STRING));
    mainWindow.show();

    // Handle path arguments on the initial launch (same logic as before).
    if (argc > 2) {
        const QString arg1 = QString::fromLocal8Bit(argv[1]);
        const QString arg2 = QString::fromLocal8Bit(argv[2]);

        QFileInfo info(arg2);
        if (info.isFile()) {
            mainWindow.addFileToPlaylistContextMenu(arg2);
        } else if (info.isDir()) {
            mainWindow.addFolderToPlaylistContextMenu(arg2);
        }

    } else if (argc > 1) {
        const QString pathName = QString::fromUtf8(argv[1]);
        if (pathName.isEmpty()) {
            QMessageBox::critical(nullptr, "Vura Error", "File requested is empty.");
        } else {
            mainWindow.openFileContextMenu(pathName);
        }
    }

    // When a second instance is launched, bring this window to the front …
    QObject::connect(&instance, &SingleInstance::newInstance,
                     &mainWindow, [&]() { mainWindow.setMainWindowVisibility(true); });

    // … and open whatever file or folder it was asked to open.
    QObject::connect(&instance, &SingleInstance::openPathRequested,
                     &mainWindow, [&](const QString &path) {
        mainWindow.setMainWindowVisibility(true);
        QFileInfo info(path);
        if (info.isFile()) {
            mainWindow.openFileContextMenu(path);
        } else if (info.isDir()) {
            mainWindow.openFolderContextMenu(path);
        }
    });

    return QApplication::exec();
}
