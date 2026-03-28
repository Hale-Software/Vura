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

#include "mainwidget.h"
#include "dumpsender.h"

#include <QApplication>
#include <QFileInfo>
#include <QHostInfo>
#include <QNetworkProxyFactory>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    if (application.arguments().count() > 1) {
        const QString dumpPath = QApplication::arguments().at(1);
        if (!QFileInfo(dumpPath).exists())
            qWarning("dumpPath not exist");
    }
    else {
        qWarning("no dumpPath specified");
    }

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QHostInfo hostInfo = QHostInfo::fromName("crashes.qt.io");

    QUrl submitUrl("http://127.0.0.1/submit");
    DumpSender dumpSender(submitUrl);

    MainWidget mainWindow;

    mainWindow.setProgressbarMaximum(dumpSender.dumperSize());

    QObject::connect(&mainWindow, &MainWidget::restartCrashedApplication,
                     &dumpSender, &DumpSender::restartCrashedApplication);
    QObject::connect(&mainWindow, &MainWidget::restartCrashedApplicationAndSendDump,
                     &dumpSender, &DumpSender::restartCrashedApplicationAndSendDump);
    QObject::connect(&mainWindow, &MainWidget::sendDump,
                     &dumpSender, &DumpSender::sendDumpAndQuit);
    QObject::connect(&mainWindow, &MainWidget::commentChanged,
                     &dumpSender, &DumpSender::setCommentText);
    QObject::connect(&mainWindow, &MainWidget::emailAdressChanged,
                     &dumpSender, &DumpSender::setEmailAddress);
    QObject::connect(&dumpSender, &DumpSender::uploadProgress,
                     &mainWindow, &MainWidget::updateProgressBar);

    mainWindow.show();
    return application.exec();
}
