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

#include <QMainWindow>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMediaDevices>
#include <QWidget>
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QMediaMetaData>
#include <QStandardPaths>
#include <QStatusBar>
#include <QAudioBufferOutput>
#include <QString>
#include <QMenuBar>
#include <QDebug>
#include <QUrl>
#include <QtMath>
#include <QPointer>
#include <QResizeEvent>
#include <QTimer>
#include <QGuiApplication>
#include <QScreen>
#include <QDialog>
#include <QFileInfo>
#include <QDesktopServices>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QVideoSink>
#include <QVideoFrame>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QItemSelectionModel>

#include <cmath>
#include <limits>
#include <iostream>

//#include <winsparkle.h>

#include <libvura/constants.h>
#include <libvura/data/video-markers.h>
#include <libvura/helpers.h>
#include <libvura/settings.h>
#include <libvura/util/blogger.h>
#include <libvura/util/messagebox.h>
#include <libvura/media-io/media-functions.h>
#include <libvura/ErrorService.h>
#include <libvura/playlist/PlaylistModel.h>
#include <libvura/playlist/PlaylistDelegate.h>
#include <libvura/playlist/MediaItem.h>

#include "ClickableLabel.h"
#include "ContinuePlaybackRibbon.h"
#include "menubar.h"
#include "videoslider.h"
#include "videocontrolwidget.h"
#include "system-tray.h"
#include "settingswindow.h"
#include "about.h"
#include "logviewer.h"
#include "MarkerEditDialog.h"
#include "updatewindow.h"
#include "mediainformation.h"
#include "helpdialog.h"
#include "convertmediadialog.h"
#include "errordialog.h"
#include "WindowsUpdater.h"
#include "EmptyStateWidget.h"
#include "videoslider-container.h"

#include <QVideoWidget>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class HSettingsWindow;
    friend class HAboutDialog;
    friend class LogViewer;
    friend class MarkerEditDialog;
    friend class HUpdate;
    friend class ConvertMediaDialog;
    friend class ErrorDialog;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //virtual void showEvent(QShowEvent *event);

    void setMainWindowVisibility(bool state);
    void processOpenParams(int argc, char *argv[]);

    void initApplication();
    void initSystemTrayIcon();
    void initMenuBar();
    void initStatusBar();
    void initVideoControls();
    void initVideoPlayer();
    void initUI();
    void initVideoSlider();
    void openFolderContextMenu(const QString &path);
    void openFileContextMenu(const QString &file);
    void addFileToPlaylistContextMenu(const QString &file) const;
    void addFolderToPlaylistContextMenu(const QString &path);
    bool isPlayerAvailable() const;
    bool loadPlaylist(const QUrl &url);
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    static QString trackName(const QMediaMetaData &metaData, int index);
    void loadFile(const QString &fileName);
    void setApplicationWindowTitle();
    static void setStyleSheet();
    bool initApplicationDirs();
    bool initUserDirs();
    void updateMarkerMenuItems();
    VuraVideoMarker findNearestVisibleMarker(double sliderPercent, double markerRange) const;
    double getSliderPercent() const;

public slots:
    void showMediaInformation();
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
    void toggleMarkers(const QString &markerType);
    void showLogFileViewer();
    void toggleVideoControls();
    void togglePlayPause() const;
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
    void editSelectedMarker();
    void clearMarkers();
    void clearInMarker();
    void clearOutMarker();
    void goToInMarker() const;
    void goToOutMarker();
    void createSubclip();
    void testFunction();
    void setLoop(int loopOption);
    void toggleShuffle();
    void clearPlaylist();
    void takeSnapshot();
    void jumpToEnd();
    void showVideoResolution(bool showing);
    void convertSave();
    void streamMedia();
    void continuePlaybackRibbon(bool con);
    void markerEdited(const VuraVideoMarker &videoMarker);
    void markerDeleted(const VuraVideoMarker &videoMarker);
    void getPrevMarker(const VuraVideoMarker &videoMarker);
    void getNextMarker(const VuraVideoMarker &videoMarker);
    void renameFile(const QString &newFileName);
    void checkForUpdates();
    void updateAvailable(const QString& versionString, const QString& releaseDateString, const QString &downloadUrl, const QString& changelog);
    void updateNotAvailable();
    void requestFileImport();
    void filesDropped(const QStringList &filePaths);

    // Video Slider
    void sliderPressed(bool pressed);

    //void initWinSparkle();
    //void checkForUpdates();

    void onCurrentPlaylistItemChanged(const QModelIndex &current, const QModelIndex &previous);

signals:
    //void windowWasShown();
    void setActiveAudioDevice(const QAudioDevice &device);
    void setActiveAudioTrack(int track);
    void setActiveVideoTrack(int track);
    void setActiveSubtitleTrack(int track);
    void updateAudioOutputs(QList<QAudioDevice> audioDevices);
    void updateAudioTracks(QList<QMediaMetaData> audioTracks);
    void updateVideoTracks(QList<QMediaMetaData> videoTracks);
    void updateSubtitleTracks(QList<QMediaMetaData> subtitleTracks);
    void updateRecentFiles();
    void setPlayerStatus(bool loaded);
    void setMuted(bool mute);
    void setMarkerShowing(QString type, bool showing);
    void setPlaylistShowing(bool showing);
    void setStatusBarShowing(bool showing);
    void setVideoControlsShowing(bool showing);
    void refreshSettings();
    void setOverrideWindowsHotkeys(bool value);
    void setClearSelectedMarkerEnabled(bool enabled);
    void updateVideoSlider();
    void quitProgram();


private slots:
    void loadSettings();
    void positionChanged(qint64 progress);
    void tracksChanged();
    void seek(int mseconds) const;
    void jump(const QModelIndex &index);
    void jumpTo(int mseconds);
    void playlistPositionChanged(int) const;
    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(float progress);
    void displayErrorMessage();
    void sourceChanged(const QUrl &media);
    void videoFrameChanged(const QVideoFrame &frame);

    void showPlaylistTableContextMenu(const QPoint &pos);
    //void showPlaylistContextMenu(const QPoint &pos);
    //void playlistContextMenu_AddFileAction();
    //void playlistContextMenu_AddFolderAction();
    //void playlistContextMenu_AdvancedOpenAction();
    //void playlistContextMenu_SaveAction();
    //void playlistContextMenu_PlayVideoAction();
    //void playlistContextMenu_StreamVideoAction();
    //void playlistContextMenu_SaveVideoAction();
    //void playlistContextMenu_InformationVideoAction();
    //void playlistContextMenu_ShowFolderVideoAction();
    //void playlistContextMenu_RemoveSelectedVideoAction();

    void systemTray_Clicked();
    void systemTray_Hide(bool hiding);

    void finishedUpdatingPlayerPosition();
    void updateEmptyState();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void changeEvent(QEvent *event) override;

private:
    Ui::MainWindow *ui;

    QList<VuraVideoMarker> videoMarkers;
    PlaylistModel *playlistModel = nullptr;
    VideoSlider *m_videoSlider = nullptr;
    QItemSelectionModel *m_playlistSelectionModel = nullptr;
    VideoSliderContainer *m_videoSliderContainer = nullptr;

    //VuraPlaylistModel *m_vuraPlaylistModel = nullptr;
    //PlaylistManager *m_playlistManager = nullptr;
    //Playlist *m_playlist = nullptr;
    //PlaylistModel *m_playlistModel = nullptr;
    //QAbstractItemView *m_playlistView = nullptr;
    WindowsUpdater *windowsUpdater = nullptr;

    ContinuePlaybackRibbon *m_continuePlaybackRibbon = nullptr;
    VuraSettings *vuraSettings;
    MenuBar *m_menuBar = nullptr;
    SystemTray *m_systemTrayIcon = nullptr;
    QVideoSink *m_videoSink = nullptr;
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QLabel *m_statusLabel = nullptr;
    QStatusBar *m_statusBar = nullptr;
    QMediaDevices m_mediaDevices;
    QPoint m_pos;
    QString m_trackInfo;
    QString m_statusInfo;
    //qint64 m_duration = 0;
    int videoTrack = 0;
    int audioOutput = 0;
    int audioTrack = 0;
    int subtitleTrack = 0;
    bool m_showingCursor = true;
    bool m_fromFullscreen = false;
    qint64 m_lastPosition = 0;
    QString m_currentUser = "UNKNOWN";

    // DIALOGS
    // =======================================================================================================
    QPointer<LogViewer> m_logViewer;
    QPointer<SettingsWindow> m_settingsWindow;
    QPointer<AboutDialog> m_aboutDialog;
    QPointer<UpdateDialog> m_updateDialog;
    QPointer<MediaInformation> m_mediaInformation;
    QPointer<HelpDialog> m_helpDialog;
    QPointer<ConvertMediaDialog> m_convertMediaDialog;
    QPointer<MarkerEditDialog> m_markerEditDialog;
    QPointer<ErrorDialog> m_errorDialog;

    // VARIABLES
    // =======================================================================================================
    QPointer<VideoControlWidget> m_videoControlWidget;
    bool m_sourceLoaded = false;
    bool m_showingPlaylist = false;
    bool m_showingStatusBar = true;
    bool m_showingVideoControls = false;
    bool m_isMuted = false;
    bool m_playlistLoaded = false;
    double m_volume = 50;
    double m_playbackSpeed = 1.0;
    QString m_currentFile;
    QString m_markerValue;
    int m_markerIndex = 0;
    int m_inMarker = 0;
    int m_outMarker = 0;
    bool m_playlistLoopAll = true;
    bool m_playlistLoopOne = false;
    bool m_playlistLoopNone = false;
    bool m_showingVideoResolution = false;
    bool m_durationLabelShowRemainingTime = false;
    QString m_videoResolution;
    QString m_markerFile;

    void updatePlayerPosition();
    bool checkMarkerProximity();
    bool isPreviousMarkerAvailable(const VuraVideoMarker &videoMarker);
    bool isNextMarkerAvailable(const VuraVideoMarker &videoMarker);
    QUrl replaceFilename(QUrl url, const QString &newBaseName);

};

