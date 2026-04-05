#include "vura-settings.h"


VuraSettings::VuraSettings(QObject *parent) : QObject(parent)
{
    loadSettings();
}

VuraSettings::~VuraSettings() {}

void VuraSettings::loadSettings()
{
    QSettings settings;

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

bool VuraSettings::showStatusBarOnStart()
{
    return m_showStatusBarOnStart;
}

bool VuraSettings::showPlaylistOnStart()
{
    return m_showPlaylistOnStart;
}

bool VuraSettings::showVideoControlsOnStart()
{
    return m_showVideoControlsOnStart;
}

bool VuraSettings::hashFile()
{
    return m_hashFile;
}

int VuraSettings::jumpSmall()
{
    return m_jumpSmall;
}

int VuraSettings::jumpMedium()
{
    return m_jumpMedium;
}

int VuraSettings::jumpLarge()
{
    return m_jumpLarge;
}

int VuraSettings::jumpExtraLarge()
{
    return m_jumpExtraLarge;
}

int VuraSettings::maxRecentFiles()
{
    return m_maxRecentFiles;
}

bool VuraSettings::hideCursorWhenPlaying()
{
    return m_hideCursorWhenPlaying;
}

int VuraSettings::hideCursorTime()
{
    return m_hideCursorTime;
}

bool VuraSettings::systemTray()
{
    return m_systemTray;
}

int VuraSettings::mediaChangeNotification()
{
    return m_mediaChangeNotification;
}

bool VuraSettings::showVideoControlsWhenFullscreen()
{
    return m_showVideoControlsWhenFullscreen;
}

bool VuraSettings::startInMinimalViewMode()
{
    return m_startInMinimalViewMode;
}

bool VuraSettings::pausePlaybackWhenMinimized()
{
    return m_pausePlaybackWhenMinimized;
}

bool VuraSettings::allowOnlyOneInstance()
{
    return m_allowOnlyOneInstance;
}

bool VuraSettings::oneInstanceFromFileManager()
{
    return m_oneInstanceFromFileManager;
}

int VuraSettings::continuePlayback()
{
    return m_continuePlayback;
}

bool VuraSettings::pauseOnLastFrameOfVideo()
{
    return m_pauseOnLastFrameOfVideo;
}

double VuraSettings::playbackSpeedAdjustment()
{
    return m_playbackSpeedAdjustment;
}

double VuraSettings::playbackSpeedFineAdjustment()
{
    return m_playbackSpeedFineAdjustment;
}

double VuraSettings::volumeStep()
{
    return m_volumeStep;
}

bool VuraSettings::durationLabelShowRemainingTime()
{
    return m_durationLabelShowRemainingTime;
}

QString VuraSettings::theme()
{
    return m_theme;
}

double VuraSettings::jumpToEndPercentage()
{
    return m_jumpToEndPercentage;
}

bool VuraSettings::setOverrideWindowsHotkeys()
{
    return m_setOverrideWindowsHotkeys;
}
