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
#include <QSettings>
#include <QFileInfo>
#include <QSurfaceFormat>
#include <QDir>
#include <QDebug>

#include <libvura/platform/platform.h>
#include <libvura/exceptions/error-service.h>
#include <libvura/util/single-instance-controller.h>

#include <ui-config.h>
#include "VuraMainWindow.h"


int main(int argc, char *argv[])
{
    CrashHandler::install();

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
        const QString uniqueKey = "Vura.SingleInstance.Gatekeeper.v1";
        SingleInstanceController instanceController(uniqueKey);

        const QSettings settings;
        const bool allowOnlyOneInstance = settings.value("allowOnlyOneInstance", true).toBool();
        if (allowOnlyOneInstance) {
            if (instanceController.checkForExistingInstance(QCoreApplication::arguments())) {
                return 0;
            }
        }

        VuraMainWindow mainWindow;
        mainWindow.setWindowTitle(QString::fromUtf8(VURA_PRODUCT_NAME) + " " + QString::fromUtf8(VURA_VERSION_STRING));
        mainWindow.show();

        int showMaximizedOnStart = settings.value("showMaximizedOnStart", 1).toInt();

        if (showMaximizedOnStart == 2)
            mainWindow.maximized();

        if (allowOnlyOneInstance) {
            QObject::connect(&instanceController, &SingleInstanceController::pathReceived, [&mainWindow](const QString &requestedPath) {
                const QFileInfo checkFile(requestedPath);
                if (checkFile.exists() && checkFile.isFile()) {
                    mainWindow.setWindowState((mainWindow.windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                    mainWindow.raise();
                    mainWindow.activateWindow();
                    mainWindow.openFile(requestedPath);
                } else if (checkFile.isDir()) {
                    mainWindow.setWindowState((mainWindow.windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                    mainWindow.raise();
                    mainWindow.activateWindow();
                    mainWindow.openFolder(requestedPath);
                }
            });
        }

        if (argc == 2) {
            const QString openFileArg = QString::fromUtf8(argv[1]);

            const QFileInfo checkFile(openFileArg);
            if (checkFile.exists() && checkFile.isFile()) {
                if (checkFile.isFile()) {
                    mainWindow.openFile(openFileArg);
                    if (showMaximizedOnStart == 1)
                        mainWindow.maximized();

                } else if (checkFile.isDir()) {
                    mainWindow.openFolder(openFileArg);
                    if (showMaximizedOnStart == 1)
                        mainWindow.maximized();
                }
            }
        } else if (argc == 3) {
            const QString openFileArg = QString::fromUtf8(argv[1]);
            const QString openOptionArg = QString::fromUtf8(argv[2]);

            if (openOptionArg == "--network") {
                mainWindow.openNetworkStream(openFileArg);
                if (showMaximizedOnStart == 1)
                    mainWindow.maximized();
            }
        }

        return QApplication::exec();

    } catch (const std::exception &e) {
        ErrorService::instance().postError({.title = "Fatal Crash", .message = e.what(), .severity = ErrorSeverity::Critical});
        return -1;
    }
}
