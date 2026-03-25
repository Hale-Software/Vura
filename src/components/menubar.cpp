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

#include "menubar.h"
#include "utility/hotkeys.h"
#include <config.h>


MenuBar::MenuBar(QWidget *parent) : QMenuBar(parent)
{
    refreshSettings();
    createMenus();
    createMenuActions();
    setActionsDefaultProperties();
    createRecentFileActions();
    createAudioDeviceActions();
    createAudioTrackActions();
    createVideoTrackActions();
    createSubtitleTrackActions();
    buildMenus();
    setActionConnections();
    updateRecentFiles();
    setHotkeys();

}

void MenuBar::createMenus()
{
    // Create Menus
    m_fileMenu = new QMenu(tr("File"), this);
    m_viewMenu = new QMenu(tr("View"), this);
    m_playbackMenu = new QMenu(tr("Playback"), this);
    m_markersMenu = new QMenu(tr("Markers"), this);
    m_audioMenu = new QMenu(tr("Audio"), this);
    m_videoMenu = new QMenu(tr("Video"), this);
    m_subtitleMenu = new QMenu(tr("Subtitle"), this);
    m_toolsMenu = new QMenu(tr("Tools"), this);
    m_helpMenu = new QMenu(tr("Help"), this);
    m_fileImportMenu = new QMenu(tr("Import"), this);
    m_fileExportMenu = new QMenu(tr("Export"), this);
    m_openRecentMenu = new QMenu(tr("Open Recent"), this);
    m_audioDeviceMenu = new QMenu(tr("Audio Device"), this);
    m_audioTrackMenu = new QMenu(tr("Audio Track"), this);
    m_jumpBackwardMenu = new QMenu(tr("Jump Backward"), this);
    m_jumpForwardMenu = new QMenu(tr("Jump Forward"), this);
    m_playbackSpeedMenu = new QMenu(tr("Playback Speed"), this);
    m_subtitleTrackMenu = new QMenu(tr("Subtitle Track"), this);
    m_videoTrackMenu = new QMenu(tr("Video Track"), this);
    m_viewMarkersMenu = new QMenu(tr("Toggle Markers"), this);
    m_logFilesMenu = new QMenu(tr("Log Files"), this);
    m_crashReportsMenu = new QMenu(tr("Crash Reports"), this);
    // m_ = new QMenu(tr(""), this);

}

// TODO: Organize code
void MenuBar::createMenuActions()
{
    // Create Menu Actions
    m_volumeIncreaseAction = new QAction(tr("Increase Volume"), this);
    m_volumeDecreaseAction = new QAction(tr("Decrease Volume"), this);
    m_volumeMuteAction = new QAction(tr("Mute"), this);
    m_closeAction = new QAction(tr("Close"), this);
    m_closeAllAction = new QAction(tr("Close All"), this);
    m_emergencyCollapseAction = new QAction(tr("Emergency Collapse"), this);
    m_exitAction = new QAction(tr("Exit"), this);
    m_openFileAction = new QAction(tr("Open File..."), this);
    m_openFolderAction = new QAction(tr("Open Folder..."), this);
    m_openMultipleFilesAction = new QAction(tr("Open Multiple Files..."), this);
    m_openNetworkStreamAction = new QAction(tr("Open Network Stream..."), this);
    m_showPreferencesAction = new QAction(tr("Preferences"), this);
    m_saveAction = new QAction(tr("Save"), this);
    m_saveAsAction = new QAction(tr("Save As..."), this);
    m_savePlaylistAction = new QAction(tr("Save Playlist"), this);
    m_saveACopyAction = new QAction(tr("Save A Copy..."), this);
    m_importProjectAction = new QAction(tr("Project..."), this);
    m_importCaptionsAction = new QAction(tr("Captions..."), this);
    m_importMarkersAction = new QAction(tr("Markers..."), this);
    m_exportCaptionsAction = new QAction(tr("Captions..."), this);
    m_exportClipsAction = new QAction(tr("Clips..."), this);
    m_exportMarkersAction = new QAction(tr("Markers..."), this);
    m_exportMediaAction = new QAction(tr("Media..."), this);
    m_showAboutAction = new QAction(tr("About"), this);
    m_showHelpAction = new QAction(tr("Help"), this);
    m_showFeedbackAction = new QAction(tr("Provide Feedback"), this);
    m_updateAction = new QAction(tr("Check for Updates..."), this);
    m_cumshotMarkerAction = new QAction(tr("Add Cumshot Marker"), this);
    m_cyanMarkerAction = new QAction(tr("Add Cyan Marker"), this);
    m_dialogMarkerAction = new QAction(tr("Add Dialog Marker"), this);
    m_magentaMarkerAction = new QAction(tr("Add Magenta Marker"), this);
    m_markerAction = new QAction(tr("Add Marker"), this);
    m_orangeMarkerAction = new QAction(tr("Add Orange Marker"), this);
    m_sceneMarkerAction = new QAction(tr("Add Scene Transition Marker"), this);
    m_stripMarkerAction = new QAction(tr("Add Strip Marker"), this);
    m_clearInMarkerAction = new QAction(tr("Clear In"), this);
    m_clearInAndOutMarkerAction = new QAction(tr("Clear In and Out"), this);
    m_clearMarkersAction = new QAction(tr("Clear Markers"), this);
    m_clearOutMarkerAction = new QAction(tr("Clear Out"), this);
    m_clearSelectedMarkerAction = new QAction(tr("Clear Selected Marker"), this);
    m_editMarkerAction = new QAction(tr("Edit Selected Marker..."), this);
    m_goToInMarkerAction = new QAction(tr("Go to In"), this);
    m_goToNextMarkerAction = new QAction(tr("Go to Next Marker"), this);
    m_goToOutMarkerAction = new QAction(tr("Go to Out"), this);
    m_goToPreviousMarkerAction = new QAction(tr("Go to Previous Marker"), this);
    m_inMarkerAction = new QAction(tr("Mark In"), this);
    m_outMarkerAction = new QAction(tr("Mark Out"), this);
    m_jumpToTimeAction = new QAction(tr("Jump to Specific Time..."), this);
    m_nextVideoAction = new QAction(tr("Next Video"), this);
    m_nextFrameAction = new QAction(tr("Next Frame"), this);
    m_togglePlayAction = new QAction(tr("Play/Pause"), this);
    m_previousVideoAction = new QAction(tr("Previous Video"), this);
    m_previousFrameAction = new QAction(tr("Previous Frame"), this);
    m_restartVideoAction = new QAction(tr("Restart Video"), this);
    m_jumpBackwardSmallAction = new QAction(tr("Jump Back Small"), this);
    m_jumpBackwardMediumAction = new QAction(tr("Jump Back Medium"), this);
    m_jumpBackwardLargeAction = new QAction(tr("Jump Back Large"), this);
    m_jumpBackwardExtraLargeAction = new QAction(tr("Jump Back Extra Large"), this);
    m_jumpForwardSmallAction = new QAction(tr("Jump Fwd Small"), this);
    m_jumpForwardMediumAction = new QAction(tr("Jump Fwd Medium"), this);
    m_jumpForwardLargeAction = new QAction(tr("Jump Fwd Large"), this);
    m_jumpForwardExtraLargeAction = new QAction(tr("Jump Fwd Extra Large"), this);
    m_playbackSpeedFasterAction = new QAction(tr("Faster"), this);
    m_playbackSpeedFasterFineAction = new QAction(tr("Faster (fine)"), this);
    m_playbackSpeedNormalAction = new QAction(tr("Normal"), this);
    m_playbackSpeedSlowerFineAction = new QAction(tr("Slower (fine)"), this);
    m_playbackSpeedSlowerAction = new QAction(tr("Slower"), this);
    m_createSubclipAction = new QAction(tr("Make Subclip"), this);
    m_streamStashVideoAction = new QAction(tr("Stream Video from Stash..."), this);
    m_testFunctionAction = new QAction(tr("Test Function"), this);
    m_openSubtitleFileAction = new QAction(tr("Open Subtitle File..."), this);
    m_toggleSubtitlesAction = new QAction(tr("Toggle Subtitles"), this);
    m_toggleFullscreenAction = new QAction(tr("Fullscreen"), this);
    m_takeSnapshotAction = new QAction(tr("Take Snapshot\tPrtSc"), this);
    m_showMediaInformationAction = new QAction(tr("Media Information"), this);
    m_showLogFileViewerAction = new QAction(tr("Log Viewer"), this);
    m_togglePlaylistAction = new QAction(tr("Toggle Playlist"), this);
    m_toggleStatusBarAction = new QAction(tr("Toggle Status Bar"), this);
    m_toggleVideoControlsAction = new QAction(tr("Toggle Video Controls"), this);
    m_toggleCumshotMarkersAction = new QAction(tr("Cumshot Markers"), this);
    m_toggleCyanMarkersAction = new QAction(tr("Cyan Markers"), this);
    m_toggleDialogMarkersAction = new QAction(tr("Dialog Markers"), this);
    m_toggleMagentaMarkersAction = new QAction(tr("Magenta Markers"), this);
    m_toggleMarkersAction = new QAction(tr("Markers"), this);
    m_toggleOrangeMarkersAction = new QAction(tr("Orange Markers"), this);
    m_toggleSceneMarkersAction = new QAction(tr("Scene Transition Markers"), this);
    m_toggleStripMarkersAction = new QAction(tr("Strip Markers"), this);
    m_clearRecentFilesAction = new QAction(tr("Clear"), this);

    // File Menu

    // View Menu

    // Playback Menu

    // Markers Menu

    // Audio Menu

    // Video Menu

    // Subtitle Menu

    // Tools Menu

    // Help Menu
    m_showLogFilesAction = new QAction(tr("Show Log Files"), this);
    m_uploadCurrentLogFileAction = new QAction(tr("Upload Current Log File"), this);
    m_uploadPreviousLogFileAction = new QAction(tr("Upload Previous Log File"), this);
    m_viewCurrentLogAction = new QAction(tr("View Current Log"), this);
    m_showCrashReportsAction = new QAction(tr("Show Crash Reports"), this);
    m_uploadPreviousCrashReportAction = new QAction(tr("Upload Previous Crash Report"), this);
    m_checkFileIntegrityAction = new QAction(tr("Check File Integrity"), this);
    m_whatsNewAction = new QAction(tr("What's New"), this);
    m_releaseNotesAction = new QAction(tr("Release Notes"), this);
    // m_ = new QAction(tr(""), this);



    // Create Separators
    m_recentFilesSeparator = new QAction(this);
    m_recentFilesSeparator->setSeparator(true);
    // m_ = new QAction(this);

}

// TODO: Organize code
void MenuBar::setActionsDefaultProperties()
{
    // Set Menu Item Default Properties
    //m_openRecentMenu->setEnabled(false);
    m_closeAction->setEnabled(false);
    m_closeAllAction->setEnabled(false);
    m_fileImportMenu->setEnabled(false);
    m_fileExportMenu->setEnabled(false);
    m_jumpBackwardMenu->setEnabled(false);
    m_jumpForwardMenu->setEnabled(false);
    m_playbackSpeedMenu->setEnabled(false);
    m_saveAction->setEnabled(false);
    m_saveAsAction->setEnabled(false);
    m_savePlaylistAction->setEnabled(false);
    m_saveACopyAction->setEnabled(false);
    m_cumshotMarkerAction->setEnabled(false);
    m_cyanMarkerAction->setEnabled(false);
    m_dialogMarkerAction->setEnabled(false);
    m_magentaMarkerAction->setEnabled(false);
    m_markerAction->setEnabled(false);
    m_orangeMarkerAction->setEnabled(false);
    m_sceneMarkerAction->setEnabled(false);
    m_stripMarkerAction->setEnabled(false);
    m_clearInMarkerAction->setEnabled(false);
    m_clearInAndOutMarkerAction->setEnabled(false);
    m_clearMarkersAction->setEnabled(false);
    m_clearOutMarkerAction->setEnabled(false);
    m_clearSelectedMarkerAction->setEnabled(false);
    m_editMarkerAction->setEnabled(false);
    m_goToInMarkerAction->setEnabled(false);
    m_goToNextMarkerAction->setEnabled(false);
    m_goToOutMarkerAction->setEnabled(false);
    m_goToPreviousMarkerAction->setEnabled(false);
    m_inMarkerAction->setEnabled(false);
    m_outMarkerAction->setEnabled(false);
    m_jumpToTimeAction->setEnabled(false);
    m_nextVideoAction->setEnabled(false);
    m_nextFrameAction->setEnabled(false);
    m_previousVideoAction->setEnabled(false);
    m_previousFrameAction->setEnabled(false);
    m_restartVideoAction->setEnabled(false);
    m_createSubclipAction->setEnabled(false);
    m_toggleSubtitlesAction->setEnabled(false);
    m_takeSnapshotAction->setEnabled(false);

    m_volumeMuteAction->setCheckable(true);
    m_toggleSubtitlesAction->setCheckable(true);
    m_togglePlaylistAction->setCheckable(true);
    m_toggleStatusBarAction->setCheckable(true);
    m_toggleVideoControlsAction->setCheckable(true);
    m_toggleCumshotMarkersAction->setCheckable(true);
    m_toggleCyanMarkersAction->setCheckable(true);
    m_toggleDialogMarkersAction->setCheckable(true);
    m_toggleMagentaMarkersAction->setCheckable(true);
    m_toggleMarkersAction->setCheckable(true);
    m_toggleOrangeMarkersAction->setCheckable(true);
    m_toggleSceneMarkersAction->setCheckable(true);
    m_toggleStripMarkersAction->setCheckable(true);

    m_toggleCumshotMarkersAction->setChecked(true);
    m_toggleCyanMarkersAction->setChecked(true);
    m_toggleDialogMarkersAction->setChecked(true);
    m_toggleMagentaMarkersAction->setChecked(true);
    m_toggleMarkersAction->setChecked(true);
    m_toggleOrangeMarkersAction->setChecked(true);
    m_toggleSceneMarkersAction->setChecked(true);
    m_toggleStripMarkersAction->setChecked(true);
    m_volumeMuteAction->setChecked(false);
    m_toggleSubtitlesAction->setChecked(false);
    m_togglePlaylistAction->setChecked(false);
    m_toggleStatusBarAction->setChecked(false);
    m_toggleVideoControlsAction->setChecked(false);
    // File Menu

    // View Menu

    // Playback Menu

    // Markers Menu

    // Audio Menu

    // Video Menu

    // Subtitle Menu

    // Tools Menu

    // Help Menu

}

void MenuBar::createRecentFileActions()
{
    // Create Recent File Items
    for (int i = 0; i < m_maxRecentFiles; ++i) {
        m_recentFileActions[i] = new QAction(this);
        m_recentFileActions[i]->setVisible(false);
        connect(m_recentFileActions[i], &QAction::triggered, this, &MenuBar::openRecentFile_Clicked);
        m_openRecentMenu->addAction(m_recentFileActions[i]);
    }
    m_recentFilesSeparator->setVisible(false);
    m_openRecentMenu->addAction(m_recentFilesSeparator);
    m_clearRecentFilesAction = new QAction(this);
    m_clearRecentFilesAction->setText("Clear");
    m_clearRecentFilesAction->setVisible(true);
    connect(m_clearRecentFilesAction, &QAction::triggered, this, &MenuBar::clearRecentFiles_Clicked);
    m_openRecentMenu->addAction(m_clearRecentFilesAction);

}

void MenuBar::createAudioDeviceActions()
{
    // Create Audio Output Actions
    for (int i = 0; i < 15; ++i) {
        m_audioOutputActions[i] = new QAction(this);
        m_audioOutputActions[i]->setCheckable(true);
        m_audioOutputActions[i]->setChecked(false);
        m_audioOutputActions[i]->setVisible(false);
        connect(m_audioOutputActions[i], &QAction::triggered, this, &MenuBar::selectAudioOutput_Clicked);
        m_audioDeviceMenu->addAction(m_audioOutputActions[i]);
    }

}

void MenuBar::createAudioTrackActions()
{
    // Create Audio Track Actions
    for (int i = 0; i < 15; ++i) {
        m_audioTrackActions[i] = new QAction(this);
        m_audioTrackActions[i]->setCheckable(true);
        m_audioTrackActions[i]->setChecked(false);
        m_audioTrackActions[i]->setVisible(false);
        connect(m_audioTrackActions[i], &QAction::triggered, this, &MenuBar::selectAudioTrack_Clicked);
        m_audioTrackMenu->addAction(m_audioTrackActions[i]);
    }
    m_audioTrackMenu->setEnabled(false);

}

void MenuBar::createVideoTrackActions()
{
    // Create Video Track Actions
    for (int i = 0; i < 15; ++i) {
        m_videoTrackActions[i] = new QAction(this);
        m_videoTrackActions[i]->setCheckable(true);
        m_videoTrackActions[i]->setChecked(false);
        m_videoTrackActions[i]->setVisible(false);
        connect(m_videoTrackActions[i], &QAction::triggered, this, &MenuBar::selectVideoTrack_Clicked);
        m_videoTrackMenu->addAction(m_videoTrackActions[i]);
    }
    m_videoTrackMenu->setEnabled(false);

}

void MenuBar::createSubtitleTrackActions()
{
    // Create Subtitle Track Actions
    for (int i = 0; i < 15; ++i) {
        m_subtitleTrackActions[i] = new QAction(this);
        m_subtitleTrackActions[i]->setCheckable(true);
        m_subtitleTrackActions[i]->setChecked(false);
        m_subtitleTrackActions[i]->setVisible(false);
        connect(m_subtitleTrackActions[i], &QAction::triggered, this, &MenuBar::selectSubtitleTrack_Clicked);
        m_subtitleTrackMenu->addAction(m_subtitleTrackActions[i]);
    }
    m_subtitleTrackMenu->setEnabled(false);

}

// TODO: Organize code
void MenuBar::buildMenus()
{
    // Build Submenus
    m_fileImportMenu->addAction(m_importProjectAction);
    m_fileImportMenu->addAction(m_importCaptionsAction);
    m_fileImportMenu->addAction(m_importMarkersAction);

    m_fileExportMenu->addAction(m_exportCaptionsAction);
    m_fileExportMenu->addAction(m_exportClipsAction);
    m_fileExportMenu->addAction(m_exportMarkersAction);
    m_fileExportMenu->addAction(m_exportMediaAction);

    m_jumpBackwardMenu->addAction(m_jumpBackwardSmallAction);
    m_jumpBackwardMenu->addAction(m_jumpBackwardMediumAction);
    m_jumpBackwardMenu->addAction(m_jumpBackwardLargeAction);
    m_jumpBackwardMenu->addAction(m_jumpBackwardExtraLargeAction);

    m_jumpForwardMenu->addAction(m_jumpForwardSmallAction);
    m_jumpForwardMenu->addAction(m_jumpForwardMediumAction);
    m_jumpForwardMenu->addAction(m_jumpForwardLargeAction);
    m_jumpForwardMenu->addAction(m_jumpForwardExtraLargeAction);

    m_playbackSpeedMenu->addAction(m_playbackSpeedFasterAction);
    m_playbackSpeedMenu->addAction(m_playbackSpeedFasterFineAction);
    m_playbackSpeedMenu->addAction(m_playbackSpeedNormalAction);
    m_playbackSpeedMenu->addAction(m_playbackSpeedSlowerFineAction);
    m_playbackSpeedMenu->addAction(m_playbackSpeedSlowerAction);

    m_viewMarkersMenu->addAction(m_toggleCumshotMarkersAction);
    m_viewMarkersMenu->addAction(m_toggleCyanMarkersAction);
    m_viewMarkersMenu->addAction(m_toggleDialogMarkersAction);
    m_viewMarkersMenu->addAction(m_toggleMagentaMarkersAction);
    m_viewMarkersMenu->addAction(m_toggleMarkersAction);
    m_viewMarkersMenu->addAction(m_toggleOrangeMarkersAction);
    m_viewMarkersMenu->addAction(m_toggleSceneMarkersAction);
    m_viewMarkersMenu->addAction(m_toggleStripMarkersAction);

    m_logFilesMenu->addAction(m_showLogFilesAction);
    m_logFilesMenu->addAction(m_uploadCurrentLogFileAction);
    m_logFilesMenu->addAction(m_uploadPreviousLogFileAction);
    m_logFilesMenu->addAction(m_viewCurrentLogAction);

    m_crashReportsMenu->addAction(m_showCrashReportsAction);
    m_crashReportsMenu->addAction(m_uploadPreviousCrashReportAction);


    // Build Menus
    m_fileMenu->addAction(m_openFileAction);
    m_fileMenu->addAction(m_openMultipleFilesAction);
    m_fileMenu->addAction(m_openFolderAction);
    m_fileMenu->addAction(m_openNetworkStreamAction);
    m_fileMenu->addMenu(m_openRecentMenu);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_closeAction);
    m_fileMenu->addAction(m_closeAllAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addAction(m_savePlaylistAction);
    m_fileMenu->addAction(m_saveACopyAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addMenu(m_fileImportMenu);
    m_fileMenu->addMenu(m_fileExportMenu);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_emergencyCollapseAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_showPreferencesAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_viewMenu->addAction(m_togglePlaylistAction);
    m_viewMenu->addAction(m_toggleStatusBarAction);
    m_viewMenu->addAction(m_toggleVideoControlsAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addMenu(m_viewMarkersMenu);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_showMediaInformationAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_showLogFileViewerAction);

    m_playbackMenu->addMenu(m_playbackSpeedMenu);
    m_playbackMenu->addSeparator();
    m_playbackMenu->addAction(m_nextFrameAction);
    m_playbackMenu->addAction(m_previousFrameAction);
    m_playbackMenu->addSeparator();
    m_playbackMenu->addMenu(m_jumpForwardMenu);
    m_playbackMenu->addMenu(m_jumpBackwardMenu);
    m_playbackMenu->addAction(m_jumpToTimeAction);
    m_playbackMenu->addSeparator();
    m_playbackMenu->addAction(m_togglePlayAction);
    m_playbackMenu->addAction(m_nextVideoAction);
    m_playbackMenu->addAction(m_previousVideoAction);
    m_playbackMenu->addSeparator();
    m_playbackMenu->addAction(m_restartVideoAction);

    m_audioMenu->addMenu(m_audioDeviceMenu);
    m_audioMenu->addMenu(m_audioTrackMenu);
    m_audioMenu->addSeparator();
    m_audioMenu->addAction(m_volumeIncreaseAction);
    m_audioMenu->addAction(m_volumeDecreaseAction);
    m_audioMenu->addAction(m_volumeMuteAction);

    m_videoMenu->addMenu(m_videoTrackMenu);
    m_videoMenu->addSeparator();
    m_videoMenu->addAction(m_toggleFullscreenAction);
    m_videoMenu->addSeparator();
    m_videoMenu->addAction(m_takeSnapshotAction);

    m_markersMenu->addAction(m_inMarkerAction);
    m_markersMenu->addAction(m_outMarkerAction);
    m_markersMenu->addSeparator();
    m_markersMenu->addAction(m_goToInMarkerAction);
    m_markersMenu->addAction(m_goToOutMarkerAction);
    m_markersMenu->addSeparator();
    m_markersMenu->addAction(m_clearInMarkerAction);
    m_markersMenu->addAction(m_clearOutMarkerAction);
    m_markersMenu->addAction(m_clearInAndOutMarkerAction);
    m_markersMenu->addSeparator();
    m_markersMenu->addAction(m_markerAction);
    m_markersMenu->addAction(m_goToNextMarkerAction);
    m_markersMenu->addAction(m_goToPreviousMarkerAction);
    m_markersMenu->addSeparator();
    m_markersMenu->addAction(m_clearMarkersAction);
    m_markersMenu->addAction(m_clearSelectedMarkerAction);
    m_markersMenu->addSeparator();
    m_markersMenu->addAction(m_editMarkerAction);
    m_markersMenu->addSeparator();
    m_markersMenu->addAction(m_sceneMarkerAction);
    m_markersMenu->addAction(m_stripMarkerAction);
    m_markersMenu->addAction(m_cumshotMarkerAction);
    m_markersMenu->addAction(m_dialogMarkerAction);
    m_markersMenu->addAction(m_cyanMarkerAction);
    m_markersMenu->addAction(m_magentaMarkerAction);
    m_markersMenu->addAction(m_orangeMarkerAction);

    m_toolsMenu->addAction(m_createSubclipAction);
    m_toolsMenu->addAction(m_streamStashVideoAction);
    m_toolsMenu->addSeparator();
    m_toolsMenu->addAction(m_testFunctionAction);

    m_subtitleMenu->addAction(m_openSubtitleFileAction);
    m_subtitleMenu->addMenu(m_subtitleTrackMenu);
    m_subtitleMenu->addSeparator();
    m_subtitleMenu->addAction(m_toggleSubtitlesAction);

    // File Menu

    // View Menu

    // Playback Menu

    // Markers Menu

    // Audio Menu

    // Video Menu

    // Subtitle Menu

    // Tools Menu

    // Help Menu
    m_helpMenu->addAction(m_showHelpAction);
    m_helpMenu->addAction(m_showFeedbackAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addMenu(m_logFilesMenu);
    m_helpMenu->addMenu(m_crashReportsMenu);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_checkFileIntegrityAction);
    m_helpMenu->addAction(m_updateAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_whatsNewAction);
    m_helpMenu->addAction(m_releaseNotesAction);
    m_helpMenu->addAction(m_showAboutAction);


    this->addMenu(m_fileMenu);
    this->addMenu(m_viewMenu);
    this->addMenu(m_playbackMenu);
    this->addMenu(m_markersMenu);
    this->addMenu(m_audioMenu);
    this->addMenu(m_videoMenu);
    this->addMenu(m_subtitleMenu);
    this->addMenu(m_toolsMenu);
    this->addMenu(m_helpMenu);

}

// TODO: Organize code
void MenuBar::setActionConnections()
{
    connect(m_volumeIncreaseAction, &QAction::triggered, this, &MenuBar::increaseVolume_Clicked);
    connect(m_volumeDecreaseAction, &QAction::triggered, this, &MenuBar::decreaseVolume_Clicked);
    connect(m_volumeMuteAction, &QAction::triggered, this, &MenuBar::toggleMute_Clicked);
    connect(m_closeAction, &QAction::triggered, this, &MenuBar::closeFile_Clicked);
    connect(m_closeAllAction, &QAction::triggered, this, &MenuBar::closeAll_Clicked);
    connect(m_emergencyCollapseAction, &QAction::triggered, this, &MenuBar::emergencyCollapse_Clicked);
    connect(m_exitAction, &QAction::triggered, this, &MenuBar::exit_Clicked);
    connect(m_openFileAction, &QAction::triggered, this, &MenuBar::openFile_Clicked);
    connect(m_openFolderAction, &QAction::triggered, this, &MenuBar::openFolder_Clicked);
    connect(m_openMultipleFilesAction, &QAction::triggered, this, &MenuBar::openMultipleFiles_Clicked);
    connect(m_openNetworkStreamAction, &QAction::triggered, this, &MenuBar::openNetworkStream_Clicked);
    connect(m_showPreferencesAction, &QAction::triggered, this, &MenuBar::showPreferences_Clicked);
    connect(m_saveAction, &QAction::triggered, this, &MenuBar::save_Clicked);
    connect(m_saveAsAction, &QAction::triggered, this, &MenuBar::saveAs_Clicked);
    connect(m_savePlaylistAction, &QAction::triggered, this, &MenuBar::savePlaylist_Clicked);
    connect(m_saveACopyAction, &QAction::triggered, this, &MenuBar::saveACopy_Clicked);
    connect(m_importProjectAction, &QAction::triggered, this, &MenuBar::importProject_Clicked);
    connect(m_importCaptionsAction, &QAction::triggered, this, &MenuBar::importCaptions_Clicked);
    connect(m_importMarkersAction, &QAction::triggered, this, &MenuBar::importMarkers_Clicked);
    connect(m_exportCaptionsAction, &QAction::triggered, this, &MenuBar::exportCaptions_Clicked);
    connect(m_exportClipsAction, &QAction::triggered, this, &MenuBar::exportClips_Clicked);
    connect(m_exportMarkersAction, &QAction::triggered, this, &MenuBar::exportMarkers_Clicked);
    connect(m_exportMediaAction, &QAction::triggered, this, &MenuBar::exportMedia_Clicked);
    connect(m_cumshotMarkerAction, &QAction::triggered, this, &MenuBar::addCumshotMarker_Clicked);
    connect(m_cyanMarkerAction, &QAction::triggered, this, &MenuBar::addCyanMarker_Clicked);
    connect(m_dialogMarkerAction, &QAction::triggered, this, &MenuBar::addDialogMarker_Clicked);
    connect(m_magentaMarkerAction, &QAction::triggered, this, &MenuBar::addMagentaMarker_Clicked);
    connect(m_markerAction, &QAction::triggered, this, &MenuBar::addMarker_Clicked);
    connect(m_orangeMarkerAction, &QAction::triggered, this, &MenuBar::addOrangeMarker_Clicked);
    connect(m_sceneMarkerAction, &QAction::triggered, this, &MenuBar::addSceneTransitionMarker_Clicked);
    connect(m_stripMarkerAction, &QAction::triggered, this, &MenuBar::addStripMarker_Clicked);
    connect(m_clearInMarkerAction, &QAction::triggered, this, &MenuBar::clearInMarker_Clicked);
    connect(m_clearInAndOutMarkerAction, &QAction::triggered, this, &MenuBar::clearInAndOutMarker_Clicked);
    connect(m_clearMarkersAction, &QAction::triggered, this, &MenuBar::clearAllMarkers_Clicked);
    connect(m_clearOutMarkerAction, &QAction::triggered, this, &MenuBar::clearOutMarker_Clicked);
    connect(m_clearSelectedMarkerAction, &QAction::triggered, this, &MenuBar::clearSelectedMarker_Clicked);
    connect(m_editMarkerAction, &QAction::triggered, this, &MenuBar::editMarker_Clicked);
    connect(m_goToInMarkerAction, &QAction::triggered, this, &MenuBar::goToInMarker_Clicked);
    connect(m_goToNextMarkerAction, &QAction::triggered, this, &MenuBar::goToNextMarker_Clicked);
    connect(m_goToOutMarkerAction, &QAction::triggered, this, &MenuBar::goToOutMarker_Clicked);
    connect(m_goToPreviousMarkerAction, &QAction::triggered, this, &MenuBar::goToPreviousMarker_Clicked);
    connect(m_inMarkerAction, &QAction::triggered, this, &MenuBar::addInMarker_Clicked);
    connect(m_outMarkerAction, &QAction::triggered, this, &MenuBar::addOutMarker_Clicked);
    connect(m_jumpToTimeAction, &QAction::triggered, this, &MenuBar::jumpToTime_Clicked);
    connect(m_nextVideoAction, &QAction::triggered, this, &MenuBar::nextVideo_Clicked);
    connect(m_nextFrameAction, &QAction::triggered, this, &MenuBar::nextFrame_Clicked);
    connect(m_togglePlayAction, &QAction::triggered, this, &MenuBar::togglePlayPause_Clicked);
    connect(m_previousVideoAction, &QAction::triggered, this, &MenuBar::previousVideo_Clicked);
    connect(m_previousFrameAction, &QAction::triggered, this, &MenuBar::previousFrame_Clicked);
    connect(m_restartVideoAction, &QAction::triggered, this, &MenuBar::restartVideo_Clicked);
    connect(m_jumpBackwardSmallAction, &QAction::triggered, this, &MenuBar::jumpBackwardSmall_Clicked);
    connect(m_jumpBackwardMediumAction, &QAction::triggered, this, &MenuBar::jumpBackwardMedium_Clicked);
    connect(m_jumpBackwardLargeAction, &QAction::triggered, this, &MenuBar::jumpBackwardLarge_Clicked);
    connect(m_jumpBackwardExtraLargeAction, &QAction::triggered, this, &MenuBar::jumpBackwardExtraLarge_Clicked);
    connect(m_jumpForwardSmallAction, &QAction::triggered, this, &MenuBar::jumpForwardSmall_Clicked);
    connect(m_jumpForwardMediumAction, &QAction::triggered, this, &MenuBar::jumpForwardMedium_Clicked);
    connect(m_jumpForwardLargeAction, &QAction::triggered, this, &MenuBar::jumpForwardLarge_Clicked);
    connect(m_jumpForwardExtraLargeAction, &QAction::triggered, this, &MenuBar::jumpForwardExtraLarge_Clicked);
    connect(m_playbackSpeedFasterAction, &QAction::triggered, this, &MenuBar::fasterSpeed_Clicked);
    connect(m_playbackSpeedFasterFineAction, &QAction::triggered, this, &MenuBar::fasterFineSpeed_Clicked);
    connect(m_playbackSpeedNormalAction, &QAction::triggered, this, &MenuBar::normalSpeed_Clicked);
    connect(m_playbackSpeedSlowerFineAction, &QAction::triggered, this, &MenuBar::slowerFineSpeed_Clicked);
    connect(m_playbackSpeedSlowerAction, &QAction::triggered, this, &MenuBar::slowerSpeed_Clicked);
    connect(m_createSubclipAction, &QAction::triggered, this, &MenuBar::createSubclip_Clicked);
    connect(m_streamStashVideoAction, &QAction::triggered, this, &MenuBar::streamVideoFromStash_Clicked);
    connect(m_testFunctionAction, &QAction::triggered, this, &MenuBar::testFunction_Clicked);
    connect(m_openSubtitleFileAction, &QAction::triggered, this, &MenuBar::openSubtitleFile_Clicked);
    connect(m_toggleSubtitlesAction, &QAction::triggered, this, &MenuBar::toggleSubtitles_Clicked);
    connect(m_toggleFullscreenAction, &QAction::triggered, this, &MenuBar::toggleFullscreen_Clicked);
    connect(m_takeSnapshotAction, &QAction::triggered, this, &MenuBar::takeSnapshot_Clicked);
    connect(m_showMediaInformationAction, &QAction::triggered, this, &MenuBar::showMediaInformation_Clicked);
    connect(m_showLogFileViewerAction, &QAction::triggered, this, &MenuBar::showLogFileViewer_Clicked);
    connect(m_togglePlaylistAction, &QAction::triggered, this, &MenuBar::togglePlaylist_Clicked);
    connect(m_toggleStatusBarAction, &QAction::triggered, this, &MenuBar::toggleStatusBar_Clicked);
    connect(m_toggleVideoControlsAction, &QAction::triggered, this, &MenuBar::toggleVideoControls_Clicked);
    connect(m_toggleCumshotMarkersAction, &QAction::triggered, this, &MenuBar::toggleCumshotMarkers_Clicked);
    connect(m_toggleCyanMarkersAction, &QAction::triggered, this, &MenuBar::toggleCyanMarkers_Clicked);
    connect(m_toggleDialogMarkersAction, &QAction::triggered, this, &MenuBar::toggleDialogMarkers_Clicked);
    connect(m_toggleMagentaMarkersAction, &QAction::triggered, this, &MenuBar::toggleMagentaMarkers_Clicked);
    connect(m_toggleMarkersAction, &QAction::triggered, this, &MenuBar::toggleMarkers_Clicked);
    connect(m_toggleOrangeMarkersAction, &QAction::triggered, this, &MenuBar::toggleOrangeMarkers_Clicked);
    connect(m_toggleSceneMarkersAction, &QAction::triggered, this, &MenuBar::toggleSceneTransitionMarkers_Clicked);
    connect(m_toggleStripMarkersAction, &QAction::triggered, this, &MenuBar::toggleStripMarkers_Clicked);
    //connect(, &QAction::triggered, this, &MenuBar::);
    // File Menu

    // View Menu

    // Playback Menu

    // Markers Menu

    // Audio Menu

    // Video Menu

    // Subtitle Menu

    // Tools Menu

    // Help Menu
    connect(m_showAboutAction, &QAction::triggered, this, &MenuBar::showAbout_Clicked);
    connect(m_showHelpAction, &QAction::triggered, this, &MenuBar::showHelp_Clicked);
    connect(m_showFeedbackAction, &QAction::triggered, this, &MenuBar::showFeedback_Clicked);
    connect(m_updateAction, &QAction::triggered, this, &MenuBar::showUpdates_Clicked);
    connect(m_showLogFilesAction, &QAction::triggered, this, &MenuBar::actionShowLogFiles_Clicked);
    connect(m_uploadCurrentLogFileAction, &QAction::triggered, this, &MenuBar::actionUploadCurrentLogFile_Clicked);
    connect(m_uploadPreviousLogFileAction, &QAction::triggered, this, &MenuBar::actionUploadPreviousLogFile_Clicked);
    connect(m_viewCurrentLogAction, &QAction::triggered, this, &MenuBar::actionViewCurrentLog_Clicked);
    connect(m_showCrashReportsAction, &QAction::triggered, this, &MenuBar::actionShowCrashReports_Clicked);
    connect(m_uploadPreviousCrashReportAction, &QAction::triggered, this, &MenuBar::actionUploadPreviousCrashReport_Clicked);
    connect(m_checkFileIntegrityAction, &QAction::triggered, this, &MenuBar::actionCheckFileIntegrity_Clicked);
    connect(m_whatsNewAction, &QAction::triggered, this, &MenuBar::actionWhatsNew_Clicked);
    connect(m_releaseNotesAction, &QAction::triggered, this, &MenuBar::actionReleaseNotes_Clicked);

}

// TODO: Organize code
void MenuBar::setHotkeys()
{
    Hotkeys m_hotkeys;

    if (m_initialised) {
        // Remove current hotkeys for actions
        m_hotkeys.removeMenuItemHotkey(*m_volumeIncreaseAction);
        m_hotkeys.removeMenuItemHotkey(*m_volumeDecreaseAction);
        m_hotkeys.removeMenuItemHotkey(*m_volumeMuteAction);
        m_hotkeys.removeMenuItemHotkey(*m_closeAction);
        m_hotkeys.removeMenuItemHotkey(*m_closeAllAction);
        m_hotkeys.removeMenuItemHotkey(*m_emergencyCollapseAction);
        m_hotkeys.removeMenuItemHotkey(*m_exitAction);
        m_hotkeys.removeMenuItemHotkey(*m_openFileAction);
        m_hotkeys.removeMenuItemHotkey(*m_openFolderAction);
        m_hotkeys.removeMenuItemHotkey(*m_openMultipleFilesAction);
        m_hotkeys.removeMenuItemHotkey(*m_openNetworkStreamAction);
        m_hotkeys.removeMenuItemHotkey(*m_showPreferencesAction);
        m_hotkeys.removeMenuItemHotkey(*m_saveAction);
        m_hotkeys.removeMenuItemHotkey(*m_saveACopyAction);
        m_hotkeys.removeMenuItemHotkey(*m_showAboutAction);
        m_hotkeys.removeMenuItemHotkey(*m_showHelpAction);
        m_hotkeys.removeMenuItemHotkey(*m_cumshotMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_cyanMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_dialogMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_magentaMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_markerAction);
        m_hotkeys.removeMenuItemHotkey(*m_orangeMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_sceneMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_stripMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_clearInMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_clearInAndOutMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_clearMarkersAction);
        m_hotkeys.removeMenuItemHotkey(*m_clearOutMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_clearSelectedMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_editMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_goToInMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_goToNextMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_goToOutMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_goToPreviousMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_inMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_outMarkerAction);
        m_hotkeys.removeMenuItemHotkey(*m_jumpToTimeAction);
        m_hotkeys.removeMenuItemHotkey(*m_nextVideoAction);
        m_hotkeys.removeMenuItemHotkey(*m_nextFrameAction);
        m_hotkeys.removeMenuItemHotkey(*m_togglePlaylistAction);
        m_hotkeys.removeMenuItemHotkey(*m_previousVideoAction);
        m_hotkeys.removeMenuItemHotkey(*m_previousFrameAction);
        m_hotkeys.removeMenuItemHotkey(*m_restartVideoAction);
        m_hotkeys.removeMenuItemHotkey(*m_jumpBackwardSmallAction);
        m_hotkeys.removeMenuItemHotkey(*m_jumpBackwardMediumAction);
        m_hotkeys.removeMenuItemHotkey(*m_jumpBackwardLargeAction);
        m_hotkeys.removeMenuItemHotkey(*m_jumpBackwardExtraLargeAction);
        m_hotkeys.removeMenuItemHotkey(*m_jumpForwardSmallAction);
        m_hotkeys.removeMenuItemHotkey(*m_jumpForwardMediumAction);
        m_hotkeys.removeMenuItemHotkey(*m_jumpForwardLargeAction);
        m_hotkeys.removeMenuItemHotkey(*m_jumpForwardExtraLargeAction);
        m_hotkeys.removeMenuItemHotkey(*m_playbackSpeedFasterAction);
        m_hotkeys.removeMenuItemHotkey(*m_playbackSpeedFasterFineAction);
        m_hotkeys.removeMenuItemHotkey(*m_playbackSpeedNormalAction);
        m_hotkeys.removeMenuItemHotkey(*m_playbackSpeedSlowerFineAction);
        m_hotkeys.removeMenuItemHotkey(*m_playbackSpeedSlowerAction);
        m_hotkeys.removeMenuItemHotkey(*m_createSubclipAction);
        m_hotkeys.removeMenuItemHotkey(*m_streamStashVideoAction);
        m_hotkeys.removeMenuItemHotkey(*m_testFunctionAction);
        m_hotkeys.removeMenuItemHotkey(*m_toggleSubtitlesAction);
        m_hotkeys.removeMenuItemHotkey(*m_toggleFullscreenAction);
        m_hotkeys.removeMenuItemHotkey(*m_takeSnapshotAction);
        m_hotkeys.removeMenuItemHotkey(*m_showMediaInformationAction);
        m_hotkeys.removeMenuItemHotkey(*m_showLogFileViewerAction);
        m_hotkeys.removeMenuItemHotkey(*m_togglePlaylistAction);
        m_hotkeys.removeMenuItemHotkey(*m_toggleStatusBarAction);
        m_hotkeys.removeMenuItemHotkey(*m_toggleVideoControlsAction);
        m_hotkeys.removeMenuItemHotkey(*m_clearRecentFilesAction);
        m_hotkeys.removeMenuItemHotkey(*m_togglePlayAction);
        m_hotkeys.removeMenuItemHotkey(*m_saveAsAction);
        m_hotkeys.removeMenuItemHotkey(*m_savePlaylistAction);
    }

    // Load user hotkeys and set actions
    m_hotkeys.setMenuItemHotkey(*m_volumeIncreaseAction);
    m_hotkeys.setMenuItemHotkey(*m_volumeDecreaseAction);
    m_hotkeys.setMenuItemHotkey(*m_volumeMuteAction);
    m_hotkeys.setMenuItemHotkey(*m_closeAction);
    m_hotkeys.setMenuItemHotkey(*m_closeAllAction);
    m_hotkeys.setMenuItemHotkey(*m_emergencyCollapseAction);
    m_hotkeys.setMenuItemHotkey(*m_exitAction);
    m_hotkeys.setMenuItemHotkey(*m_openFileAction);
    m_hotkeys.setMenuItemHotkey(*m_openFolderAction);
    m_hotkeys.setMenuItemHotkey(*m_openMultipleFilesAction);
    m_hotkeys.setMenuItemHotkey(*m_openNetworkStreamAction);
    m_hotkeys.setMenuItemHotkey(*m_showPreferencesAction);
    m_hotkeys.setMenuItemHotkey(*m_saveAction);
    m_hotkeys.setMenuItemHotkey(*m_saveACopyAction);
    m_hotkeys.setMenuItemHotkey(*m_showAboutAction);
    m_hotkeys.setMenuItemHotkey(*m_showHelpAction);
    m_hotkeys.setMenuItemHotkey(*m_cumshotMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_cyanMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_dialogMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_magentaMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_markerAction);
    m_hotkeys.setMenuItemHotkey(*m_orangeMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_sceneMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_stripMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_clearInMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_clearInAndOutMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_clearMarkersAction);
    m_hotkeys.setMenuItemHotkey(*m_clearOutMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_clearSelectedMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_editMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_goToInMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_goToNextMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_goToOutMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_goToPreviousMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_inMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_outMarkerAction);
    m_hotkeys.setMenuItemHotkey(*m_jumpToTimeAction);
    m_hotkeys.setMenuItemHotkey(*m_nextVideoAction);
    m_hotkeys.setMenuItemHotkey(*m_nextFrameAction);
    m_hotkeys.setMenuItemHotkey(*m_togglePlaylistAction);
    m_hotkeys.setMenuItemHotkey(*m_previousVideoAction);
    m_hotkeys.setMenuItemHotkey(*m_previousFrameAction);
    m_hotkeys.setMenuItemHotkey(*m_restartVideoAction);
    m_hotkeys.setMenuItemHotkey(*m_jumpBackwardSmallAction);
    m_hotkeys.setMenuItemHotkey(*m_jumpBackwardMediumAction);
    m_hotkeys.setMenuItemHotkey(*m_jumpBackwardLargeAction);
    m_hotkeys.setMenuItemHotkey(*m_jumpBackwardExtraLargeAction);
    m_hotkeys.setMenuItemHotkey(*m_jumpForwardSmallAction);
    m_hotkeys.setMenuItemHotkey(*m_jumpForwardMediumAction);
    m_hotkeys.setMenuItemHotkey(*m_jumpForwardLargeAction);
    m_hotkeys.setMenuItemHotkey(*m_jumpForwardExtraLargeAction);
    m_hotkeys.setMenuItemHotkey(*m_playbackSpeedFasterAction);
    m_hotkeys.setMenuItemHotkey(*m_playbackSpeedFasterFineAction);
    m_hotkeys.setMenuItemHotkey(*m_playbackSpeedNormalAction);
    m_hotkeys.setMenuItemHotkey(*m_playbackSpeedSlowerFineAction);
    m_hotkeys.setMenuItemHotkey(*m_playbackSpeedSlowerAction);
    m_hotkeys.setMenuItemHotkey(*m_createSubclipAction);
    m_hotkeys.setMenuItemHotkey(*m_streamStashVideoAction);
    m_hotkeys.setMenuItemHotkey(*m_testFunctionAction);
    m_hotkeys.setMenuItemHotkey(*m_toggleSubtitlesAction);
    m_hotkeys.setMenuItemHotkey(*m_toggleFullscreenAction);
    m_hotkeys.setMenuItemHotkey(*m_takeSnapshotAction);
    m_hotkeys.setMenuItemHotkey(*m_showMediaInformationAction);
    m_hotkeys.setMenuItemHotkey(*m_showLogFileViewerAction);
    m_hotkeys.setMenuItemHotkey(*m_togglePlaylistAction);
    m_hotkeys.setMenuItemHotkey(*m_toggleStatusBarAction);
    m_hotkeys.setMenuItemHotkey(*m_toggleVideoControlsAction);
    m_hotkeys.setMenuItemHotkey(*m_clearRecentFilesAction);
    m_hotkeys.setMenuItemHotkey(*m_togglePlayAction);
    m_hotkeys.setMenuItemHotkey(*m_saveAsAction);
    m_hotkeys.setMenuItemHotkey(*m_savePlaylistAction);

    m_initialised = true;
}

// TODO: Organize code
void MenuBar::refreshMenuItems()
{
    m_closeAction->setEnabled(m_fileLoaded);
    m_closeAllAction->setEnabled(m_fileLoaded);
    m_fileImportMenu->setEnabled(m_fileLoaded);
    m_fileExportMenu->setEnabled(m_fileLoaded);
    m_jumpBackwardMenu->setEnabled(m_fileLoaded);
    m_jumpForwardMenu->setEnabled(m_fileLoaded);
    m_playbackSpeedMenu->setEnabled(m_fileLoaded);
    m_saveAction->setEnabled(m_fileLoaded);
    m_saveAsAction->setEnabled(m_fileLoaded);
    m_savePlaylistAction->setEnabled(m_fileLoaded);
    m_saveACopyAction->setEnabled(m_fileLoaded);
    m_cumshotMarkerAction->setEnabled(m_fileLoaded);
    m_cyanMarkerAction->setEnabled(m_fileLoaded);
    m_dialogMarkerAction->setEnabled(m_fileLoaded);
    m_magentaMarkerAction->setEnabled(m_fileLoaded);
    m_markerAction->setEnabled(m_fileLoaded);
    m_orangeMarkerAction->setEnabled(m_fileLoaded);
    m_sceneMarkerAction->setEnabled(m_fileLoaded);
    m_stripMarkerAction->setEnabled(m_fileLoaded);
    m_clearInMarkerAction->setEnabled(m_fileLoaded);
    m_clearInAndOutMarkerAction->setEnabled(m_fileLoaded);
    m_clearMarkersAction->setEnabled(m_fileLoaded);
    m_clearOutMarkerAction->setEnabled(m_fileLoaded);
    m_clearSelectedMarkerAction->setEnabled(m_fileLoaded);
    m_editMarkerAction->setEnabled(m_fileLoaded);
    m_goToInMarkerAction->setEnabled(m_fileLoaded);
    m_goToNextMarkerAction->setEnabled(m_fileLoaded);
    m_goToOutMarkerAction->setEnabled(m_fileLoaded);
    m_goToPreviousMarkerAction->setEnabled(m_fileLoaded);
    m_inMarkerAction->setEnabled(m_fileLoaded);
    m_outMarkerAction->setEnabled(m_fileLoaded);
    m_jumpToTimeAction->setEnabled(m_fileLoaded);
    m_nextVideoAction->setEnabled(m_fileLoaded);
    m_nextFrameAction->setEnabled(m_fileLoaded);
    m_previousVideoAction->setEnabled(m_fileLoaded);
    m_previousFrameAction->setEnabled(m_fileLoaded);
    m_restartVideoAction->setEnabled(m_fileLoaded);
    m_createSubclipAction->setEnabled(m_fileLoaded);
    m_toggleSubtitlesAction->setEnabled(m_fileLoaded);
    m_takeSnapshotAction->setEnabled(m_fileLoaded);

    updateRecentFiles();
}

QString MenuBar::trackName(const QMediaMetaData &metaData, int index)
{
    QString name;
    QString title = metaData.stringValue(QMediaMetaData::Title);
    QLocale::Language lang = metaData.value(QMediaMetaData::Language).value<QLocale::Language>();

    if (title.isEmpty()) {
        if (lang == QLocale::Language::AnyLanguage)
            name = tr("Track %1").arg(index + 1);
        else
            name = QLocale::languageToString(lang);
    } else {
        if (lang == QLocale::Language::AnyLanguage)
            name = title;
        else
            name = QStringLiteral("%1 - [%2]").arg(title).arg(QLocale::languageToString(lang));
    }
    return name;
}

void MenuBar::setActiveAudioDevice(const QAudioDevice &audioDevice)
{
    m_activeAudioDevice = audioDevice;
}

void MenuBar::setActiveAudioTrack(int track)
{
    m_activeAudioTrack = track;
}

void MenuBar::setActiveVideoTrack(int track)
{
    m_activeVideoTrack = track;
}

void MenuBar::setActiveSubtitleTrack(int track)
{
    m_activeSubtitleTrack = track;
}

void MenuBar::refreshSettings()
{
    QSettings settings;

    m_frameWalkTime = settings.value("frameWalkTime", 1000).toInt();
    m_playbackSpeedAdjustment = settings.value("playbackSpeedAdjustment", 0.5).toDouble();
    m_playbackSpeedFineAdjustment = settings.value("playbackSpeedFineAdjustment", 0.25).toDouble();
    m_volumeStep = settings.value("volumeStep", 0.10).toDouble();
    m_jumpSmall = settings.value("jumpSmall", 5).toInt();
    m_jumpMedium = settings.value("jumpMedium", 15).toInt();
    m_jumpLarge = settings.value("jumpLarge", 30).toInt();
    m_jumpExtraLarge = settings.value("jumpExtraLarge", 90).toInt();
    m_stashServerUrl = settings.value("stashServerUrl", "http://127.0.0.1:9999").toString();
    m_maxRecentFiles = settings.value("maxRecentFiles", 9).toInt();

    if (m_initialised)
        setHotkeys();
}

void MenuBar::updateAudioOutputs(QList<QAudioDevice> audioDevices)
{
    for (int j = 0; j < 15; ++j)
        m_audioOutputActions[j]->setVisible(false);

    m_audioOutputActions[0]->setText(tr("Default"));
    m_audioOutputActions[0]->setData(QVariant::fromValue(QAudioDevice()));
    m_audioOutputActions[0]->setVisible(true);
    int i = 1;
    for (auto &deviceInfo : audioDevices) {
        if (i <= 15) {
            m_audioOutputActions[i]->setText(deviceInfo.description());
            m_audioOutputActions[i]->setData(QVariant::fromValue(deviceInfo));
            m_audioOutputActions[i]->setVisible(true);
            if (m_activeAudioDevice == deviceInfo) {
                m_audioOutputActions[i]->setChecked(true);
            }
            ++i;
        }
    }

    for (int j = i; j < 15; ++j)
        m_audioOutputActions[j]->setVisible(false);

    m_audioDeviceMenu->setEnabled(i > 1);
}

void MenuBar::updateAudioTracks(QList<QMediaMetaData> audioTracks)
{
    for (int j = 0; j < 15; ++j)
        m_audioTrackActions[j]->setVisible(false);

    m_audioTrackActions[0]->setText(tr("No audio"));
    m_audioTrackActions[0]->setData(-1);
    m_audioTrackActions[0]->setVisible(true);

    for (int i = 0; i < audioTracks.size(); ++i) {
        if (i <= 14) {
            m_audioTrackActions[i+1]->setText(trackName(audioTracks.at(i), i));
            m_audioTrackActions[i+1]->setData(i);
            m_audioTrackActions[i+1]->setVisible(true);
            if (m_activeAudioTrack == i) {
                m_audioTrackActions[i+1]->setChecked(true);
            }
        }
    }

    for (int j = audioTracks.size(); j < 14; ++j)
        m_audioTrackActions[j+1]->setVisible(false);

    m_audioTrackMenu->setEnabled(audioTracks.size() > 0);
}

void MenuBar::updateVideoTracks(QList<QMediaMetaData> videoTracks)
{
    for (int j = 0; j < 15; ++j)
        m_videoTrackActions[j]->setVisible(false);

    m_videoTrackActions[0]->setText(tr("No video"));
    m_videoTrackActions[0]->setData(-1);
    m_videoTrackActions[0]->setVisible(true);

    for (int i = 0; i < videoTracks.size(); ++i) {
        if (i <= 14) {
            m_videoTrackActions[i+1]->setText(trackName(videoTracks.at(i), i));
            m_videoTrackActions[i+1]->setData(i);
            m_videoTrackActions[i+1]->setVisible(true);
            if (m_activeVideoTrack == i) {
                m_videoTrackActions[i+1]->setChecked(true);
            }
        }
    }

    for (int j = videoTracks.size(); j < 14; ++j)
        m_videoTrackActions[j+1]->setVisible(false);

    m_videoTrackMenu->setEnabled(videoTracks.size() > 0);
}

void MenuBar::updateSubtitleTracks(QList<QMediaMetaData> subtitleTracks)
{
    for (int j = 0; j < 15; ++j)
        m_subtitleTrackActions[j]->setVisible(false);

    m_subtitleTrackActions[0]->setText(tr("No subtitles"));
    m_subtitleTrackActions[0]->setData(-1);
    m_subtitleTrackActions[0]->setVisible(true);

    for (int i = 0; i < subtitleTracks.size(); ++i) {
        if (i <= 14) {
            m_subtitleTrackActions[i+1]->setText(trackName(subtitleTracks.at(i), i));
            m_subtitleTrackActions[i+1]->setData(i);
            m_subtitleTrackActions[i+1]->setVisible(true);
            if (m_activeSubtitleTrack == i) {
                m_subtitleTrackActions[i+1]->setChecked(true);
            }
        }
    }

    for (int j = subtitleTracks.size(); j < 14; ++j)
        m_subtitleTrackActions[j+1]->setVisible(false);

    m_subtitleTrackMenu->setEnabled(subtitleTracks.size() > 0);
}

// TODO: Refactor
void MenuBar::updateRecentFiles()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), m_maxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1: %2").arg(i + 1).arg(files[i]);
        m_recentFileActions[i]->setText(text);
        m_recentFileActions[i]->setData(files[i]);
        m_recentFileActions[i]->setVisible(true);
        m_recentFileActions[i]->setShortcut(QKeySequence(QString("Alt+%1").arg(i+1)));
        m_recentFileActions[i]->setShortcutContext(Qt::ApplicationShortcut);
    }
    for (int j = numRecentFiles; j < m_maxRecentFiles; ++j)
        m_recentFileActions[j]->setVisible(false);

    m_recentFilesSeparator->setVisible(numRecentFiles > 0);
    m_openRecentMenu->setEnabled(numRecentFiles > 0);
}

void MenuBar::setPlayerStatus(bool loaded)
{
    m_fileLoaded = loaded;
    refreshMenuItems();
}

void MenuBar::setMuted(bool mute)
{
    m_volumeMuteAction->setChecked(mute);
}

void MenuBar::markerShowing(QString type, bool showing)
{
    if (type == "marker") {
        m_toggleMarkersAction->setChecked(showing);
    } else if (type == "cumshot") {
        m_toggleCumshotMarkersAction->setChecked(showing);
    } else if (type == "cyan") {
        m_toggleCyanMarkersAction->setChecked(showing);
    } else if (type == "dialog") {
        m_toggleDialogMarkersAction->setChecked(showing);
    } else if (type == "magenta") {
        m_toggleMagentaMarkersAction->setChecked(showing);
    } else if (type == "orange") {
        m_toggleOrangeMarkersAction->setChecked(showing);
    } else if (type == "scene") {
        m_toggleSceneMarkersAction->setChecked(showing);
    } else if (type == "strip") {
        m_toggleStripMarkersAction->setChecked(showing);
    }
}

void MenuBar::playlistShowing(bool showing)
{
    m_togglePlaylistAction->setChecked(showing);
}

void MenuBar::statusBarShowing(bool showing)
{
    m_toggleStatusBarAction->setChecked(showing);
}

void MenuBar::videoControlsShowing(bool showing)
{
    m_toggleVideoControlsAction->setChecked(showing);
}

void MenuBar::settingsUpdatedSlot()
{
    emit settingsUpdated();
    refreshSettings();
}


// File Menu
void MenuBar::openFile_Clicked()
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
    QFileDialog fileDialog(this);
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

void MenuBar::openMultipleFiles_Clicked()
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
    QFileDialog fileDialog(this);
    fileDialog.setNameFilters(fileFilters);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setWindowTitle(tr("Open Multiple File"));
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

void MenuBar::openFolder_Clicked()
{
    QSettings settings;

    // Create open folder dialog.
    QFileDialog folderDialog(this);
    folderDialog.setFileMode(QFileDialog::Directory);
    folderDialog.setOption(QFileDialog::ShowDirsOnly, true);
    folderDialog.setWindowTitle(tr("Open Folder"));
    folderDialog.setDirectory(settings.value("lastFolderDirectory", QStandardPaths::MoviesLocation).toString());

    if (folderDialog.exec() == QDialog::Accepted) {
        QStringList selectedFolders = folderDialog.selectedFiles();
        if (!selectedFolders.isEmpty()) {
            QStringList fileList;
            QDirIterator it(selectedFolders.first(), QDir::Files | QDir::NoDotAndDotDot);

            while (it.hasNext()) {
                it.next();
                fileList.append(it.filePath());
            }

            emit openFiles(fileList);
            settings.setValue("lastFolderDirectory", selectedFolders.first());
        }
    }
}

void MenuBar::openNetworkStream_Clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("Open Network Stream"),
                                         tr("Network Stream URL:"),
                                         QLineEdit::Normal,
                                         QString(),
                                         &ok);

    if (ok && !text.isEmpty()) {
        QStringList networkStreamList;
        networkStreamList << text;

        emit openFiles(networkStreamList, false);
    }
}

void MenuBar::openRecentFile_Clicked()
{
    QStringList fileList;

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        fileList << action->data().toString();
        emit openFiles(fileList);
    }
}

void MenuBar::openRecent_Clicked() {}

void MenuBar::clearRecentFiles_Clicked()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    while (!files.isEmpty()) {
        files.removeLast();
    }
    settings.setValue("recentFileList", files);
    updateRecentFiles();
}

void MenuBar::closeFile_Clicked()
{
    emit closeFile();
}

void MenuBar::closeAll_Clicked()
{
    emit closeAllFiles();
}

void MenuBar::save_Clicked() {}

void MenuBar::saveAs_Clicked() {}

void MenuBar::savePlaylist_Clicked()
{
    QSettings settings;

    // File filters
    QStringList fileFilters;
    fileFilters << constants::PlaylistFileExtensions;
    fileFilters << "All Files (*.*)";

    // Create save file dialog.
    QFileDialog fileDialog(this);
    fileDialog.setNameFilters(fileFilters);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setWindowTitle(tr("Save Playlist"));
    fileDialog.setDirectory(settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString());
    fileDialog.selectFile("untitled.vpl");

    if (fileDialog.exec() == QDialog::Accepted) {
        QStringList fileNames = fileDialog.selectedFiles();
        if (!fileNames.isEmpty()) {
            QString fileName = fileNames.first();

            // Check if files exists and ask user if they wish to overwrite file
            if (QFile::exists(fileName)) {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this,
                    tr("File Exists"),
                    tr("The file %1 already exists. Do you want to overwrite it?").arg(fileName),
                    QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    emit savePlaylist(fileName, "");
                }
            }
        }
    }
}

void MenuBar::saveACopy_Clicked() {}

void MenuBar::importProject_Clicked() {}

void MenuBar::importCaptions_Clicked() {}

void MenuBar::importMarkers_Clicked() {}

void MenuBar::exportCaptions_Clicked() {}

void MenuBar::exportClips_Clicked() {}

void MenuBar::exportMarkers_Clicked() {}

void MenuBar::exportMedia_Clicked() {}

void MenuBar::showPreferences_Clicked()
{
    if (m_settingsWindow)
        m_settingsWindow->close();

    m_settingsWindow = new SettingsWindow(this);
    m_settingsWindow->show();
    m_settingsWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(m_settingsWindow, &SettingsWindow::updateSettings, this, &MenuBar::settingsUpdatedSlot);
}

void MenuBar::emergencyCollapse_Clicked()
{
    emit emergencyCollapse();
}

void MenuBar::exit_Clicked()
{
    QMessageBox::StandardButton confirmationBox;
    confirmationBox = QMessageBox::question(this,
        tr("Exit Application"),
        tr("Are you sure you want to exit?"),
        QMessageBox::Yes | QMessageBox::No);

    if (confirmationBox == QMessageBox::Yes) {
        emit exitApplication();
    }
}



// View Menu
void MenuBar::togglePlaylist_Clicked()
{
    emit togglePlaylist();
}

void MenuBar::toggleStatusBar_Clicked()
{
    emit toggleStatusBar();
}

void MenuBar::toggleMarkers_Clicked()
{
    emit toggleMarkers();
}

void MenuBar::toggleCumshotMarkers_Clicked()
{
    emit toggleCumshotMarkers();
}

void MenuBar::toggleCyanMarkers_Clicked()
{
    emit toggleCyanMarkers();
}

void MenuBar::toggleDialogMarkers_Clicked()
{
    emit toggleDialogMarkers();
}

void MenuBar::toggleMagentaMarkers_Clicked()
{
    emit toggleMagentaMarkers();
}

void MenuBar::toggleOrangeMarkers_Clicked()
{
    emit toggleOrangeMarkers();
}

void MenuBar::toggleSceneTransitionMarkers_Clicked()
{
    emit toggleSceneTransitionMarkers();
}

void MenuBar::toggleStripMarkers_Clicked()
{
    emit toggleStripMarkers();
}

void MenuBar::showLogFileViewer_Clicked()
{
    if (m_logViewer)
        m_logViewer->close();

    m_logViewer = new LogViewer(this);
    m_logViewer->show();
    m_logViewer->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MenuBar::toggleVideoControls_Clicked()
{
    emit toggleVideoControls();
}

void MenuBar::showMediaInformation_Clicked()
{
    emit showMediaInformation();
}



// Playback Menu
void MenuBar::togglePlayPause_Clicked()
{
    emit togglePlayPause();
}

void MenuBar::nextVideo_Clicked()
{
    emit nextVideo();
}

void MenuBar::nextFrame_Clicked()
{
    emit videoSeek(m_frameWalkTime);
}

void MenuBar::previousVideo_Clicked()
{
    emit previousVideo();
}

void MenuBar::previousFrame_Clicked()
{
    emit videoSeek(m_frameWalkTime*-1);
}

void MenuBar::fasterSpeed_Clicked()
{
    emit changePlaybackSpeed(m_playbackSpeedAdjustment);
}

void MenuBar::fasterFineSpeed_Clicked()
{
    emit changePlaybackSpeed(m_playbackSpeedFineAdjustment);
}

void MenuBar::normalSpeed_Clicked()
{
    emit setPlaybackSpeedNormal();
}

void MenuBar::slowerSpeed_Clicked()
{
    emit changePlaybackSpeed(-m_playbackSpeedAdjustment);
}

void MenuBar::slowerFineSpeed_Clicked()
{
    emit changePlaybackSpeed(-m_playbackSpeedFineAdjustment);
}

void MenuBar::jumpBackwardSmall_Clicked()
{
    emit videoSeek(m_jumpSmall*-1000);
}

void MenuBar::jumpBackwardMedium_Clicked()
{
    emit videoSeek(m_jumpMedium*-1000);
}

void MenuBar::jumpBackwardLarge_Clicked()
{
    emit videoSeek(m_jumpLarge*-1000);
}

void MenuBar::jumpBackwardExtraLarge_Clicked()
{
    emit videoSeek(m_jumpExtraLarge*-1000);
}

void MenuBar::jumpForwardSmall_Clicked()
{
    emit videoSeek(m_jumpSmall*1000);
}

void MenuBar::jumpForwardMedium_Clicked()
{
    emit videoSeek(m_jumpMedium*1000);
}

void MenuBar::jumpForwardLarge_Clicked()
{
    emit videoSeek(m_jumpLarge*1000);
}

void MenuBar::jumpForwardExtraLarge_Clicked()
{
    emit videoSeek(m_jumpExtraLarge*1000);
}

void MenuBar::jumpToTime_Clicked()
{
    QDialog timeDialog;
    timeDialog.setWindowTitle(tr("Go to time"));

    QLabel *label = new QLabel(tr("Go to time:"), &timeDialog);
    QTimeEdit *timeEdit = new QTimeEdit(&timeDialog);
    timeEdit->setDisplayFormat("HH:mm:ss");

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &timeDialog);

    connect(buttonBox, &QDialogButtonBox::accepted, &timeDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &timeDialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&timeDialog);
    layout->addWidget(label);
    layout->addWidget(timeEdit);
    layout->addWidget(buttonBox);
    timeDialog.setLayout(layout);

    int result = timeDialog.exec();
    if (result == QDialog::Accepted) {
        QTime selectedTime = timeEdit->time();
        qint64 mseconds = selectedTime.msecsSinceStartOfDay();

        emit videoJumpToTime(mseconds);
    }
}

void MenuBar::restartVideo_Clicked()
{
    emit restartVideo();
}



// Audio Menu
void MenuBar::increaseVolume_Clicked()
{
    emit changeVolume(m_volumeStep);
}

void MenuBar::decreaseVolume_Clicked()
{
    emit changeVolume(-m_volumeStep);
}

void MenuBar::toggleMute_Clicked()
{
    emit toggleMute();
}

void MenuBar::selectAudioOutput_Clicked()
{
    for (int i = 0; i < 15; ++i) {
        m_audioOutputActions[i]->setChecked(false);
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        auto device = action->data().value<QAudioDevice>();
        emit setAudioOutput(device);
        action->setChecked(true);
    }
}

void MenuBar::selectAudioTrack_Clicked()
{
    for (int i = 0; i < 15; ++i) {
        m_audioTrackActions[i]->setChecked(false);
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        emit setAudioTrack(action->data().toInt());
        action->setChecked(true);
    }
}



// Video Menu
void MenuBar::toggleFullscreen_Clicked()
{
    emit toggleFullscreen();
}

void MenuBar::takeSnapshot_Clicked()
{
    emit takeSnapshot();
}

void MenuBar::selectVideoTrack_Clicked()
{
    for (int i = 0; i < 15; ++i) {
        m_videoTrackActions[i]->setChecked(false);
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        emit setVideoTrack(action->data().toInt());
        action->setChecked(true);
    }
}

void MenuBar::selectSubtitleTrack_Clicked()
{
    for (int i = 0; i < 15; ++i) {
        m_subtitleTrackActions[i]->setChecked(false);
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        emit setSubtitleTrack(action->data().toInt());
        action->setChecked(true);
    }
}



// Markers Menu
void MenuBar::addMarker_Clicked()
{
    emit addMarker("marker");
}

void MenuBar::goToNextMarker_Clicked()
{
    emit nextMarker();
}

void MenuBar::goToPreviousMarker_Clicked()
{
    emit previousMarker();
}

void MenuBar::clearSelectedMarker_Clicked()
{
    emit clearSelectedMarker();
}

void MenuBar::clearAllMarkers_Clicked()
{
    QMessageBox::StandardButton confirmationBox = QMessageBox::question(
        this,
        tr("Clear Markers"),
        tr("Are you sure you want to clear all the markers?"),
        QMessageBox::Yes | QMessageBox::No
    );

    if (confirmationBox == QMessageBox::Yes) {
        emit clearMarkers();
    }
}

void MenuBar::editMarker_Clicked() {}

void MenuBar::addSceneTransitionMarker_Clicked()
{
    emit addMarker("scene");
}

void MenuBar::addCumshotMarker_Clicked()
{
    emit addMarker("cumshot");
}

void MenuBar::addStripMarker_Clicked()
{
    emit addMarker("strip");
}

void MenuBar::addDialogMarker_Clicked()
{
    emit addMarker("dialog");
}

void MenuBar::addCyanMarker_Clicked()
{
    emit addMarker("cyan");
}

void MenuBar::addMagentaMarker_Clicked()
{
    emit addMarker("magenta");
}

void MenuBar::addOrangeMarker_Clicked()
{
    emit addMarker("orange");
}

void MenuBar::addInMarker_Clicked()
{
    emit addMarker("in");
}

void MenuBar::addOutMarker_Clicked()
{
    emit addMarker("out");
}

void MenuBar::clearInMarker_Clicked()
{
    emit clearInMarker();
}

void MenuBar::clearOutMarker_Clicked()
{
    emit clearOutMarker();
}

void MenuBar::clearInAndOutMarker_Clicked()
{
    emit clearInMarker();
    emit clearOutMarker();
}

void MenuBar::goToInMarker_Clicked()
{
    emit goToInMarker();
}

void MenuBar::goToOutMarker_Clicked()
{
    emit goToOutMarker();
}



// Tools Menu
void MenuBar::createSubclip_Clicked()
{
    emit createSubclip();
}

void MenuBar::streamVideoFromStash_Clicked()
{
    if (m_stashServerUrl.isEmpty()) {
        QMessageBox::warning(this, tr("Stash Error"),
            tr("No stash url set in preferences. Please go to preferences and add your stash server url"));
        return;
    }

    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("Stream Stash Video"),    // Dialog title
                                         tr("Stash Video ID:"),         // Input label
                                         QLineEdit::Normal,        // Input mode (e.g., Normal, NoEcho, Password)
                                         QString(),                // Default text
                                         &ok);                     // Pointer to a boolean that is true if OK was pressed

    if (ok && !text.isEmpty()) {
        QStringList stashFiles;
        stashFiles << m_stashServerUrl + "/scene/" + text + "/stream";

        emit openFiles(stashFiles);
    }
}

void MenuBar::testFunction_Clicked()
{
    emit testFunction();
}



// Subtitle Menu
void MenuBar::openSubtitleFile_Clicked() {}

void MenuBar::toggleSubtitles_Clicked() {}



// Help Menu
void MenuBar::showHelp_Clicked()
{
    if (m_helpDialog)
        m_helpDialog->close();

    m_helpDialog = new HelpDialog(this);
    m_helpDialog->show();
    m_helpDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

// TODO: Implement
void MenuBar::showFeedback_Clicked()
{
}

void MenuBar::actionShowLogFiles_Clicked()
{
    QString logsDirString = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    if (VURA_BUILD_TYPE == "Debug") {
        logsDirString = constants::ApplicationDebugFolder + "/logs";
    }

    if (QDir(logsDirString).exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(logsDirString));

    } else {
        QMessageBox::critical(
            this,
            tr("Vura Error"),
            QString("The application log directory %1 does not exist.").arg(logsDirString)
            );
    }
}

// TODO: Implement
void MenuBar::actionUploadCurrentLogFile_Clicked()
{

}

// TODO: Implement
void MenuBar::actionUploadPreviousLogFile_Clicked()
{

}

void MenuBar::actionViewCurrentLog_Clicked()
{
    if (m_logViewer)
        m_logViewer->close();

    m_logViewer = new LogViewer(this);
    m_logViewer->show();
    m_logViewer->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MenuBar::actionShowCrashReports_Clicked()
{
    QString crashesDirString = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/crashes";
    if (VURA_BUILD_TYPE == "Debug") {
        crashesDirString = constants::ApplicationDebugFolder + "/crashes";
    }

    if (QDir(crashesDirString).exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(crashesDirString));

    } else {
        QMessageBox::critical(
            this,
            tr("Vura Error"),
            QString("The application crashes directory %1 does not exist.").arg(crashesDirString)
            );
    }
}

// TODO: Implement
void MenuBar::actionUploadPreviousCrashReport_Clicked()
{

}

void MenuBar::actionCheckFileIntegrity_Clicked() {}

void MenuBar::showUpdates_Clicked()
{
    if (m_updateDialog)
        m_updateDialog->close();

    m_updateDialog = new UpdateDialog(this);
    m_updateDialog->show();
    m_updateDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

// TODO: Implement
void MenuBar::actionWhatsNew_Clicked()
{

}

// TODO: Implement
void MenuBar::actionReleaseNotes_Clicked()
{

}

void MenuBar::showAbout_Clicked()
{
    if (m_aboutDialog)
        m_aboutDialog->close();

    m_aboutDialog = new AboutDialog(this);
    m_aboutDialog->show();
    m_aboutDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

