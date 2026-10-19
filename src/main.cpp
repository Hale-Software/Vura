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
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QSurfaceFormat>
#include <QDir>
#include <QFileOpenEvent>
#include <QUrl>
#include <QFont>
#include <QDebug>

#include <libvura/audio.h>
#include <libvura/logging/logger.h>
#include <libvura/platform/platform.h>
#include <libvura/exceptions/error-service.h>
#include <libvura/util/single-instance-controller.h>
#include <libvura/media-controller.h>
#include <libvura/media-engine/engine-factory.h>

#include "vura.h"
#include <ui-config.h>

constexpr int RESTART_CODE = 0xA1;


int main(int argc, char *argv[])
{
    int rc;

    do {
        CrashHandler::install();

        QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

        VuraApplication app(argc, argv);
        QCoreApplication::setApplicationName(VURA_PRODUCT_NAME);
        QCoreApplication::setOrganizationName(VURA_COMPANY_NAME);
        QCoreApplication::setApplicationVersion(VURA_VERSION_CANONICAL);

        qInstallMessageHandler(Logger::messageHandler);
        logDeviceInfo();
        logDefaultAudioDeviceSpecs();
        qCInfo(Core) << "==== Startup complete ===============================================";

        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setDepthBufferSize(24);
        QSurfaceFormat::setDefaultFormat(format);

        try {
            QCommandLineParser parser;
            parser.setApplicationDescription(VURA_COMMENTS);

            parser.addHelpOption();
            parser.addVersionOption();
            QCommandLineOption openFileOption(QStringList() << "f" << "file", "Specify the file to open.", "file");
            parser.addOption(openFileOption);
            QCommandLineOption openDirectoryOption(QStringList() << "d" << "dir", "Specify the directory to open.", "path");
            parser.addOption(openDirectoryOption);
            QCommandLineOption openNetworkOption(QStringList() << "n" << "network", "Open a network stream.", "url");
            parser.addOption(openNetworkOption);
            parser.addPositionalArgument("paths", "Files or directories to open.", "[paths...]");
            parser.process(app);

            QStringList forwardPaths;
            const auto absPath = [](const QString &p) {
                return QFileInfo(p).absoluteFilePath();
            };
            if (parser.isSet(openFileOption))
                forwardPaths << absPath(parser.value(openFileOption));
            if (parser.isSet(openDirectoryOption))
                forwardPaths << absPath(parser.value(openDirectoryOption));
            for (const QString &p : parser.positionalArguments())
                forwardPaths << absPath(p);

            const QString uniqueKey = "Vura.SingleInstance.Gatekeeper.v1";
            SingleInstanceController instanceController(uniqueKey);

            const QSettings settings;
            const bool allowOnlyOneInstance = settings.value("allowOnlyOneInstance", true).toBool();
            if (allowOnlyOneInstance && instanceController.checkForExistingInstance(forwardPaths))
                return 0;


            MediaController controller;

            QString backendId = settings.value("backend").toString();
            if (!backendId.isEmpty()) {
                bool known = false;
                const media::Backend backend = media::backendFromId(backendId, &known);
                if (known)
                    controller.setBackend(backend);
            }

            QObject::connect(&controller, &MediaController::backendChanged, &controller, [](media::Backend backend) {
                QSettings().setValue("backend", media::idForBackend(backend));
            });

            VuraMainWindow window(&controller);
            app.setWindow(&window);

            int theme = settings.value("theme", 0).toInt();
            app.setTheme(theme);


            //VuraMainWindow mainWindow;
            window.setWindowTitle(QString::fromUtf8(VURA_PRODUCT_NAME) + " " + QString::fromUtf8(VURA_VERSION_STRING));
            window.show();

            int showMaximizedOnStart = settings.value("showMaximizedOnStart", 1).toInt();

            if (showMaximizedOnStart == 2)
                window.maximized();

            if (allowOnlyOneInstance) {
                QObject::connect(&instanceController, &SingleInstanceController::pathReceived, [&window](const QString &requestedPath) {
                    const QFileInfo checkFile(requestedPath);
                    if (checkFile.exists() && checkFile.isFile()) {
                        window.setWindowState((window.windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                        window.raise();
                        window.activateWindow();
                        window.openFile(requestedPath);
                    } else if (checkFile.isDir()) {
                        window.setWindowState((window.windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                        window.raise();
                        window.activateWindow();
                        window.openFolder(requestedPath);
                    }
                });
            }

            if (parser.isSet(openFileOption)) {
                window.openFile(parser.value(openFileOption));
                if (showMaximizedOnStart == 1)
                    window.maximized();
            } else if (parser.isSet(openDirectoryOption)) {
                window.openFolder(parser.value(openDirectoryOption));
                if (showMaximizedOnStart == 1)
                    window.maximized();
            } else if (parser.isSet(openNetworkOption)) {
                window.openNetworkStream(parser.value(openNetworkOption));
                if (showMaximizedOnStart == 1)
                    window.maximized();
            }

            rc = app.exec();

        } catch (const std::exception &e) {
            qFatal() << "Fatal Crash: " << e.what();
            ErrorService::instance().postError({.title = "Fatal Crash", .message = e.what(), .severity = ErrorSeverity::Critical});
            return -1;
        }
    } while (rc == RESTART_CODE);
    return rc;
}
