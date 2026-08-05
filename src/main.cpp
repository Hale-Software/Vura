/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

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
#include <QFileInfo>
#include <QSurfaceFormat>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

#include <libvura/platform/platform.h>
#include <libvura/exceptions/error-service.h>
#include <libvura/util/single-instance-controller.h>

#include <ui-config.h>
#include "VuraMainWindow.h"


int main(int argc, char *argv[])
{
    // --- Initialize crash handler ---
    CrashHandler::install();

    // --- Initialize your player main window UI context layer --
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(VURA_PRODUCT_NAME);
    QCoreApplication::setOrganizationName(VURA_COMPANY_NAME);
    QCoreApplication::setApplicationVersion(VURA_VERSION_CANONICAL);

    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    try {

        // --- Single Instance Handling ---
        // Use a completely unique app key identifier for the local socket name
        QString uniqueKey = "Vura.SingleInstance.Gatekeeper.v1";
        SingleInstanceController instanceController(uniqueKey);

        // If another instance exists, it receives the arguments via IPC and we exit instantly
        if (instanceController.checkForExistingInstance(QCoreApplication::arguments())) {
            return 0;
        }

        // --- Create and show the main window ---
        VuraMainWindow mainWindow;
        mainWindow.setWindowTitle(QString::fromUtf8(VURA_PRODUCT_NAME) + " " + QString::fromUtf8(VURA_VERSION_STRING));
        mainWindow.show();

        // Setup IPC slot connection to open files smoothly when incoming signals fire
        QObject::connect(&instanceController, &SingleInstanceController::fileReceived, &mainWindow, [&mainWindow](const QString &filePath) {
            const QFileInfo checkFile(filePath);
            if (checkFile.exists() && checkFile.isFile()) {
                // Bring the primary window to the foreground instantly
                mainWindow.setWindowState((mainWindow.windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                mainWindow.raise();
                mainWindow.activateWindow();

                // Load and play the file inside your pipeline
                // Example: mainWindow.openFile(filePath);
                mainWindow.openFile(filePath);
            }
        });

        if (argc == 2) {
            QString openFileArg = QString::fromUtf8(argv[1]);

            QFileInfo checkFile(openFileArg);
            if (checkFile.exists() && checkFile.isFile()) {
                if (checkFile.isFile()) {
                    mainWindow.openFile(openFileArg);

                } else if (checkFile.isDir()) {
                    mainWindow.openFolder(openFileArg);
                }
            }
        } else if (argc == 3) {
            QString openFileArg = QString::fromUtf8(argv[1]);
            QString openOptionArg = QString::fromUtf8(argv[2]);

            if (openOptionArg == "--network") {
                mainWindow.openNetworkStream(openFileArg);
            }
        }

        return QApplication::exec();

    } catch (const std::exception &e) {
        ErrorService::instance().postError({.title = "Fatal Crash", .message = e.what(), .severity = ErrorSeverity::Critical});
        return -1;
    }
}
