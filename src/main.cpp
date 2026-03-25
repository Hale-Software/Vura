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

#include "main.h"

#include <config.h>

VuraApp *m_program;

#ifdef Q_OS_WIN
#include <windows.h>

bool m_83key = false;
bool m_91key = false;
bool m_160key = false;

HHOOK hKeyboardHook;

// Callback function to intercept Windows hotkeys
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (m_program->overrideWindowsHotkeys) {
        if (nCode >= 0) {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;

                if (pKeyStruct->vkCode == 83) {
                    m_83key = true;
                } else if (pKeyStruct->vkCode == 91) {
                    m_91key = true;
                } else if (pKeyStruct->vkCode == 160) {
                    m_160key = true;
                } else {
                    m_83key = false;
                    m_91key = false;
                    m_160key = false;
                }

                if (m_83key) {
                    if (m_91key) {
                        if (m_160key) {
                            m_program->windowsPrintKey();
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

#endif


VuraApp::VuraApp(int &argc, char **argv) : QApplication(argc, argv)
{
    QCoreApplication::setApplicationName(VURA_PRODUCT_NAME);
    QCoreApplication::setOrganizationName(VURA_COMPANY_NAME);
    QCoreApplication::setApplicationVersion(VURA_VERSION_CANONICAL);

    QObject::connect(qApp, &QCoreApplication::aboutToQuit, this, &VuraApp::applicationQuiting);
    connect(mainWindow, &MainWindow::setOverrideWindowsHotkeys, this, &VuraApp::setOverrideWindowsHotkeys);
}

void VuraApp::windowsPrintKey()
{
    mainWindow->takeSnapshot();
}

void VuraApp::AppInit(int argc, char* argv[])
{

    if (useTestWindow) {
        testWindow = new TestWindow();
        testWindow->setAttribute(Qt::WA_DeleteOnClose, true);
        testWindow->show();
    } else {
        mainWindow = new MainWindow();
        mainWindow->setAttribute(Qt::WA_DeleteOnClose, true);
        mainWindow->setWindowTitle(QString::fromUtf8(VURA_PRODUCT_NAME) + " " + QString::fromUtf8(VURA_VERSION_STRING));
        connect(mainWindow, SIGNAL(destroyed()), this, SLOT(quit()));
        mainWindow->show();
        mainWindow->windowHandle()->setScreen(qApp->screens()[0]);

        if (argc > 2) {
            QString pathName = QString::fromUtf8(argv[2]);
            if (pathName.isEmpty()) {
                QMessageBox::critical(nullptr, "Vura Error", "File or folder requested is empty.");

            } else {
                QFileInfo fileInfo(pathName);
                if (!fileInfo.exists()) {
                    QMessageBox::critical(nullptr, "Vura Error", "Requested file or folder does not exist.");

                } else {
                    if (fileInfo.isFile()) {
                        if (QString::fromLocal8Bit(argv[1]) == "open") {
                            mainWindow->openFileContextMenu(pathName);

                        } else if (QString::fromLocal8Bit(argv[1]) == "playlist") {
                            mainWindow->addFileToPlaylistContextMenu(pathName);

                        } else {
                            QMessageBox::critical(nullptr, "Vura Error", "Requested file to open does not specify whether to open or add to playlist.");

                        }
                    } else if (fileInfo.isDir()) {
                        if (QString::fromLocal8Bit(argv[1]) == "open") {
                            mainWindow->openFolderContextMenu(pathName);

                        } else if (QString::fromLocal8Bit(argv[1]) == "playlist") {
                            mainWindow->addFolderToPlaylistContextMenu(pathName);

                        } else {
                            QMessageBox::critical(nullptr, "Vura Error", "Requested folder to open does not specify whether to open or add to playlist.");

                        }
                    } else {
                        QMessageBox::critical(nullptr, "Vura Error", "Requested file or folder to open is neither a file nor a folder.");
                    }
                }
            }
        } else if (argc > 1) {
            QString pathName = QString::fromUtf8(argv[1]);
            if (pathName.isEmpty()) {
                QMessageBox::critical(nullptr, "Vura Error", "File requested is empty.");

            } else {
                mainWindow->openFileContextMenu(pathName);

            }
        }
    }
}

void VuraApp::applicationQuiting()
{

#ifdef Q_OS_WIN
    UnhookWindowsHookEx(hKeyboardHook);
#endif

}

void VuraApp::setOverrideWindowsHotkeys(bool value)
{
    overrideWindowsHotkeys = value;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setCrashHandler()
{

#ifdef Q_OS_WIN
    QString defaultCrashFileLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/crashes";
    if (VURA_BUILD_TYPE == "Debug") {
        defaultCrashFileLocation = constants::ApplicationDebugFolder + "/crashes";
    }

    if (!QDir(defaultCrashFileLocation).exists()) {
        if (!QDir().mkpath(defaultCrashFileLocation)) {
            QMessageBox::critical(nullptr, "Vura Error", "Failed to configure Windows crash handler directory.");
            return;
        }
    }

    QBreakpadInstance.setDumpPath(defaultCrashFileLocation);

#endif

}

static int run_program(int argc, char* argv[])
{
    int ret = -1;
    m_program = new VuraApp(argc, argv);


#ifdef Q_OS_WIN
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
#endif

    m_program->AppInit(argc, argv);
    ret = m_program->exec();

    return ret;
}

int main(int argc, char *argv[])
{
    int ret = run_program(argc, argv);

    return ret;
}
