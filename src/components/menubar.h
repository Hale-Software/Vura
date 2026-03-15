/*  MENUBAR
 *  ============================
 *  This is a custom menubar used by vura.
 *
 *  MENU ITEM NAMING CONVENTION
 *  ============================
 *  If menu item launches a dialog box append (...) to the menu item
 *  label.
 */
#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QMap>
#include <QVariant>
#include <QDebug>

#include "../utility/hotkeys.h"


class MenuBar : public QMenuBar {
    Q_OBJECT

public:
    explicit MenuBar(QWidget *parent = nullptr);

signals:
    /*
    void open(QStringList fileNames);
    void openF(QString folderName);
    void openNetworkStreams();
    void closeF();
    void closeAllFiles();
    void saveFile(QString fileName);
    void importFile(QString fileName);
    void exportFile(QString fileName);
    void checkForUpdate();
    void addMarker(QString type);
    void clearIn();
    void clearOut();
    void clearInAndOut();
    void clearMarkers();
    void clearMarker();
    void editSelectedMarker();
    void goToIn();
    void goToOut();
    void nextMarker();
    void previousMarker();
    void markIn();
    void markOut();
    void jump(int pos);
    void next();
    void previous();
    void togglePlay();
    void subclip();
    void tFunction();
    void fullscreen();
    void snapshot();
    void audioDeviceChanged(int device);
    void audioTrackChanged(int track);
    void subtitleTrackChanged(int track);
    void seek(int mseconds);
    void speed(double mrate);
    void volume(int mvolume);
    void mute();
    void toggleMarkerVisibility(QString type);
    void playlist();
    void statusBar();
    void videoControls();
    void settingsChanged();
    void collapse();
    void exiting();
    */

public slots:
    void setPlayerStatus(bool loaded);
    void refreshSettings();
    void setMuted(bool mute);
    void markerShowing(QString type, bool showing);
    void playlistShowing(bool showing);
    void statusBarShowing(bool showing);
    void videoControlsShowing(bool showing);

private slots:
    // File Menu
    void closeFile();
    void closeAll();
    void emergencyCollapse();
    void exit();
    void openFile();
    void openMultipleFiles();
    void openFolder();
    void openNetworkStream();
    void save();
    void saveAs();
    void savePlaylist();
    void saveACopy();
    void importProject();
    void importCaptions();
    void importMarkers();
    void exportCaptions();
    void exportClips();
    void exportMarkers();
    void exportMedia();
    void openRecent();
    void showPreferences();
    void openRecentFile();
    void clearRecentFiles();

    // View Menu
    void togglePlaylist();
    void toggleStatusBar();
    void toggleMarkers();
    void toggleCumshotMarkers();
    void toggleCyanMarkers();
    void toggleDialogMarkers();
    void toggleMagentaMarkers();
    void toggleOrangeMarkers();
    void toggleSceneTransitionMarkers();
    void toggleStripMarkers();
    void showLogFileViewer();
    void toggleVideoControls();
    void showMediaInformation();
    void toggleTags();

    // Playback Menu
    void togglePlayPause();
    void nextVideo();
    void nextFrame();
    void previousVideo();
    void previousFrame();
    void fasterSpeed();
    void fasterFineSpeed();
    void normalSpeed();
    void slowerSpeed();
    void slowerFineSpeed();
    void jumpBackwardSmall();
    void jumpBackwardMedium();
    void jumpBackwardLarge();
    void jumpBackwardExtraLarge();
    void jumpForwardSmall();
    void jumpForwardMedium();
    void jumpForwardLarge();
    void jumpForwardExtraLarge();
    void jumpToTime();
    void restartVideo();

    // Audio Menu
    void increaseVolume();
    void decreaseVolume();
    void toggleMute();
    void selectAudioOutput();
    void selectAudioTrack();

    // Video Menu
    void toggleFullscreen();
    void takeSnapshot();
    void selectVideoTrack();
    void selectSubtitleTrack();

    // Markers Menu
    void addMarker();
    void goToNextMarker();
    void goToPreviousMarker();
    void clearSelectedMarker();
    void clearAllMarkers();
    void editMarker();
    void addSceneTransitionMarker();
    void addCumshotMarker();
    void addStripMarker();
    void addDialogMarker();
    void addCyanMarker();
    void addMagentaMarker();
    void addOrangeMarker();
    void addInMarker();
    void addOutMarker();
    void clearInMarker();
    void clearOutMarker();
    void clearInAndOutMarker();
    void goToInMarker();
    void goToOutMarker();

    // Tools Menu
    void createSubclip();
    void streamVideoFromStash();
    void testFunction();

    // Subtitle Menu
    void openSubtitleFile();
    void toggleSubtitles();

    // Help Menu
    void showAbout();
    void showHelp();
    void showUpdates();
    void showFeedback();

private:
    QMenu *m_fileMenu = nullptr;
    QMenu *m_viewMenu = nullptr;
    QMenu *m_playbackMenu = nullptr;
    QMenu *m_markersMenu = nullptr;
    QMenu *m_audioMenu = nullptr;
    QMenu *m_videoMenu = nullptr;
    QMenu *m_subtitleMenu = nullptr;
    QMenu *m_toolsMenu = nullptr;
    QMenu *m_helpMenu = nullptr;
    QMenu *m_fileImportMenu = nullptr;
    QMenu *m_fileExportMenu = nullptr;
    QMenu *m_openRecentMenu = nullptr;
    QMenu *m_audioDeviceMenu = nullptr;
    QMenu *m_audioTrackMenu = nullptr;
    QMenu *m_jumpBackwardMenu = nullptr;
    QMenu *m_jumpForwardMenu = nullptr;
    QMenu *m_playbackSpeedMenu = nullptr;
    QMenu *m_subtitleTrackMenu = nullptr;
    QMenu *m_videoTrackMenu = nullptr;
    QMenu *m_viewMarkersMenu = nullptr;

    QAction *m_volumeIncreaseAction = nullptr;
    QAction *m_volumeDecreaseAction = nullptr;
    QAction *m_volumeMuteAction = nullptr;
    QAction *m_closeAction = nullptr;
    QAction *m_closeAllAction = nullptr;
    QAction *m_emergencyCollapseAction = nullptr;
    QAction *m_exitAction = nullptr;
    QAction *m_openFileAction = nullptr;
    QAction *m_openFolderAction = nullptr;
    QAction *m_openMultipleFilesAction = nullptr;
    QAction *m_openNetworkStreamAction = nullptr;
    QAction *m_showPreferencesAction = nullptr;
    QAction *m_saveAction = nullptr;
    QAction *m_saveAsAction = nullptr;
    QAction *m_savePlaylistAction = nullptr;
    QAction *m_saveACopyAction = nullptr;
    QAction *m_importProjectAction = nullptr;
    QAction *m_importCaptionsAction = nullptr;
    QAction *m_importMarkersAction = nullptr;
    QAction *m_exportCaptionsAction = nullptr;
    QAction *m_exportClipsAction = nullptr;
    QAction *m_exportMarkersAction = nullptr;
    QAction *m_exportMediaAction = nullptr;
    QAction *m_showAboutAction = nullptr;
    QAction *m_showHelpAction = nullptr;
    QAction *m_showFeedbackAction = nullptr;
    QAction *m_updateAction = nullptr;
    QAction *m_cumshotMarkerAction = nullptr;
    QAction *m_cyanMarkerAction = nullptr;
    QAction *m_dialogMarkerAction = nullptr;
    QAction *m_magentaMarkerAction = nullptr;
    QAction *m_markerAction = nullptr;
    QAction *m_orangeMarkerAction = nullptr;
    QAction *m_sceneMarkerAction = nullptr;
    QAction *m_stripMarkerAction = nullptr;
    QAction *m_clearInMarkerAction = nullptr;
    QAction *m_clearInAndOutMarkerAction = nullptr;
    QAction *m_clearMarkersAction = nullptr;
    QAction *m_clearOutMarkerAction = nullptr;
    QAction *m_clearSelectedMarkerAction = nullptr;
    QAction *m_editMarkerAction = nullptr;
    QAction *m_goToInMarkerAction = nullptr;
    QAction *m_goToNextMarkerAction = nullptr;
    QAction *m_goToOutMarkerAction = nullptr;
    QAction *m_goToPreviousMarkerAction = nullptr;
    QAction *m_inMarkerAction = nullptr;
    QAction *m_outMarkerAction = nullptr;
    QAction *m_jumpToTimeAction = nullptr;
    QAction *m_nextVideoAction = nullptr;
    QAction *m_nextFrameAction = nullptr;
    QAction *m_togglePlayAction = nullptr;
    QAction *m_previousVideoAction = nullptr;
    QAction *m_previousFrameAction = nullptr;
    QAction *m_restartVideoAction = nullptr;
    QAction *m_jumpBackwardSmallAction = nullptr;
    QAction *m_jumpBackwardMediumAction = nullptr;
    QAction *m_jumpBackwardLargeAction = nullptr;
    QAction *m_jumpBackwardExtraLargeAction = nullptr;
    QAction *m_jumpForwardSmallAction = nullptr;
    QAction *m_jumpForwardMediumAction = nullptr;
    QAction *m_jumpForwardLargeAction = nullptr;
    QAction *m_jumpForwardExtraLargeAction = nullptr;
    QAction *m_playbackSpeedFasterAction = nullptr;
    QAction *m_playbackSpeedFasterFineAction = nullptr;
    QAction *m_playbackSpeedNormalAction = nullptr;
    QAction *m_playbackSpeedSlowerFineAction = nullptr;
    QAction *m_playbackSpeedSlowerAction = nullptr;
    QAction *m_createSubclipAction = nullptr;
    QAction *m_streamStashVideoAction = nullptr;
    QAction *m_testFunctionAction = nullptr;
    QAction *m_openSubtitleFileAction = nullptr;
    QAction *m_toggleSubtitlesAction = nullptr;
    QAction *m_toggleFullscreenAction = nullptr;
    QAction *m_takeSnapshotAction = nullptr;
    QAction *m_showMediaInformationAction = nullptr;
    QAction *m_showLogFileViewerAction = nullptr;
    QAction *m_togglePlaylistAction = nullptr;
    QAction *m_toggleStatusBarAction = nullptr;
    QAction *m_toggleTagsAction = nullptr;
    QAction *m_toggleVideoControlsAction = nullptr;
    QAction *m_toggleCumshotMarkersAction = nullptr;
    QAction *m_toggleCyanMarkersAction = nullptr;
    QAction *m_toggleDialogMarkersAction = nullptr;
    QAction *m_toggleMagentaMarkersAction = nullptr;
    QAction *m_toggleMarkersAction = nullptr;
    QAction *m_toggleOrangeMarkersAction = nullptr;
    QAction *m_toggleSceneMarkersAction = nullptr;
    QAction *m_toggleStripMarkersAction = nullptr;
    QAction *m_clearRecentFilesAction = nullptr;
    QAction *m_recentFilesSeparator = nullptr;
    QAction *m_recentFileActions[10];
    QAction *m_audioOutputActions[15];
    QAction *m_audioTrackActions[15];
    QAction *m_videoTrackActions[15];
    QAction *m_subtitleTrackActions[15];
    // QAction *m_Action = nullptr;

    bool m_fileLoaded = false;
    int videoTrack;
    int audioOutput;
    int audioTrack;
    int subtitleTrack;
    int m_jumpSmall = 0;
    int m_jumpMedium = 0;
    int m_jumpLarge = 0;
    int m_jumpExtraLarge = 0;

    void setHotkeys();
    void refreshMenuItems();

};
