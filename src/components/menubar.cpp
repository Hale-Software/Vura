#include "menubar.h"


MenuBar::MenuBar(QWidget *parent) : QMenuBar(parent)
{
    QSettings settings;

    m_jumpSmall = settings.value("jumpSmall", 5).toInt();
    m_jumpMedium = settings.value("jumpMedium", 15).toInt();
    m_jumpLarge = settings.value("jumpLarge", 45).toInt();
    m_jumpExtraLarge = settings.value("jumpExtraLarge", 45).toInt();

    // Create Menus
    m_fileMenu = new QMenu(tr("&File"), this);
    m_viewMenu = new QMenu(tr("&View"), this);
    m_playbackMenu = new QMenu(tr("&Playback"), this);
    m_markersMenu = new QMenu(tr("&Markers"), this);
    m_audioMenu = new QMenu(tr("&Audio"), this);
    m_videoMenu = new QMenu(tr("&Video"), this);
    m_subtitleMenu = new QMenu(tr("&Subtitle"), this);
    m_toolsMenu = new QMenu(tr("&Tools"), this);
    m_helpMenu = new QMenu(tr("&Help"), this);
    m_fileImportMenu = new QMenu(tr("&Import"), this);
    m_fileExportMenu = new QMenu(tr("&Export"), this);
    m_openRecentMenu = new QMenu(tr("&Open Recent"), this);
    m_audioDeviceMenu = new QMenu(tr("&Audio Device"), this);
    m_audioTrackMenu = new QMenu(tr("&Audio Track"), this);
    m_jumpBackwardMenu = new QMenu(tr("&Jump Backward"), this);
    m_jumpForwardMenu = new QMenu(tr("&Jump Forward"), this);
    m_playbackSpeedMenu = new QMenu(tr("&Playback Speed"), this);
    m_subtitleTrackMenu = new QMenu(tr("&Subtitle Track"), this);
    m_videoTrackMenu = new QMenu(tr("&Video Track"), this);
    m_viewMarkersMenu = new QMenu(tr("&Toggle Markers"), this);
    // m_ = new QMenu(tr("&"), this);

    // Create Menu Actions
    m_volumeIncreaseAction = new QAction(tr("&Increase Volume"), this);
    m_volumeDecreaseAction = new QAction(tr("&Decrease Volume"), this);
    m_volumeMuteAction = new QAction(tr("&Mute"), this);
    m_closeAction = new QAction(tr("&Close"), this);
    m_closeAllAction = new QAction(tr("&Close All"), this);
    m_emergencyCollapseAction = new QAction(tr("&Emergency Collapse"), this);
    m_exitAction = new QAction(tr("&Exit"), this);
    m_openFileAction = new QAction(tr("&Open File..."), this);
    m_openFolderAction = new QAction(tr("&Open Folder..."), this);
    m_openMultipleFilesAction = new QAction(tr("&Open Multiple Files..."), this);
    m_openNetworkStreamAction = new QAction(tr("&Open Network Stream..."), this);
    m_showPreferencesAction = new QAction(tr("&Preferences"), this);
    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAsAction = new QAction(tr("&Save As..."), this);
    m_savePlaylistAction = new QAction(tr("&Save Playlist"), this);
    m_saveACopyAction = new QAction(tr("&Save A Copy..."), this);
    m_importProjectAction = new QAction(tr("&Project..."), this);
    m_importCaptionsAction = new QAction(tr("&Captions..."), this);
    m_importMarkersAction = new QAction(tr("&Markers..."), this);
    m_exportCaptionsAction = new QAction(tr("&Captions..."), this);
    m_exportClipsAction = new QAction(tr("&Clips..."), this);
    m_exportMarkersAction = new QAction(tr("&Markers..."), this);
    m_exportMediaAction = new QAction(tr("&Media..."), this);
    m_showAboutAction = new QAction(tr("&About"), this);
    m_showHelpAction = new QAction(tr("&Help"), this);
    m_showFeedbackAction = new QAction(tr("&Provide Feedback"), this);
    m_updateAction = new QAction(tr("&Check for Updates..."), this);
    m_cumshotMarkerAction = new QAction(tr("&Add Cumshot Marker"), this);
    m_cyanMarkerAction = new QAction(tr("&Add Cyan Marker"), this);
    m_dialogMarkerAction = new QAction(tr("&Add Dialog Marker"), this);
    m_magentaMarkerAction = new QAction(tr("&Add Magenta Marker"), this);
    m_markerAction = new QAction(tr("&Add Marker"), this);
    m_orangeMarkerAction = new QAction(tr("&Add Orange Marker"), this);
    m_sceneMarkerAction = new QAction(tr("&Add Scene Transition Marker"), this);
    m_stripMarkerAction = new QAction(tr("&Add Strip Marker"), this);
    m_clearInMarkerAction = new QAction(tr("&Clear In"), this);
    m_clearInAndOutMarkerAction = new QAction(tr("&Clear In and Out"), this);
    m_clearMarkersAction = new QAction(tr("&Clear Markers"), this);
    m_clearOutMarkerAction = new QAction(tr("&Clear Out"), this);
    m_clearSelectedMarkerAction = new QAction(tr("&Clear Selected Marker"), this);
    m_editMarkerAction = new QAction(tr("&Edit Selected Marker..."), this);
    m_goToInMarkerAction = new QAction(tr("&Go to In"), this);
    m_goToNextMarkerAction = new QAction(tr("&Go to Next Marker"), this);
    m_goToOutMarkerAction = new QAction(tr("&Go to Out"), this);
    m_goToPreviousMarkerAction = new QAction(tr("&Go to Previous Marker"), this);
    m_inMarkerAction = new QAction(tr("&Mark In"), this);
    m_outMarkerAction = new QAction(tr("&Mark Out"), this);
    m_jumpToTimeAction = new QAction(tr("&Jump to Specific Time..."), this);
    m_nextVideoAction = new QAction(tr("&Next Video"), this);
    m_nextFrameAction = new QAction(tr("&Next Frame"), this);
    m_togglePlayAction = new QAction(tr("&Play/Pause"), this);
    m_previousVideoAction = new QAction(tr("&Previous Video"), this);
    m_previousFrameAction = new QAction(tr("&Previous Frame"), this);
    m_restartVideoAction = new QAction(tr("&Restart Video"), this);
    m_jumpBackwardSmallAction = new QAction(tr("&Small"), this);
    m_jumpBackwardMediumAction = new QAction(tr("&Medium"), this);
    m_jumpBackwardLargeAction = new QAction(tr("&Large"), this);
    m_jumpBackwardExtraLargeAction = new QAction(tr("&Extra Large"), this);
    m_jumpForwardSmallAction = new QAction(tr("&Small"), this);
    m_jumpForwardMediumAction = new QAction(tr("&Medium"), this);
    m_jumpForwardLargeAction = new QAction(tr("&Large"), this);
    m_jumpForwardExtraLargeAction = new QAction(tr("&Extra Large"), this);
    m_playbackSpeedFasterAction = new QAction(tr("&Faster"), this);
    m_playbackSpeedFasterFineAction = new QAction(tr("&Faster (fine)"), this);
    m_playbackSpeedNormalAction = new QAction(tr("&Normal"), this);
    m_playbackSpeedSlowerFineAction = new QAction(tr("&Slower (fine)"), this);
    m_playbackSpeedSlowerAction = new QAction(tr("&Slower"), this);
    m_createSubclipAction = new QAction(tr("&Make Subclip"), this);
    m_streamStashVideoAction = new QAction(tr("&Stream Video from Stash..."), this);
    m_testFunctionAction = new QAction(tr("&Test Function"), this);
    m_openSubtitleFileAction = new QAction(tr("&Open Subtitle File..."), this);
    m_toggleSubtitlesAction = new QAction(tr("&Toggle Subtitles"), this);
    m_toggleFullscreenAction = new QAction(tr("&Fullscreen"), this);
    m_takeSnapshotAction = new QAction(tr("&Take Snapshot"), this);
    m_showMediaInformationAction = new QAction(tr("&Media Information"), this);
    m_showLogFileViewerAction = new QAction(tr("&Log Viewer"), this);
    m_togglePlaylistAction = new QAction(tr("&Toggle Playlist"), this);
    m_toggleStatusBarAction = new QAction(tr("&Toggle Status Bar"), this);
    m_toggleTagsAction = new QAction(tr("&Toggle Tags"), this);
    m_toggleVideoControlsAction = new QAction(tr("&Toggle Video Controls"), this);
    m_toggleCumshotMarkersAction = new QAction(tr("&Cumshot Markers"), this);
    m_toggleCyanMarkersAction = new QAction(tr("&Cyan Markers"), this);
    m_toggleDialogMarkersAction = new QAction(tr("&Dialog Markers"), this);
    m_toggleMagentaMarkersAction = new QAction(tr("&Magenta Markers"), this);
    m_toggleMarkersAction = new QAction(tr("&Markers"), this);
    m_toggleOrangeMarkersAction = new QAction(tr("&Orange Markers"), this);
    m_toggleSceneMarkersAction = new QAction(tr("&Scene Transition Markers"), this);
    m_toggleStripMarkersAction = new QAction(tr("&Strip Markers"), this);
    m_clearRecentFilesAction = new QAction(tr("&Clear"), this);
    // m_ = new QAction(tr("&"), this);

    // Create Separators
    m_recentFilesSeparator = new QAction(this);
    m_recentFilesSeparator->setSeparator(true);
    // m_ = new QAction(this);

    // Set Menu Item Default Properties
    m_openRecentMenu->setEnabled(false);
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
    m_togglePlayAction->setEnabled(false);

    m_volumeMuteAction->setCheckable(true);
    m_toggleSubtitlesAction->setCheckable(true);
    m_togglePlaylistAction->setCheckable(true);
    m_toggleStatusBarAction->setCheckable(true);
    m_toggleTagsAction->setCheckable(true);
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
    m_toggleTagsAction->setChecked(false);
    m_toggleVideoControlsAction->setChecked(false);


    // Create Recent File Items
    for (int i = 0; i < settings.value("maxRecentFiles", 9).toInt(); ++i) {
        m_recentFileActions[i] = new QAction(this);
        m_recentFileActions[i]->setVisible(false);
        connect(m_recentFileActions[i], &QAction::triggered, this, &MenuBar::openRecentFile);
        m_openRecentMenu->addAction(m_recentFileActions[i]);
    }
    m_recentFilesSeparator->setVisible(false);
    m_openRecentMenu->addAction(m_recentFilesSeparator);
    m_clearRecentFilesAction = new QAction(this);
    m_clearRecentFilesAction->setText("Clear");
    m_clearRecentFilesAction->setVisible(true);
    connect(m_clearRecentFilesAction, &QAction::triggered, this, &MenuBar::clearRecentFiles);
    m_openRecentMenu->addAction(m_clearRecentFilesAction);


    // Create Audio Output Actions
    for (int i = 0; i < 15; ++i) {
        m_audioOutputActions[i] = new QAction(this);
        m_audioOutputActions[i]->setCheckable(true);
        m_audioOutputActions[i]->setChecked(false);
        m_audioOutputActions[i]->setVisible(false);
        connect(m_audioOutputActions[i], &QAction::triggered, this, &MenuBar::selectAudioOutput);
        m_audioDeviceMenu->addAction(m_audioOutputActions[i]);
    }


    // Create Audio Track Actions
    for (int i = 0; i < 15; ++i) {
        m_audioTrackActions[i] = new QAction(this);
        m_audioTrackActions[i]->setCheckable(true);
        m_audioTrackActions[i]->setChecked(false);
        m_audioTrackActions[i]->setVisible(false);
        connect(m_audioTrackActions[i], &QAction::triggered, this, &MenuBar::selectAudioTrack);
        m_audioTrackMenu->addAction(m_audioTrackActions[i]);
    }
    m_audioTrackMenu->setEnabled(false);


    // Create Video Track Actions
    for (int i = 0; i < 15; ++i) {
        m_videoTrackActions[i] = new QAction(this);
        m_videoTrackActions[i]->setCheckable(true);
        m_videoTrackActions[i]->setChecked(false);
        m_videoTrackActions[i]->setVisible(false);
        connect(m_videoTrackActions[i], &QAction::triggered, this, &MenuBar::selectVideoTrack);
        m_videoTrackMenu->addAction(m_videoTrackActions[i]);
    }
    m_videoTrackMenu->setEnabled(false);


    // Create Subtitle Track Actions
    for (int i = 0; i < 15; ++i) {
        m_subtitleTrackActions[i] = new QAction(this);
        m_subtitleTrackActions[i]->setCheckable(true);
        m_subtitleTrackActions[i]->setChecked(false);
        m_subtitleTrackActions[i]->setVisible(false);
        connect(m_subtitleTrackActions[i], &QAction::triggered, this, &MenuBar::selectSubtitleTrack);
        m_subtitleTrackMenu->addAction(m_subtitleTrackActions[i]);
    }
    m_subtitleTrackMenu->setEnabled(false);


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
    m_viewMenu->addAction(m_toggleTagsAction);
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

    m_helpMenu->addAction(m_showHelpAction);
    m_helpMenu->addAction(m_showFeedbackAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_updateAction);
    m_helpMenu->addSeparator();
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


    // Set Menu Item Hotkeys
    Hotkeys m_hotkeys;
    m_hotkeys.setMenuItemHotkey(*m_volumeIncreaseAction);
    m_hotkeys.setMenuItemHotkey(*m_emergencyCollapseAction);
    m_hotkeys.setMenuItemHotkey(*m_exitAction);
    m_hotkeys.setMenuItemHotkey(*m_openFileAction);


    // Menu Item Connections
    connect(m_volumeIncreaseAction, &QAction::triggered, this, &MenuBar::increaseVolume);
    connect(m_volumeDecreaseAction, &QAction::triggered, this, &MenuBar::decreaseVolume);
    connect(m_volumeMuteAction, &QAction::triggered, this, &MenuBar::toggleMute);
    connect(m_closeAction, &QAction::triggered, this, &MenuBar::closeFile);
    connect(m_closeAllAction, &QAction::triggered, this, &MenuBar::closeAll);
    connect(m_emergencyCollapseAction, &QAction::triggered, this, &MenuBar::emergencyCollapse);
    connect(m_exitAction, &QAction::triggered, this, &MenuBar::exit);
    connect(m_openFileAction, &QAction::triggered, this, &MenuBar::openFile);
    connect(m_openFolderAction, &QAction::triggered, this, &MenuBar::openFolder);
    connect(m_openMultipleFilesAction, &QAction::triggered, this, &MenuBar::openMultipleFiles);
    connect(m_openNetworkStreamAction, &QAction::triggered, this, &MenuBar::openNetworkStream);
    connect(m_showPreferencesAction, &QAction::triggered, this, &MenuBar::showPreferences);
    connect(m_saveAction, &QAction::triggered, this, &MenuBar::save);
    connect(m_saveAsAction, &QAction::triggered, this, &MenuBar::saveAs);
    connect(m_savePlaylistAction, &QAction::triggered, this, &MenuBar::savePlaylist);
    connect(m_saveACopyAction, &QAction::triggered, this, &MenuBar::saveACopy);
    connect(m_importProjectAction, &QAction::triggered, this, &MenuBar::importProject);
    connect(m_importCaptionsAction, &QAction::triggered, this, &MenuBar::importCaptions);
    connect(m_importMarkersAction, &QAction::triggered, this, &MenuBar::importMarkers);
    connect(m_exportCaptionsAction, &QAction::triggered, this, &MenuBar::exportCaptions);
    connect(m_exportClipsAction, &QAction::triggered, this, &MenuBar::exportClips);
    connect(m_exportMarkersAction, &QAction::triggered, this, &MenuBar::exportMarkers);
    connect(m_exportMediaAction, &QAction::triggered, this, &MenuBar::exportMedia);
    connect(m_showAboutAction, &QAction::triggered, this, &MenuBar::showAbout);
    connect(m_showHelpAction, &QAction::triggered, this, &MenuBar::showHelp);
    connect(m_showFeedbackAction, &QAction::triggered, this, &MenuBar::showFeedback);
    connect(m_updateAction, &QAction::triggered, this, &MenuBar::showUpdates);
    connect(m_cumshotMarkerAction, &QAction::triggered, this, &MenuBar::addCumshotMarker);
    connect(m_cyanMarkerAction, &QAction::triggered, this, &MenuBar::addCyanMarker);
    connect(m_dialogMarkerAction, &QAction::triggered, this, &MenuBar::addDialogMarker);
    connect(m_magentaMarkerAction, &QAction::triggered, this, &MenuBar::addMagentaMarker);
    connect(m_markerAction, &QAction::triggered, this, &MenuBar::addMarker);
    connect(m_orangeMarkerAction, &QAction::triggered, this, &MenuBar::addOrangeMarker);
    connect(m_sceneMarkerAction, &QAction::triggered, this, &MenuBar::addSceneTransitionMarker);
    connect(m_stripMarkerAction, &QAction::triggered, this, &MenuBar::addStripMarker);
    connect(m_clearInMarkerAction, &QAction::triggered, this, &MenuBar::clearInMarker);
    connect(m_clearInAndOutMarkerAction, &QAction::triggered, this, &MenuBar::clearInAndOutMarker);
    connect(m_clearMarkersAction, &QAction::triggered, this, &MenuBar::clearAllMarkers);
    connect(m_clearOutMarkerAction, &QAction::triggered, this, &MenuBar::clearOutMarker);
    connect(m_clearSelectedMarkerAction, &QAction::triggered, this, &MenuBar::clearSelectedMarker);
    connect(m_editMarkerAction, &QAction::triggered, this, &MenuBar::editMarker);
    connect(m_goToInMarkerAction, &QAction::triggered, this, &MenuBar::goToInMarker);
    connect(m_goToNextMarkerAction, &QAction::triggered, this, &MenuBar::goToNextMarker);
    connect(m_goToOutMarkerAction, &QAction::triggered, this, &MenuBar::goToOutMarker);
    connect(m_goToPreviousMarkerAction, &QAction::triggered, this, &MenuBar::goToPreviousMarker);
    connect(m_inMarkerAction, &QAction::triggered, this, &MenuBar::addInMarker);
    connect(m_outMarkerAction, &QAction::triggered, this, &MenuBar::addOutMarker);
    connect(m_jumpToTimeAction, &QAction::triggered, this, &MenuBar::jumpToTime);
    connect(m_nextVideoAction, &QAction::triggered, this, &MenuBar::nextVideo);
    connect(m_nextFrameAction, &QAction::triggered, this, &MenuBar::nextFrame);
    connect(m_togglePlayAction, &QAction::triggered, this, &MenuBar::togglePlayPause);
    connect(m_previousVideoAction, &QAction::triggered, this, &MenuBar::previousVideo);
    connect(m_previousFrameAction, &QAction::triggered, this, &MenuBar::previousFrame);
    connect(m_restartVideoAction, &QAction::triggered, this, &MenuBar::restartVideo);
    connect(m_jumpBackwardSmallAction, &QAction::triggered, this, &MenuBar::jumpBackwardSmall);
    connect(m_jumpBackwardMediumAction, &QAction::triggered, this, &MenuBar::jumpBackwardMedium);
    connect(m_jumpBackwardLargeAction, &QAction::triggered, this, &MenuBar::jumpBackwardLarge);
    connect(m_jumpBackwardExtraLargeAction, &QAction::triggered, this, &MenuBar::jumpBackwardExtraLarge);
    connect(m_jumpForwardSmallAction, &QAction::triggered, this, &MenuBar::jumpForwardSmall);
    connect(m_jumpForwardMediumAction, &QAction::triggered, this, &MenuBar::jumpForwardMedium);
    connect(m_jumpForwardLargeAction, &QAction::triggered, this, &MenuBar::jumpForwardLarge);
    connect(m_jumpForwardExtraLargeAction, &QAction::triggered, this, &MenuBar::jumpForwardExtraLarge);
    connect(m_playbackSpeedFasterAction, &QAction::triggered, this, &MenuBar::fasterSpeed);
    connect(m_playbackSpeedFasterFineAction, &QAction::triggered, this, &MenuBar::fasterFineSpeed);
    connect(m_playbackSpeedNormalAction, &QAction::triggered, this, &MenuBar::normalSpeed);
    connect(m_playbackSpeedSlowerFineAction, &QAction::triggered, this, &MenuBar::slowerFineSpeed);
    connect(m_playbackSpeedSlowerAction, &QAction::triggered, this, &MenuBar::slowerSpeed);
    connect(m_createSubclipAction, &QAction::triggered, this, &MenuBar::createSubclip);
    connect(m_streamStashVideoAction, &QAction::triggered, this, &MenuBar::streamVideoFromStash);
    connect(m_testFunctionAction, &QAction::triggered, this, &MenuBar::testFunction);
    connect(m_openSubtitleFileAction, &QAction::triggered, this, &MenuBar::openSubtitleFile);
    connect(m_toggleSubtitlesAction, &QAction::triggered, this, &MenuBar::toggleSubtitles);
    connect(m_toggleFullscreenAction, &QAction::triggered, this, &MenuBar::toggleFullscreen);
    connect(m_takeSnapshotAction, &QAction::triggered, this, &MenuBar::takeSnapshot);
    connect(m_showMediaInformationAction, &QAction::triggered, this, &MenuBar::showMediaInformation);
    connect(m_showLogFileViewerAction, &QAction::triggered, this, &MenuBar::showLogFileViewer);
    connect(m_togglePlaylistAction, &QAction::triggered, this, &MenuBar::togglePlaylist);
    connect(m_toggleStatusBarAction, &QAction::triggered, this, &MenuBar::toggleStatusBar);
    connect(m_toggleTagsAction, &QAction::triggered, this, &MenuBar::toggleTags);
    connect(m_toggleVideoControlsAction, &QAction::triggered, this, &MenuBar::toggleVideoControls);
    connect(m_toggleCumshotMarkersAction, &QAction::triggered, this, &MenuBar::toggleCumshotMarkers);
    connect(m_toggleCyanMarkersAction, &QAction::triggered, this, &MenuBar::toggleCyanMarkers);
    connect(m_toggleDialogMarkersAction, &QAction::triggered, this, &MenuBar::toggleDialogMarkers);
    connect(m_toggleMagentaMarkersAction, &QAction::triggered, this, &MenuBar::toggleMagentaMarkers);
    connect(m_toggleMarkersAction, &QAction::triggered, this, &MenuBar::toggleMarkers);
    connect(m_toggleOrangeMarkersAction, &QAction::triggered, this, &MenuBar::toggleOrangeMarkers);
    connect(m_toggleSceneMarkersAction, &QAction::triggered, this, &MenuBar::toggleSceneTransitionMarkers);
    connect(m_toggleStripMarkersAction, &QAction::triggered, this, &MenuBar::toggleStripMarkers);
    //connect(, &QAction::triggered, this, &MenuBar::);

}

void MenuBar::setHotkeys()
{
    /*
    qDebug() << "Configuring hotkeys...";
    QSettings settings;
    QMap<QString,QString> hotkeys;
    settings.beginGroup("Hotkeys");
    const QStringList childKeys = settings.childKeys();
    foreach (const QString &childKey, childKeys)
    {
        hotkeys[childKey] = settings.value(childKey).toString();
    }
    settings.endGroup();

    if (hotkeys.isEmpty()) {
        qDebug() << "QSettings hotkey map is empty.";
        configureDefaultHotkeys();
    }

    foreach (QAction *action, findChildren<QAction *>())
    {
        if (!action->text().isEmpty()) {
            if (action->text() == "Play / Pause") {
                //qDebug() << "Setting hotkey Space for action " << action->text();
                action->setShortcut(tr("Space"));
                action->setShortcutContext(Qt::ApplicationShortcut);
            } else if (hotkeys.contains(action->text())) {
                //qDebug() << "Setting hotkey " << hotkeys[action->text()] << " for action " << action->text();
                action->setShortcut(hotkeys.value(action->text()));
                action->setShortcutContext(Qt::ApplicationShortcut);
            } else {
                //qDebug() << "No hotkey found for action " << action->text();
            }
        }
    }
    */
}

void MenuBar::refreshMenuItems() {}

void MenuBar::setPlayerStatus(bool loaded)
{
    m_fileLoaded = loaded;
    refreshMenuItems();
}

void MenuBar::refreshSettings() {}
void MenuBar::setMuted(bool mute) {}
void MenuBar::markerShowing(QString type, bool showing) {}
void MenuBar::playlistShowing(bool showing) {}
void MenuBar::statusBarShowing(bool showing) {}
void MenuBar::videoControlsShowing(bool showing) {}

// File Menu
void MenuBar::openFile() {}
void MenuBar::openMultipleFiles() {}
void MenuBar::openFolder() {}
void MenuBar::openNetworkStream() {}
void MenuBar::openRecentFile() {}
void MenuBar::openRecent() {}
void MenuBar::clearRecentFiles() {}
void MenuBar::closeFile() {}
void MenuBar::closeAll() {}
void MenuBar::save() {}
void MenuBar::saveAs() {}
void MenuBar::savePlaylist() {}
void MenuBar::saveACopy() {}
void MenuBar::importProject() {}
void MenuBar::importCaptions() {}
void MenuBar::importMarkers() {}
void MenuBar::exportCaptions() {}
void MenuBar::exportClips() {}
void MenuBar::exportMarkers() {}
void MenuBar::exportMedia() {}
void MenuBar::showPreferences() {}
void MenuBar::emergencyCollapse() {}
void MenuBar::exit() {}

// View Menu
void MenuBar::togglePlaylist() {}
void MenuBar::toggleStatusBar() {}
void MenuBar::toggleMarkers() {}
void MenuBar::toggleCumshotMarkers() {}
void MenuBar::toggleCyanMarkers() {}
void MenuBar::toggleDialogMarkers() {}
void MenuBar::toggleMagentaMarkers() {}
void MenuBar::toggleOrangeMarkers() {}
void MenuBar::toggleSceneTransitionMarkers() {}
void MenuBar::toggleStripMarkers() {}
void MenuBar::showLogFileViewer() {}
void MenuBar::toggleVideoControls() {}
void MenuBar::showMediaInformation() {}
void MenuBar::toggleTags() {}

// Playback Menu
void MenuBar::togglePlayPause() {}
void MenuBar::nextVideo() {}
void MenuBar::nextFrame() {}
void MenuBar::previousVideo() {}
void MenuBar::previousFrame() {}
void MenuBar::fasterSpeed() {}
void MenuBar::fasterFineSpeed() {}
void MenuBar::normalSpeed() {}
void MenuBar::slowerSpeed() {}
void MenuBar::slowerFineSpeed() {}
void MenuBar::jumpBackwardSmall() {}
void MenuBar::jumpBackwardMedium() {}
void MenuBar::jumpBackwardLarge() {}
void MenuBar::jumpBackwardExtraLarge() {}
void MenuBar::jumpForwardSmall() {}
void MenuBar::jumpForwardMedium() {}
void MenuBar::jumpForwardLarge() {}
void MenuBar::jumpForwardExtraLarge() {}
void MenuBar::jumpToTime() {}
void MenuBar::restartVideo() {}

// Audio Menu
void MenuBar::increaseVolume() {}
void MenuBar::decreaseVolume() {}
void MenuBar::toggleMute() {}
void MenuBar::selectAudioOutput() {}
void MenuBar::selectAudioTrack() {}

// Video Menu
void MenuBar::toggleFullscreen() {}
void MenuBar::takeSnapshot() {}
void MenuBar::selectVideoTrack() {}
void MenuBar::selectSubtitleTrack() {}

// Markers Menu
void MenuBar::addMarker() {}
void MenuBar::goToNextMarker() {}
void MenuBar::goToPreviousMarker() {}
void MenuBar::clearSelectedMarker() {}
void MenuBar::clearAllMarkers() {}
void MenuBar::editMarker() {}
void MenuBar::addSceneTransitionMarker() {}
void MenuBar::addCumshotMarker() {}
void MenuBar::addStripMarker() {}
void MenuBar::addDialogMarker() {}
void MenuBar::addCyanMarker() {}
void MenuBar::addMagentaMarker() {}
void MenuBar::addOrangeMarker() {}
void MenuBar::addInMarker() {}
void MenuBar::addOutMarker() {}
void MenuBar::clearInMarker() {}
void MenuBar::clearOutMarker() {}
void MenuBar::clearInAndOutMarker() {}
void MenuBar::goToInMarker() {}
void MenuBar::goToOutMarker() {}

// Tools Menu
void MenuBar::createSubclip() {}
void MenuBar::streamVideoFromStash() {}
void MenuBar::testFunction() {}

// Subtitle Menu
void MenuBar::openSubtitleFile() {}
void MenuBar::toggleSubtitles() {}

// Help Menu
void MenuBar::showAbout() {}
void MenuBar::showHelp() {}
void MenuBar::showUpdates() {}
void MenuBar::showFeedback() {}
