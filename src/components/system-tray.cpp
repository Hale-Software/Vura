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

#include "system-tray.h"


SystemTray::SystemTray(QWidget *parent) : QSystemTrayIcon(parent)
{
    m_systemTrayIcon = new QSystemTrayIcon(this);
    m_systemTrayIcon->setIcon(QIcon(":/img/images/vura-white.png"));

    createContextMenu();
}

SystemTray::~SystemTray() {}

void SystemTray::show()
{
    m_systemTrayIcon->show();
    m_showing = true;
}

void SystemTray::hide()
{
    m_systemTrayIcon->hide();
    m_showing = false;
}

void SystemTray::createContextMenu()
{
    m_toggleShow = new QAction(this);
    if (m_showing) {
        m_toggleShow->setText(tr("Hide Vura in taskbar"));
    } else {
        m_toggleShow->setText(tr("Show Vura"));
    }

    QMenu *menu = new QMenu();
    menu->addAction(m_toggleShow);
    menu->addSeparator();
    QAction *playAction = menu->addAction(tr("Play"));
    QAction *stopAction = menu->addAction(tr("Stop"));
    QAction *nextAction = menu->addAction(tr("Next"));
    QAction *previousAction = menu->addAction(tr("Previous"));
    QAction *recordAction = menu->addAction(tr("Record"));
    menu->addSeparator();

    QMenu *speedMenu = menu->addMenu(tr("Speed"));
    QAction *fasterAction = speedMenu->addAction(tr("Faster"));
    QAction *fasterFineAction = speedMenu->addAction(tr("Faster (fine)"));
    QAction *normalAction = speedMenu->addAction(tr("Normal"));
    QAction *slowerFineAction = speedMenu->addAction(tr("Slower (fine)"));
    QAction *slowerAction = speedMenu->addAction(tr("Slower"));

    menu->addSeparator();
    QAction *increaseVolumeAction = menu->addAction(tr("Increase Volume"));
    QAction *decreaseVolumeAction = menu->addAction(tr("Decrease Volume"));
    QAction *muteAction = menu->addAction(tr("Mute"));
    menu->addSeparator();
    QAction *openFileAction = menu->addAction(tr("Open File"));
    QAction *quitAction = menu->addAction(tr("Quit"));

    connect(m_systemTrayIcon, &QSystemTrayIcon::activated, this, &SystemTray::systemTray_Clicked);
    connect(m_toggleShow, &QAction::triggered, this, &SystemTray::systemTray_Hide);
    connect(stopAction, &QAction::triggered, this, &SystemTray::systemTray_Stop);
    connect(recordAction, &QAction::triggered, this, &SystemTray::systemTray_Record);
    connect(fasterAction, &QAction::triggered, this, &SystemTray::systemTray_Faster);
    connect(fasterFineAction, &QAction::triggered, this, &SystemTray::systemTray_FasterFine);
    connect(normalAction, &QAction::triggered, this, &SystemTray::systemTray_NormalSpeed);
    connect(slowerFineAction, &QAction::triggered, this, &SystemTray::systemTray_SlowerFine);
    connect(slowerAction, &QAction::triggered, this, &SystemTray::systemTray_Slower);
    connect(increaseVolumeAction, &QAction::triggered, this, &SystemTray::systemTray_IncreaseVolume);
    connect(decreaseVolumeAction, &QAction::triggered, this, &SystemTray::systemTray_DecreaseVolume);
    connect(openFileAction, &QAction::triggered, this, &SystemTray::systemTray_OpenFile);
    connect(quitAction, &QAction::triggered, this, &SystemTray::systemTray_Exit);
    connect(playAction, &QAction::triggered, this, &SystemTray::systemTray_TogglePlayPause);
    connect(nextAction, &QAction::triggered, this, &SystemTray::systemTray_Next);
    connect(previousAction, &QAction::triggered, this, &SystemTray::systemTray_Previous);
    connect(muteAction, &QAction::triggered, this, &SystemTray::systemTray_ToggleMute);

    m_systemTrayIcon->setContextMenu(menu);
    m_systemTrayIcon->setToolTip(tr("Vura media player"));
}

void SystemTray::systemTray_Clicked(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        emit clicked();
    }
}

void SystemTray::systemTray_Hide()
{
    if (m_showing) {
        emit hiding(true);
        m_toggleShow->setText(tr("Show Vura"));
        m_showing = false;

    } else {
        emit hiding(false);
        m_toggleShow->setText(tr("Hide Vura in taskbar"));
        m_showing = true;
    }
}

void SystemTray::systemTray_Stop()
{
    emit stop();
}

void SystemTray::systemTray_Record()
{
    emit record();
}

void SystemTray::systemTray_Faster()
{
    emit changePlaybackSpeed(0.5);
}

void SystemTray::systemTray_FasterFine()
{
    emit changePlaybackSpeed(0.25);
}

void SystemTray::systemTray_NormalSpeed()
{
    emit setPlaybackSpeedNormal();
}

void SystemTray::systemTray_SlowerFine()
{
    emit changePlaybackSpeed(-0.25);
}

void SystemTray::systemTray_Slower()
{
    emit changePlaybackSpeed(-0.5);
}

void SystemTray::systemTray_IncreaseVolume()
{
    emit changeVolume(0.10);
}

void SystemTray::systemTray_DecreaseVolume()
{
    emit changeVolume(-0.10);
}

void SystemTray::systemTray_ToggleMute()
{
    emit toggleMute();
}

void SystemTray::systemTray_OpenFile()
{
    QSettings settings;

    // File filters
    QStringList fileFilters;
    fileFilters << constants::MediaFileExtensions;
    fileFilters << constants::VideoFileExtensions;
    fileFilters << constants::AudioFileExtensions;
    fileFilters << constants::ApplicationFileExtensions;
    fileFilters << constants::PlaylistFileExtensions;
    fileFilters << "All Files (*.*)";

    // Create open file dialog.
    QFileDialog fileDialog;
    fileDialog.setNameFilters(fileFilters);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open File"));
    fileDialog.setDirectory(settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString());

    if (fileDialog.exec() == QDialog::Accepted) {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (!selectedFiles.isEmpty()) {
            QStringList fileList;
            for (auto &url : selectedFiles) {
                fileList.append(url);
            }

            // Set last file directory where file was opened.
            QString lastFileDirectory = selectedFiles.last();
            settings.setValue("lastFileDirectory", QFileInfo(lastFileDirectory).path());

            emit openFiles(fileList);
        }
    }
}

void SystemTray::systemTray_TogglePlayPause()
{
    emit togglePlayPause();
}

void SystemTray::systemTray_Next()
{
    emit nextVideo();
}

void SystemTray::systemTray_Previous()
{
    emit previousVideo();
}

void SystemTray::systemTray_Exit()
{
    QMessageBox::StandardButton confirmationBox;
    confirmationBox = QMessageBox::question(nullptr,
        tr("Exit Application"),
        tr("Are you sure you want to exit?"),
        QMessageBox::Yes | QMessageBox::No);

    if (confirmationBox == QMessageBox::Yes) {
        emit exit();
    }
}
