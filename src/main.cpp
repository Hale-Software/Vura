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

#include <libvura/logging/logger.h>
#include <libvura/platform/platform.h>
#include <libvura/exceptions/error-service.h>
#include <libvura/util/single-instance-controller.h>
#include <libvura/media-controller.h>
#include <libvura/media-engine/engine-factory.h>

#include <ui-config.h>
#include "VuraMainWindow.h"


constexpr int RESTART_CODE = 0xA1;

class Application : public QApplication
{
public:
    Application(int &argc, char **argv) : QApplication(argc, argv) {}

    void setWindow(VuraMainWindow *window)
    {
        m_window = window;
    }

    void setTheme(const int theme)
    {
        switch (theme) {
            case 0:
                break;

            case 1:
                break;

            case 2:
                break;

            case 3:
                QFile style(QStringLiteral(":/styles/nova.qss"));
                if (style.open(QIODevice::ReadOnly | QIODevice::Text))
                    this->setStyleSheet(QString::fromUtf8(style.readAll()));
                break;
        }
    }

private:
    VuraMainWindow *m_window = nullptr;

};


int main(int argc, char *argv[])
{
    int rc;

    do {
        CrashHandler::install();

        QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

        Application app(argc, argv);
        QCoreApplication::setApplicationName(VURA_PRODUCT_NAME);
        QCoreApplication::setOrganizationName(VURA_COMPANY_NAME);
        QCoreApplication::setApplicationVersion(VURA_VERSION_CANONICAL);

        qInstallMessageHandler(Logger::messageHandler);

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

            QCommandLineParser parser;
            parser.setApplicationDescription(VURA_COMMENTS);

            parser.addHelpOption();
            parser.addVersionOption();

            QCommandLineOption openFileOption(QStringList() << "f" << "file", "Specify the file to open.", "file");
            parser.addOption(openFileOption);

            QCommandLineOption openFolderOption(QStringList() << "folder", "Specify the folder to open.", "path");
            parser.addOption(openFolderOption);

            QCommandLineOption openNetworkOption(QStringList() << "network", "Open a network stream.", "url");
            parser.addOption(openNetworkOption);

            parser.process(app);

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
            } else if (parser.isSet(openFolderOption)) {
                window.openFolder(parser.value(openFolderOption));
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
