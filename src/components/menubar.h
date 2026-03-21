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

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QAudioDevice>
#include <QMediaMetaData>
#include <QSettings>
#include <QMap>
#include <QVariant>
#include <QLocale>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimeEdit>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QStringList>
#include <QStandardPaths>
#include <QDirIterator>
#include <QFile>
#include <QLabel>
#include <QKeySequence>
#include <QDebug>


class MenuBar : public QMenuBar {
    Q_OBJECT

public:
    explicit MenuBar(QWidget *parent = nullptr);

public slots:
    void setPlayerStatus(bool loaded);
    void setActiveAudioDevice(const QAudioDevice &audioDevice);
    void setActiveAudioTrack(int track);
    void setActiveVideoTrack(int track);
    void setActiveSubtitleTrack(int track);
    void refreshSettings();
    void updateAudioOutputs(QList<QAudioDevice> audioDevices);
    void updateAudioTracks(QList<QMediaMetaData> audioTracks);
    void updateVideoTracks(QList<QMediaMetaData> videoTracks);
    void updateSubtitleTracks(QList<QMediaMetaData> subtitleTracks);
    void updateRecentFiles();
    void setMuted(bool mute);
    void markerShowing(QString type, bool showing);
    void playlistShowing(bool showing);
    void statusBarShowing(bool showing);
    void videoControlsShowing(bool showing);

signals:
    void showPreferences();
    void showAbout();
    void showHelp();
    void showUpdates();
    void showFeedback();
    void emergencyCollapse();
    void exitApplication();
    void openFiles(const QStringList &fileList, bool localFile = true);
    void closeFile();
    void closeAllFiles();
    void openFolder(const QString &folderPath);
    void saveFile(const QString &filePath);
    void savePlaylist(const QString &filePath, const QString &type);
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
    void togglePlayPause();
    void nextVideo();
    void previousVideo();
    void changePlaybackSpeed(double mrate);
    void setPlaybackSpeedNormal();
    void videoSeek(int mseconds);
    void videoJumpToTime(int position);
    void restartVideo();
    void changeVolume(double mvolume);
    void toggleMute();
    void toggleFullscreen();
    void setAudioOutput(const QAudioDevice &moutput);
    void setAudioTrack(int mtrack);
    void setVideoTrack(int mtrack);
    void setSubtitleTrack(int mtrack);
    void addMarker(const QString &markerType);
    void nextMarker();
    void previousMarker();
    void clearSelectedMarker();
    void clearMarkers();
    void clearInMarker();
    void clearOutMarker();
    void goToInMarker();
    void goToOutMarker();
    void createSubclip();
    void takeSnapshot();
    void testFunction();

private slots:
    // File Menu
    void closeFile_Clicked();
    void closeAll_Clicked();
    void emergencyCollapse_Clicked();
    void exit_Clicked();
    void openFile_Clicked();
    void openMultipleFiles_Clicked();
    void openFolder_Clicked();
    void openNetworkStream_Clicked();
    void save_Clicked();
    void saveAs_Clicked();
    void savePlaylist_Clicked();
    void saveACopy_Clicked();
    void importProject_Clicked();
    void importCaptions_Clicked();
    void importMarkers_Clicked();
    void exportCaptions_Clicked();
    void exportClips_Clicked();
    void exportMarkers_Clicked();
    void exportMedia_Clicked();
    void openRecent_Clicked();
    void showPreferences_Clicked();
    void openRecentFile_Clicked();
    void clearRecentFiles_Clicked();

    // View Menu
    void togglePlaylist_Clicked();
    void toggleStatusBar_Clicked();
    void toggleMarkers_Clicked();
    void toggleCumshotMarkers_Clicked();
    void toggleCyanMarkers_Clicked();
    void toggleDialogMarkers_Clicked();
    void toggleMagentaMarkers_Clicked();
    void toggleOrangeMarkers_Clicked();
    void toggleSceneTransitionMarkers_Clicked();
    void toggleStripMarkers_Clicked();
    void showLogFileViewer_Clicked();
    void toggleVideoControls_Clicked();
    void showMediaInformation_Clicked();
    void toggleTags_Clicked();

    // Playback Menu
    void togglePlayPause_Clicked();
    void nextVideo_Clicked();
    void nextFrame_Clicked();
    void previousVideo_Clicked();
    void previousFrame_Clicked();
    void fasterSpeed_Clicked();
    void fasterFineSpeed_Clicked();
    void normalSpeed_Clicked();
    void slowerSpeed_Clicked();
    void slowerFineSpeed_Clicked();
    void jumpBackwardSmall_Clicked();
    void jumpBackwardMedium_Clicked();
    void jumpBackwardLarge_Clicked();
    void jumpBackwardExtraLarge_Clicked();
    void jumpForwardSmall_Clicked();
    void jumpForwardMedium_Clicked();
    void jumpForwardLarge_Clicked();
    void jumpForwardExtraLarge_Clicked();
    void jumpToTime_Clicked();
    void restartVideo_Clicked();

    // Audio Menu
    void increaseVolume_Clicked();
    void decreaseVolume_Clicked();
    void toggleMute_Clicked();
    void selectAudioOutput_Clicked();
    void selectAudioTrack_Clicked();

    // Video Menu
    void toggleFullscreen_Clicked();
    void takeSnapshot_Clicked();
    void selectVideoTrack_Clicked();
    void selectSubtitleTrack_Clicked();

    // Markers Menu
    void addMarker_Clicked();
    void goToNextMarker_Clicked();
    void goToPreviousMarker_Clicked();
    void clearSelectedMarker_Clicked();
    void clearAllMarkers_Clicked();
    void editMarker_Clicked();
    void addSceneTransitionMarker_Clicked();
    void addCumshotMarker_Clicked();
    void addStripMarker_Clicked();
    void addDialogMarker_Clicked();
    void addCyanMarker_Clicked();
    void addMagentaMarker_Clicked();
    void addOrangeMarker_Clicked();
    void addInMarker_Clicked();
    void addOutMarker_Clicked();
    void clearInMarker_Clicked();
    void clearOutMarker_Clicked();
    void clearInAndOutMarker_Clicked();
    void goToInMarker_Clicked();
    void goToOutMarker_Clicked();

    // Tools Menu
    void createSubclip_Clicked();
    void streamVideoFromStash_Clicked();
    void testFunction_Clicked();

    // Subtitle Menu
    void openSubtitleFile_Clicked();
    void toggleSubtitles_Clicked();

    // Help Menu
    void showAbout_Clicked();
    void showHelp_Clicked();
    void showUpdates_Clicked();
    void showFeedback_Clicked();

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

    QAudioDevice m_activeAudioDevice;

    bool m_initialised = false;
    bool m_fileLoaded = false;
    int m_activeVideoTrack = 0;
    int m_activeAudioOutput = 0;
    int m_activeAudioTrack = 0;
    int m_activeSubtitleTrack= 0;
    int m_frameWalkTime = 0;
    double m_playbackSpeedAdjustment = 0.0;
    double m_playbackSpeedFineAdjustment = 0.0;
    double m_volumeStep = 0.10;
    int m_jumpSmall = 0;
    int m_jumpMedium = 0;
    int m_jumpLarge = 0;
    int m_jumpExtraLarge = 0;
    QString m_stashServerUrl = "";
    int m_maxRecentFiles = 0;

    void setHotkeys();
    void refreshMenuItems();
    QString trackName(const QMediaMetaData &metaData, int index);

};
