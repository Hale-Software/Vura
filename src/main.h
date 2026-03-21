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

#pragma once

#include <QApplication>
#include <QCoreApplication>
#include <QPointer>
#include <QString>
#include <QMessageBox>
#include <QSettings>

#include <qBreakpad/handler/QBreakpadHandler.h>

#include "constants.h"
#include "widgets/mainwindow.h"
#include "widgets/testwindow.h"


class VuraApp : public QApplication {
    Q_OBJECT

    friend class MainWindow;
    friend class TestWindow;

private:
    QPointer<MainWindow> mainWindow;
    QPointer<TestWindow> testWindow;

    bool useTestWindow = false;

public:
    VuraApp(int& argc, char** argv);

    void AppInit(int argc, char* argv[]);
    void windowsPrintKey();

    bool overrideWindowsHotkeys = true;

public slots:
    void setOverrideWindowsHotkeys(bool value);

private slots:
    void applicationQuiting();

};
