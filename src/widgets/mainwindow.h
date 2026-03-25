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

#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMediaDevices>
#include <QWidget>
#include <QMainWindow>
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <QShortcut>
#include <QKeySequence>
#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QBoxLayout>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QStandardPaths>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QAudioBufferOutput>
#include <QString>
//#include <QVideoWidget>
#include <QMenuBar>
#include <QMenu>
#include <QDebug>
#include <QUrl>
#include <QtMath>
#include <QDirIterator>
#include <QPointer>
#include <QPixmap>
#include <QActionGroup>
#include <QProcess>
#include <QResizeEvent>
#include <QTimer>
#include <QGuiApplication>
#include <QScreen>
#include <QInputDialog>
#include <QTimeEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QKeyEvent>
#include <QTableWidget>
#include <QFileInfo>
#include <QDesktopServices>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QVideoSink>
#include <QVideoFrame>

#include <xxHash/xxhash.h>

#include "../constants.h"
#include "../models/playlistmodel.h"
#include "../components/ClickableLabel.h"
#include "../components/ContinuePlaybackRibbon.h"
#include "../components/menubar.h"
#include "../components/videoslider.h"
#include "../components/videocontrolwidget.h"
#include "../components/system-tray.h"
#include "../utility/logger.h"
#include "../utility/playlist.h"
#include "../utility/qt-wrappers.h"
#include "../utility/startup.h"
#include "../utility/videomarkers.h"
#include "../settings/settingswindow.h"
#include "../dialogs/editmarker.h"
#include "../dialogs/about.h"
#include "../dialogs/logviewer.h"
#include "../dialogs/updatewindow.h"
#include "../dialogs/mediainformation.h"


QT_BEGIN_NAMESPACE
class QAbstractItemView;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QComboBox;
class QSlider;
class QStatusBar;
class QVideoWidget;
class QAudioBufferOutput;
QT_END_NAMESPACE

class PlaylistModel;
class AudioLevelMeter;

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

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openFolderContextMenu(const QString &path);
    void openFileContextMenu(const QString &file);
    void addFileToPlaylistContextMenu(const QString &file);
    void addFolderToPlaylistContextMenu(const QString &path);
    bool isPlayerAvailable() const;

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
    void addInMarker();
    void addOutMarker();
    void createSubclip();
    void testFunction();
    void setLoop(int loopOption);
    void toggleShuffle();
    void clearPlaylist();
    void takeSnapshot();


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
    void metaDataChanged();
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
    Logger* m_hLogger;
    MenuBar *m_menuBar = nullptr;
    SystemTray *m_systemTrayIcon = nullptr;
    QVideoSink *m_videoSink = nullptr;
    ContinuePlaybackRibbon *m_continuePlaybackRibbon = nullptr;
    QTimer *timer;
    int m_x = 0;
    int m_y = 0;
    bool m_showingCursor = true;
    QString m_currentUser = "UNKNOWN";

    // WINDOWS
    // =======================================================================================================
    QPointer<LogViewer> m_logViewer;
    QPointer<SettingsWindow> m_settingsWindow;
    QPointer<AboutDialog> m_aboutDialog;
    QPointer<UpdateDialog> m_updateDialog;
    QPointer<MediaInformation> m_mediaInformation;

    // VARIABLES
    // =======================================================================================================
    QPointer<VideoControlWidget> m_videoControlWidget;
    VideoSlider *m_videoSlider;
    bool m_sourceLoaded = false;
    bool m_playing = false;
    bool m_showingPlaylist = false;
    bool m_showingStatusBar = true;
    bool m_showingVideoControls = false;
    bool m_showingSubtitles = false;
    bool m_isMuted = false;
    bool m_playlistLoaded = false;
    bool m_subtitlesLoaded = false;
    float m_volume = 50;
    float m_playbackSpeed = 1.0;
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
    bool m_setOverrideWindowsHotkeys = true;

    // SETTINGS
    // =======================================================================================================
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

    //
    // =======================================================================================================
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    Playlist *m_playlist = nullptr;
    QLabel *m_statusLabel = nullptr;
    QStatusBar *m_statusBar = nullptr;
    PlaylistModel *m_playlistModel = nullptr;
    QAbstractItemView *m_playlistView = nullptr;
    QString m_trackInfo;
    QString m_statusInfo;
    qint64 m_duration;
    QMediaDevices m_mediaDevices;
    int videoTrack;
    int audioOutput;
    int audioTrack;
    int subtitleTrack;
    QPoint m_pos;

    // FUNCTIONS
    // =======================================================================================================
    bool isPlaylist(const QUrl &url);
    bool loadPlaylist(const QUrl &url);
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void updateDurationInfo(qint64 currentInfo);
    QString trackName(const QMediaMetaData &metaData, int index);
    void showNotImplemented_Message();
    void loadFile(const QString &fileName);
    QString strippedFileName(const QString &fileName);
    QString timestampString(qint64 position);
    void setApplicationWindowTitle();
    void setToolTips();
    void setStyleSheet();
    bool createUserDirs();


    // VIDEO EDITING FUNCTIONS
    // =======================================================================================================
    void showErrorMessage(const QString &message);
    void extractSubclipFromVideo();
    QString createTimestampString(qint64 pos);
    QString generateSubclipFilenameWithIncrement(const QString& directoryPath, const QString& baseFileName, const QString& extension);

    qint64 fileHash(const QString& filePath);

};

