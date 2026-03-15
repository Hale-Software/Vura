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

#include <xxHash/xxhash.h>


#include "../models/playlistmodel.h"
#include "../components/markerslider.h"
#include "../components/videoslider.h"
#include "../components/videocontrolwidget.h"
#include "../utility/logger.h"
#include "../utility/hdata.h"
#include "../utility/playlist.h"
#include "../utility/qt-wrappers.h"
#include "../utility/startup.h"
#include "../settings/settingswindow.h"
#include "../dialogs/editmarker.h"
#include "../dialogs/about.h"
#include "../dialogs/logviewer.h"
#include "../dialogs/updatewindow.h"
#include "hvideowidget.h"


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

    void openedWithFile(QString file);
    bool isPlayerAvailable() const;

public slots:
    void markersChanged(const QString &markerName, const double &markerTime, const QString &markerType);
    void markerDeleted(const double &markerTime);

signals:
    void h_keyPress(QKeyEvent *event);
    void fullScreenChanged(bool fullScreen);
    void refreshSettings();
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

#pragma region Menu Item Slots

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
    void saveAll();
    void saveAs();
    void savePlaylist();
    void saveACopy();
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
    void streamVideoFromStash();

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
    void testFunction();

    // Subtitle Menu
    void openSubtitleFile();
    void toggleSubtitles();

    // Help Menu
    void showAbout();
    void showHelp();
    void showUpdates();

#pragma endregion

    void videoControl_Fullscreen();
    void videoControl_TogglePlaylist();
    void videoControl_LoopAll();
    void videoControl_LoopOne();
    void videoControl_LoopNone();
    void videoControl_Shuffle();

    void showPlaylistContextMenu(const QPoint &pos);
    void playlistContextMenu_AddFileAction();
    void playlistContextMenu_AddFolderAction();
    void playlistContextMenu_AdvancedOpenAction();
    void playlistContextMenu_SaveAction();
    void playlistContextMenu_ClearAction();
    void playlistContextMenu_ShuffleAction();
    void playlistContextMenu_PlayVideoAction();
    void playlistContextMenu_StreamVideoAction();
    void playlistContextMenu_SaveVideoAction();
    void playlistContextMenu_InformationVideoAction();
    void playlistContextMenu_ShowFolderVideoAction();
    void playlistContextMenu_RemoveSelectedVideoAction();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    Ui::MainWindow *ui;
    Logger* m_hLogger;
    QByteArray h_geometry;
    Qt::WindowStates h_windowState;
    QTimer *h_onScreenMessageTimer;

    // WINDOWS
    // =======================================================================================================
    QPointer<LogViewer> h_logviewer;
    QPointer<SettingsWindow> h_settingsWindow;
    QPointer<EditMarkerDialog> h_editMarkerDialog;
    QPointer<AboutDialog> h_aboutDialog;
    QPointer<UpdateDialog> h_updateDialog;

    // VARIABLES
    // =======================================================================================================
    QPointer<VideoControlWidget> h_videoControlWidget;
    VideoSlider *m_videoSlider;
    //QTableWidget *h_playlistWidget;
    bool h_playing = false;
    bool h_showingPlaylist = false;
    bool h_showingStatusBar = true;
    bool h_showingVideoControls = false;
    bool h_showingSubtitles = false;
    bool h_isMuted = false;
    bool h_playlistLoaded = false;
    bool h_subtitlesLoaded = false;
    float h_volume = 50;
    float h_playbackSpeed = 1.0;
    QString currentFile;
    QString currentFileHash;
    QMap<QString,QList<double>> markerMap;
    QString m_markerValue;
    int markerIndex;
    HData *hData;
    int h_inMarker;
    int h_outMarker;
    bool h_showingMarkers = true;
    bool h_showingCumshotMarkers = true;
    bool h_showingCyanMarkers = true;
    bool h_showingDialogMarkers = true;
    bool h_showingMagentaMarkers = true;
    bool h_showingOrangeMarkers = true;
    bool h_showingSceneMarkers = true;
    bool h_showingStripMarkers = true;

    // SETTINGS
    bool m_rememberWindowSize = true;
    bool m_hashFile = false;
    int m_jumpSmall = 0;
    int m_jumpMedium = 0;
    int m_jumpLarge = 0;
    int m_jumpExtraLarge = 0;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    Playlist *m_playlist = nullptr;
    bool m_playlistLoopAll = true;
    bool m_playlistLoopOne = false;
    bool m_playlistLoopNone = false;
    QLabel *m_statusLabel = nullptr;
    QStatusBar *m_statusBar = nullptr;
    PlaylistModel *m_playlistModel = nullptr;
    QAbstractItemView *m_playlistView = nullptr;
    QString m_trackInfo;
    QString m_statusInfo;
    qint64 m_duration;
    QMediaDevices m_mediaDevices;

    AudioLevelMeter *m_audioLevelMeter = nullptr;
    QAudioBufferOutput *m_audioBufferOutput = nullptr;
    QPushButton *m_fullScreenButton = nullptr;
    QPushButton *m_pitchCompensationButton = nullptr;
    QComboBox *m_audioOutputCombo = nullptr;
    QComboBox *m_audioTracks = nullptr;
    QComboBox *m_videoTracks = nullptr;
    QComboBox *m_subtitleTracks = nullptr;
    QMenu *h_audioMenu = nullptr;
    QAction *h_recentFileActions[10];
    QAction *h_clearRecentFilesAction;
    QAction *h_recentFilesSeparator;
    QAction *h_audioOutputActions[15];
    QAction *h_audioTrackActions[15];
    QAction *h_videoTrackActions[15];
    QAction *h_subtitleTrackActions[15];
    int videoTrack;
    int audioOutput;
    int audioTrack;
    int subtitleTrack;
    QPoint m_pos;


    // STARTUP FUNCTIONS
    // =======================================================================================================
    void configureApplication();
    void configureStatusBar();
    void configureVideoWidget();
    void configureVideoWidgetConnections();
    void configureUI();
    void configureMenuItems();
    void configureHotkeys();
    void configureDefaultHotkeys();
    void createRecentFileActions();
    void updateRecentFileActions();
    void createAudioOutputActions();
    void updateAudioOutputActions();
    void createAudioTrackActions();
    void updateAudioTrackActions();
    void createVideoTrackActions();
    void updateVideoTrackActions();
    void createSubtitleTrackActions();
    void updateSubtitleTrackActions();


    // FUNCTIONS
    // =======================================================================================================
    bool isPlaylist(const QUrl &url);
    QString getMarker(const double &markerTime);
    void saveMediaFilterList(const QStringList& filterList);
    QStringList loadMediaFilterList();
    bool loadPlaylist(const QUrl &url);

    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);
    void updateDurationInfo(qint64 currentInfo);
    QString trackName(const QMediaMetaData &metaData, int index);
    void showNotImplemented_Message();
    void loadFile(const QString &fileName);
    QString strippedFileName(const QString &fileName);
    QString timestampString(qint64 position);


    // VIDEO EDITING FUNCTIONS
    // =======================================================================================================
    void showErrorMessage(const QString &message);
    void extractSubclipFromVideo();
    QString createTimestampString(qint64 pos);
    QString generateSubclipFilenameWithIncrement(const QString& directoryPath, const QString& baseFileName, const QString& extension);

    qint64 fileHash(const QString& filePath);

};

