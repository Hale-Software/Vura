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

#pragma once

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>


class QIcon;
class QFileDialog;
class QFileInfo;
class QMessageBox;
class QSettings;
class QDebug;

class SystemTrayWidget : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayWidget(QWidget *parent = nullptr);

    void setVisibility(bool visible);

signals:
    void clicked();
    void hiding(bool hiding);
    void stop();
    void record();
    void playbackRateFaster();
    void playbackRateFasterFine();
    void playbackRateNormal();
    void playbackRateSlower();
    void playbackRateSlowerFine();
    void volumeUp();
    void volumeDown();
    void toggleMute();
    void openFile();
    void togglePlayPause();
    void nextVideo();
    void previousVideo();
    void exit();

private slots:
    void systemTray_Clicked(ActivationReason reason);
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
    void createContextMenu();
    void setActionEnables() const;

    QSystemTrayIcon *m_systemTrayIcon = nullptr;
    QMenu *m_speedMenu = nullptr;
    QAction *m_toggleShow = nullptr;
    QAction *m_playAction = nullptr;
    QAction *m_stopAction = nullptr;
    QAction *m_nextAction = nullptr;
    QAction *m_previousAction = nullptr;
    QAction *m_recordAction = nullptr;
    QAction *m_fasterAction = nullptr;
    QAction *m_fasterFineAction = nullptr;
    QAction *m_normalAction = nullptr;
    QAction *m_slowerFineAction = nullptr;
    QAction *m_slowerAction = nullptr;
    QAction *m_increaseVolumeAction = nullptr;
    QAction *m_decreaseVolumeAction = nullptr;
    QAction *m_muteAction = nullptr;
    QAction *m_openFileAction = nullptr;
    QAction *m_quitAction = nullptr;

    bool m_showing = true;

};
