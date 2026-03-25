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

#include <QSystemTrayIcon>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

#include "../constants.h"


class SystemTray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTray(QWidget *parent = nullptr);
    ~SystemTray();

    void show();
    void hide();

signals:
    void clicked();
    void hiding(bool hiding);
    void stop();
    void record();
    void changePlaybackSpeed(double rate);
    void setPlaybackSpeedNormal();
    void changeVolume(double volume);
    void toggleMute();
    void openFiles(const QStringList &fileList, bool localFile = true);
    void togglePlayPause();
    void nextVideo();
    void previousVideo();
    void exit();

private slots:
    void systemTray_Clicked(QSystemTrayIcon::ActivationReason reason);
    void systemTray_Hide();
    void systemTray_Stop();
    void systemTray_Record();
    void systemTray_Faster();
    void systemTray_FasterFine();
    void systemTray_NormalSpeed();
    void systemTray_SlowerFine();
    void systemTray_Slower();
    void systemTray_IncreaseVolume();
    void systemTray_DecreaseVolume();
    void systemTray_ToggleMute();
    void systemTray_OpenFile();
    void systemTray_TogglePlayPause();
    void systemTray_Next();
    void systemTray_Previous();
    void systemTray_Exit();

private:
    QSystemTrayIcon *m_systemTrayIcon = nullptr;
    QAction *m_toggleShow = nullptr;
    bool m_showing = true;

    void createContextMenu();

};
