/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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
#include <QDebug>

#include "widgets/mainwindow.h"
#include <util/singleinstance.h>

#include <config.h>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Set application information
    QCoreApplication::setApplicationName(VURA_PRODUCT_NAME);
    QCoreApplication::setOrganizationName(VURA_COMPANY_NAME);
    QCoreApplication::setApplicationVersion(VURA_VERSION_CANONICAL);

    // Prevent many instances of the app to be launched
    QString name = "com.hale-software.vura";
    SingleInstance instance;
    if (SingleInstance::hasPrevious(name, argc, argv)) {
        return EXIT_SUCCESS;
    }

    instance.listen(name);

    // Create and Show the app
    MainWindow mainWindow;
    mainWindow.setWindowTitle(QString::fromUtf8(VURA_PRODUCT_NAME) + " " + QString::fromUtf8(VURA_VERSION_STRING));
    mainWindow.show();

    if (argc > 2) {
        QString pathParam = QString::fromUtf8(argv[2]);

        QFileInfo pathParamInfo(pathParam);
        if (pathParamInfo.isFile()) {

            if (QString::fromLocal8Bit(argv[1]) == "playlist") {
                mainWindow.addFileToPlaylistContextMenu(pathParam);

            } else {
                mainWindow.addFileToPlaylistContextMenu(pathParam);
            }

        } else if (pathParamInfo.isDir()) {
            if (QString::fromLocal8Bit(argv[1]) == "playlist") {
                mainWindow.addFolderToPlaylistContextMenu(pathParam);

            } else {
                mainWindow.addFolderToPlaylistContextMenu(pathParam);
            }
        }

    } else if (argc > 1) {
        QString pathName = QString::fromUtf8(argv[1]);
        if (pathName.isEmpty()) {
            QMessageBox::critical(nullptr, "Vura Error", "File requested is empty.");

        } else {
            mainWindow.openFileContextMenu(pathName);

        }
    }

    // Bring the main window to the front
    QObject::connect(&instance, &SingleInstance::newInstance, &mainWindow, [&]() { (&mainWindow)->setMainWindowVisibility(true); });
    QObject::connect(&instance, &SingleInstance::sendParamsToInstance, &mainWindow, [&]() { (&mainWindow)->processOpenParams(argc, argv); });

    return QApplication::exec();
}
