/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

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

#include <QObject>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>

#include "data/video-markers.h"


class VuraSettings : public QObject
{
    Q_OBJECT
public:
    explicit VuraSettings(QObject *parent = nullptr);
    ~VuraSettings();

    void loadSettings();

    QString locale();
    bool showStatusBarOnStart();
    bool showPlaylistOnStart();
    bool showVideoControlsOnStart();
    bool hashFile();
    int jumpSmall();
    int jumpMedium();
    int jumpLarge();
    int jumpExtraLarge();
    int maxRecentFiles();
    bool hideCursorWhenPlaying();
    int hideCursorTime();
    bool systemTray();
    int mediaChangeNotification();
    bool showVideoControlsWhenFullscreen();
    bool startInMinimalViewMode();
    bool pausePlaybackWhenMinimized();
    bool allowOnlyOneInstance();
    bool oneInstanceFromFileManager();
    int continuePlayback();
    bool pauseOnLastFrameOfVideo();
    double playbackSpeedAdjustment();
    double playbackSpeedFineAdjustment();
    double volumeStep();
    bool durationLabelShowRemainingTime();
    QString theme();
    double jumpToEndPercentage();
    bool setOverrideWindowsHotkeys();
    QString markerFile();

private:
    QString m_locale = "en-US";
    bool m_showStatusBarOnStart = false;
    bool m_showPlaylistOnStart = false;
    bool m_showVideoControlsOnStart = false;
    bool m_hashFile = false;
    int m_jumpSmall = 0;
    int m_jumpMedium = 0;
    int m_jumpLarge = 0;
    int m_jumpExtraLarge = 0;
    int m_maxRecentFiles = 0;
    bool m_hideCursorWhenPlaying = false;
    int m_hideCursorTime = 0;
    bool m_systemTray = true;
    int m_mediaChangeNotification = 1;
    bool m_showVideoControlsWhenFullscreen = false;
    bool m_startInMinimalViewMode = false;
    bool m_pausePlaybackWhenMinimized = false;
    bool m_allowOnlyOneInstance = false;
    bool m_oneInstanceFromFileManager = true;
    int m_continuePlayback = 1;
    bool m_pauseOnLastFrameOfVideo = false;
    double m_playbackSpeedAdjustment = 0.0;
    double m_playbackSpeedFineAdjustment = 0.0;
    double m_volumeStep = 0.10;
    bool m_durationLabelShowRemainingTime = false;
    QString m_theme = "System";
    double m_jumpToEndPercentage = 0.0;
    bool m_setOverrideWindowsHotkeys = false;
    QString m_markerFile = "";

};
