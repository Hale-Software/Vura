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

#include "vura-settings.h"
#include "constants.h"

#include <vura-config.h>


VuraSettings::VuraSettings(QObject *parent) : QObject(parent)
{
    loadSettings();
}

VuraSettings::~VuraSettings() {}

void VuraSettings::loadSettings()
{
    const QSettings settings;

    if (VURA_BUILD_TYPE == "Debug") {
        m_markerFile = constants::ApplicationDebugFolder + "/global.json";
    } else {
        const QString videoMarkerDataFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/global.json";
        m_markerFile = videoMarkerDataFile;
    }

    if (!QFile::exists(m_markerFile)) {
        qDebug() << "Marker data file doesn't exist. Creating blank marker data file.";
        VideoMarkers::createBlankFile(m_markerFile);
    }

    m_locale = settings.value("language", "en-US").toString();
    m_showStatusBarOnStart = settings.value("showStatusBarOnStart", false).toBool();
    m_showPlaylistOnStart = settings.value("showPlaylistOnStart", false).toBool();
    m_showVideoControlsOnStart = settings.value("showVideoControlsOnStart", false).toBool();
    m_hashFile = settings.value("hashFile", false).toBool();
    m_jumpSmall = settings.value("jumpSmall", 5).toInt();
    m_jumpMedium = settings.value("jumpMedium", 15).toInt();
    m_jumpLarge = settings.value("jumpLarge", 30).toInt();
    m_jumpExtraLarge = settings.value("jumpExtraLarge", 90).toInt();
    m_maxRecentFiles = settings.value("maxRecentFiles", 9).toInt();
    m_hideCursorWhenPlaying = settings.value("hideCursorWhenPlaying", true).toBool();
    m_hideCursorTime = settings.value("hideCursorTime", 2000).toInt();
    m_systemTray = settings.value("systemTray", true).toBool();
    m_mediaChangeNotification = settings.value("mediaChangeNotification", 1).toInt();
    m_showVideoControlsWhenFullscreen = settings.value("showVideoControlsWhenFullscreen", false).toBool();
    m_startInMinimalViewMode = settings.value("startInMinimalViewMode", false).toBool();
    m_pausePlaybackWhenMinimized = settings.value("pausePlaybackWhenMinimized", false).toBool();
    m_allowOnlyOneInstance = settings.value("allowOnlyOneInstance", false).toBool();
    m_oneInstanceFromFileManager = settings.value("oneInstanceFromFileManager", true).toBool();
    m_continuePlayback = settings.value("continuePlayback", 1).toInt();
    m_pauseOnLastFrameOfVideo = settings.value("pauseOnLastFrameOfVideo", false).toBool();
    m_playbackSpeedAdjustment = settings.value("playbackSpeedAdjustment", 0.5).toDouble();
    m_playbackSpeedFineAdjustment = settings.value("playbackSpeedFineAdjustment", 0.25).toDouble();
    m_volumeStep = settings.value("volumeStep", 0.10).toDouble();
    m_theme = settings.value("theme", "System").toString();
    m_setOverrideWindowsHotkeys = settings.value("setOverrideWindowsHotkeys", true).toBool();
    m_jumpToEndPercentage = settings.value("jumpToEndPercentage", 0.05).toDouble();
}

QString VuraSettings::locale()
{
    return m_locale;
}

bool VuraSettings::showStatusBarOnStart() const
{
    return m_showStatusBarOnStart;
}

bool VuraSettings::showPlaylistOnStart() const
{
    return m_showPlaylistOnStart;
}

bool VuraSettings::showVideoControlsOnStart() const
{
    return m_showVideoControlsOnStart;
}

bool VuraSettings::hashFile() const
{
    return m_hashFile;
}

int VuraSettings::jumpSmall() const
{
    return m_jumpSmall;
}

int VuraSettings::jumpMedium() const
{
    return m_jumpMedium;
}

int VuraSettings::jumpLarge() const
{
    return m_jumpLarge;
}

int VuraSettings::jumpExtraLarge() const
{
    return m_jumpExtraLarge;
}

int VuraSettings::maxRecentFiles() const
{
    return m_maxRecentFiles;
}

bool VuraSettings::hideCursorWhenPlaying() const
{
    return m_hideCursorWhenPlaying;
}

int VuraSettings::hideCursorTime() const
{
    return m_hideCursorTime;
}

bool VuraSettings::systemTray() const
{
    return m_systemTray;
}

int VuraSettings::mediaChangeNotification() const
{
    return m_mediaChangeNotification;
}

bool VuraSettings::showVideoControlsWhenFullscreen() const
{
    return m_showVideoControlsWhenFullscreen;
}

bool VuraSettings::startInMinimalViewMode() const
{
    return m_startInMinimalViewMode;
}

bool VuraSettings::pausePlaybackWhenMinimized() const
{
    return m_pausePlaybackWhenMinimized;
}

bool VuraSettings::allowOnlyOneInstance() const
{
    return m_allowOnlyOneInstance;
}

bool VuraSettings::oneInstanceFromFileManager() const
{
    return m_oneInstanceFromFileManager;
}

int VuraSettings::continuePlayback() const
{
    return m_continuePlayback;
}

bool VuraSettings::pauseOnLastFrameOfVideo() const
{
    return m_pauseOnLastFrameOfVideo;
}

double VuraSettings::playbackSpeedAdjustment() const
{
    return m_playbackSpeedAdjustment;
}

double VuraSettings::playbackSpeedFineAdjustment() const
{
    return m_playbackSpeedFineAdjustment;
}

double VuraSettings::volumeStep() const
{
    return m_volumeStep;
}

bool VuraSettings::durationLabelShowRemainingTime() const
{
    return m_durationLabelShowRemainingTime;
}

QString VuraSettings::theme()
{
    return m_theme;
}

double VuraSettings::jumpToEndPercentage() const
{
    return m_jumpToEndPercentage;
}

bool VuraSettings::setOverrideWindowsHotkeys() const
{
    return m_setOverrideWindowsHotkeys;
}

QString VuraSettings::markerFile()
{
    return m_markerFile;
}
