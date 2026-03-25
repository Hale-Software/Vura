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
    setActionEnables();
}

SystemTray::~SystemTray() {}

void SystemTray::show()
{
    m_systemTrayIcon->show();
    m_showing = true;
    setActionEnables();
}

void SystemTray::hide()
{
    m_systemTrayIcon->hide();
    m_showing = false;
    setActionEnables();
}

void SystemTray::createContextMenu()
{
    m_toggleShow = new QAction(this);
    if (m_showing) {
        m_toggleShow->setText(tr("Hide Vura in taskbar"));
    } else {
        m_toggleShow->setText(tr("Show Vura"));
    }

    m_speedMenu = new QMenu();
    m_playAction = new QAction(this);
    m_stopAction = new QAction(this);
    m_nextAction = new QAction(this);
    m_previousAction = new QAction(this);
    m_recordAction = new QAction(this);
    m_fasterAction = new QAction(this);
    m_fasterFineAction = new QAction(this);
    m_normalAction = new QAction(this);
    m_slowerFineAction = new QAction(this);
    m_slowerAction = new QAction(this);
    m_increaseVolumeAction = new QAction(this);
    m_decreaseVolumeAction = new QAction(this);
    m_muteAction = new QAction(this);
    m_openFileAction = new QAction(this);
    m_quitAction = new QAction(this);

    QMenu *menu = new QMenu();
    menu->addAction(m_toggleShow);
    menu->addSeparator();
    m_playAction = menu->addAction(tr("Play"));
    m_stopAction = menu->addAction(tr("Stop"));
    m_nextAction = menu->addAction(tr("Next"));
    m_previousAction = menu->addAction(tr("Previous"));
    m_recordAction = menu->addAction(tr("Record"));
    menu->addSeparator();

    m_speedMenu = menu->addMenu(tr("Speed"));
    m_fasterAction = m_speedMenu->addAction(tr("Faster"));
    m_fasterFineAction = m_speedMenu->addAction(tr("Faster (fine)"));
    m_normalAction = m_speedMenu->addAction(tr("Normal"));
    m_slowerFineAction = m_speedMenu->addAction(tr("Slower (fine)"));
    m_slowerAction = m_speedMenu->addAction(tr("Slower"));

    menu->addSeparator();
    m_increaseVolumeAction = menu->addAction(tr("Increase Volume"));
    m_decreaseVolumeAction = menu->addAction(tr("Decrease Volume"));
    m_muteAction = menu->addAction(tr("Mute"));
    menu->addSeparator();
    m_openFileAction = menu->addAction(tr("Open File"));
    m_quitAction = menu->addAction(tr("Quit"));

    connect(m_systemTrayIcon, &QSystemTrayIcon::activated, this, &SystemTray::systemTray_Clicked);
    connect(m_toggleShow, &QAction::triggered, this, &SystemTray::systemTray_Hide);
    connect(m_stopAction, &QAction::triggered, this, &SystemTray::systemTray_Stop);
    connect(m_recordAction, &QAction::triggered, this, &SystemTray::systemTray_Record);
    connect(m_fasterAction, &QAction::triggered, this, &SystemTray::systemTray_Faster);
    connect(m_fasterFineAction, &QAction::triggered, this, &SystemTray::systemTray_FasterFine);
    connect(m_normalAction, &QAction::triggered, this, &SystemTray::systemTray_NormalSpeed);
    connect(m_slowerFineAction, &QAction::triggered, this, &SystemTray::systemTray_SlowerFine);
    connect(m_slowerAction, &QAction::triggered, this, &SystemTray::systemTray_Slower);
    connect(m_increaseVolumeAction, &QAction::triggered, this, &SystemTray::systemTray_IncreaseVolume);
    connect(m_decreaseVolumeAction, &QAction::triggered, this, &SystemTray::systemTray_DecreaseVolume);
    connect(m_openFileAction, &QAction::triggered, this, &SystemTray::systemTray_OpenFile);
    connect(m_quitAction, &QAction::triggered, this, &SystemTray::systemTray_Exit);
    connect(m_playAction, &QAction::triggered, this, &SystemTray::systemTray_TogglePlayPause);
    connect(m_nextAction, &QAction::triggered, this, &SystemTray::systemTray_Next);
    connect(m_previousAction, &QAction::triggered, this, &SystemTray::systemTray_Previous);
    connect(m_muteAction, &QAction::triggered, this, &SystemTray::systemTray_ToggleMute);

    m_systemTrayIcon->setContextMenu(menu);
    m_systemTrayIcon->setToolTip(tr("Vura media player"));
}

void SystemTray::setActionEnables()
{
    m_playAction->setEnabled(m_showing);
    m_stopAction->setEnabled(m_showing);
    m_nextAction->setEnabled(m_showing);
    m_previousAction->setEnabled(m_showing);
    m_recordAction->setEnabled(m_showing);
    m_fasterAction->setEnabled(m_showing);
    m_fasterFineAction->setEnabled(m_showing);
    m_normalAction->setEnabled(m_showing);
    m_slowerFineAction->setEnabled(m_showing);
    m_slowerAction->setEnabled(m_showing);
    m_increaseVolumeAction->setEnabled(m_showing);
    m_decreaseVolumeAction->setEnabled(m_showing);
    m_muteAction->setEnabled(m_showing);
    m_openFileAction->setEnabled(m_showing);
    m_speedMenu->setEnabled(m_showing);
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

    setActionEnables();
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
