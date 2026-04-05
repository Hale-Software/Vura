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

#include <xxHash/xxhash.h>

#include <constants.h>
#include <vura-data.h>
#include <vura-helpers.h>
#include <vura-serializer.h>
#include <vura-settings.h>
#include <vura-startup.h>
#include <util/blogger.h>
#include <util/messagebox.h>
#include <media-io/media-functions.h>

#include "../models/playlistmodel.h"
#include "../components/ClickableLabel.h"
#include "../components/ContinuePlaybackRibbon.h"
#include "../components/menubar.h"
#include "../components/videoslider.h"
#include "../components/videocontrolwidget.h"
#include "../components/system-tray.h"
#include "../utility/playlist.h"
#include "../utility/videomarkers.h"
#include "../settings/settingswindow.h"
#include "../dialogs/about.h"
#include "../dialogs/logviewer.h"
#include "../dialogs/updatewindow.h"
#include "../dialogs/mediainformation.h"
#include "../dialogs/helpdialog.h"
#include "../dialogs/convertmediadialog.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class HSettingsWindow;
    friend class HAboutDialog;
    friend class LogViewer;
    friend class HUpdate;
    friend class ConvertMediaDialog;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setMainWindowVisibility(bool state);
    void processOpenParams(int argc, char *argv[]);

    void initApplication();
    void initSystemTrayIcon();
    void initMenuBar();
    void initStatusBar();
    void initVideoControls();
    void initVideoPlayer();
    void initUI();
    void initAudioDevices();
    void openFolderContextMenu(const QString &path);
    void openFileContextMenu(const QString &file);
    void addFileToPlaylistContextMenu(const QString &file);
    void addFolderToPlaylistContextMenu(const QString &path);
    bool isPlayerAvailable() const;
    bool loadPlaylist(const QUrl &url);
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void updateDurationInfo(qint64 currentInfo);
    static QString trackName(const QMediaMetaData &metaData, int index);
    void loadFile(const QString &fileName);
    void setApplicationWindowTitle();
    void setToolTips();
    static void setStyleSheet();
    bool createUserDirs();
    static qint64 fileHash(const QString& filePath);

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


signals:
    void setActiveAudioDevice(const QAudioDevice &audioDevice);
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
    void quitProgram();


private slots:
    void loadSettings();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void tracksChanged();
    void seek(int mseconds);
    void jump(const QModelIndex &index);
    void jumpTo(int mseconds);
    void playlistPositionChanged(int);
    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(float progress);
    void displayErrorMessage();
    void sourceChanged(const QUrl &media);
    void playbackRateChanged(qreal rate);
    void videoFrameChanged(const QVideoFrame &frame);

    void showPlaylistContextMenu(const QPoint &pos);
    void playlistContextMenu_AddFileAction();
    void playlistContextMenu_AddFolderAction();
    void playlistContextMenu_AdvancedOpenAction();
    void playlistContextMenu_SaveAction();
    void playlistContextMenu_PlayVideoAction();
    void playlistContextMenu_StreamVideoAction();
    void playlistContextMenu_SaveVideoAction();
    void playlistContextMenu_InformationVideoAction();
    void playlistContextMenu_ShowFolderVideoAction();
    void playlistContextMenu_RemoveSelectedVideoAction();

    void hideCursor();
    void durationLabel_Clicked();
    void systemTray_Clicked();
    void systemTray_Hide(bool hiding);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void changeEvent(QEvent *event) override;
    bool event(QEvent *event) override;

private:
    Ui::MainWindow *ui;
    Blogger* blog;
    QList<ApplicationData> applicationData;
    ApplicationData currentVideoData;
    MediaFunctions *mediaFunctions;
    ContinuePlaybackRibbon *m_continuePlaybackRibbon = nullptr;
    VuraSettings *vuraSettings;
    MenuBar *m_menuBar = nullptr;
    SystemTray *m_systemTrayIcon = nullptr;
    QVideoSink *m_videoSink = nullptr;
    QTimer *timer;
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    Playlist *m_playlist = nullptr;
    QLabel *m_statusLabel = nullptr;
    QStatusBar *m_statusBar = nullptr;
    PlaylistModel *m_playlistModel = nullptr;
    QAbstractItemView *m_playlistView = nullptr;
    QMediaDevices m_mediaDevices;
    QPoint m_pos;
    QString m_trackInfo;
    QString m_statusInfo;
    qint64 m_duration;
    int videoTrack;
    int audioOutput;
    int audioTrack;
    int subtitleTrack;
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

    // VARIABLES
    // =======================================================================================================
    QPointer<VideoControlWidget> m_videoControlWidget;
    VideoSlider *m_videoSlider;
    bool m_sourceLoaded = false;
    bool m_showingPlaylist = false;
    bool m_showingStatusBar = true;
    bool m_showingVideoControls = false;
    bool m_isMuted = false;
    bool m_playlistLoaded = false;
    double m_volume = 50;
    double m_playbackSpeed = 1.0;
    QString m_currentFile;
    QString m_currentFileHash;
    VideoMarkers *m_videoMarkers;
    QMap<QString,QList<double>> m_videoMarkersList;
    QString m_markerValue;
    int m_markerIndex;
    int m_inMarker;
    int m_outMarker;
    bool m_showingMarkers = true;
    bool m_showingCumshotMarkers = true;
    bool m_showingCyanMarkers = true;
    bool m_showingDialogMarkers = true;
    bool m_showingMagentaMarkers = true;
    bool m_showingOrangeMarkers = true;
    bool m_showingSceneMarkers = true;
    bool m_showingStripMarkers = true;
    bool m_playlistLoopAll = true;
    bool m_playlistLoopOne = false;
    bool m_playlistLoopNone = false;
    bool m_showingVideoResolution = false;
    bool m_durationLabelShowRemainingTime = false;
    QString m_videoResolution;

};

