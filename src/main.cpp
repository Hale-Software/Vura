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
#include <QCommandLineParser>
#include <QFileInfo>
#include <QSurfaceFormat>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

#include <libvura/ErrorService.h>
//#include <libvura/util/singleinstance.h>

#include <ui-config.h>

#include "SingleInstanceController.h"
#include "VuraMainWindow.h"


int main(int argc, char *argv[])
{
    // --- Initialize your player main window UI context layer --
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough
    );

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
            QFileInfo checkFile(filePath);
            if (checkFile.exists() && checkFile.isFile()) {
                // Bring the primary window to the foreground instantly
                mainWindow.setWindowState((mainWindow.windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                mainWindow.raise();
                mainWindow.activateWindow();

                // Load and play the file inside your pipeline
                // Example: w.loadVideo(filePath);
                mainWindow.openFile(filePath);
            }
        });

        // --- Command Line Argument Parsing Configuration ---
        QCommandLineParser parser;
        parser.setApplicationDescription(VURA_COMMENTS);
        parser.addHelpOption();
        parser.addVersionOption();

        // Add positional argument for capturing target media files
        parser.addPositionalArgument("file", "The media file path to open on initialization.");

        QCommandLineOption launchInFullscreenOption(QStringList() << "fullscreen", "Launch the media file directly in fullscreen mode.");
        parser.addOption(launchInFullscreenOption);

        QCommandLineOption quitterAfterFinishOption(QStringList() << "quit", "Quit the application after playback or conversion finishes.");
        parser.addOption(quitterAfterFinishOption);

        QCommandLineOption openGLOption(QStringList() << "opengl", "Enable OpenGL rendering for video playback.");
        parser.addOption(openGLOption);

        parser.process(app);
        const QStringList positionalArguments = parser.positionalArguments();

        if (!positionalArguments.isEmpty()) {
            QString targetFilePath = positionalArguments.first();

            // Confirm the file actually exists on local user storage bounds
            QFileInfo checkFile(targetFilePath);
            if (checkFile.exists() && checkFile.isFile()) {
                // Pass the absolute file path into your FFmpeg decoder worker pipeline thread
                // Example: emit w.startVideoPlayback(targetFilePath);
                if (checkFile.isFile()) {
                    mainWindow.openFile(targetFilePath);

                } else if (checkFile.isDir()) {
                    mainWindow.openFolder(targetFilePath);
                }
            }
        }


        return QApplication::exec();

    } catch (const std::exception &e) {
        ErrorService::instance().postError({"Fatal Crash", e.what(), ErrorSeverity::Critical});
        return -1;
    }
}
