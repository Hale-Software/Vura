/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

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
#include <QWidget>
#include <QMimeData>
#include <QProgressDialog>

#include <libvura/libvura.h>
#include <libvura/settings.h>
#include <libvura/hotkeys/hotkey-manager.h>
#include <libvura/models/video-marker-record.h>
#include <libvura/helpers.h>
#include <libvura/models/types.h>
#include <libvura/media-engine/engine-factory.h>
#include <libvura/models/subtitle-cue.h>
#include <libvura/platform/platform.h>
#include <libvura/subtitles/subtitle-track.h>

#include <utility>

#include "HelpDialog.h"
#include "AboutDialog.h"
#include "UpdateDialog.h"
#include "SettingsDialog.h"
#include "FeedbackDialog.h"
#include "LogViewerDialog.h"
#include "MarkerEditDialog.h"
#include "ConvertMediaDialog.h"
#include "MediaInformationDialog.h"
#include "UpdateChecker.h"

#include "VideoMarkerController.h"
#include "VideoSlider.h"
#include "PlaylistWidget.h"
#include "SystemTrayWidget.h"
#include "VideoSliderWidget.h"
#include "VideoControlWidget.h"
#include "ContinuePlaybackWidget.h"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QComboBox;
class QDockWidget;
class QLabel;
class QListView;
class QMenu;
class QToolButton;
class QTimer;
QT_END_NAMESPACE

class MediaController;
class SeekSlider;
class SleepInhibitor;
class VideoStage;
class RecentFilesMenu;


namespace Ui {
class VuraMainWindow;
}


class VuraMainWindow : public QMainWindow
{
    Q_OBJECT

    friend class HelpDialog;
    friend class AboutDialog;
    friend class UpdateDialog;
    friend class SettingsDialog;
    friend class FeedbackDialog;
    friend class LogViewerDialog;
    friend class MarkerEditDialog;
    friend class ConvertMediaDialog;
    friend class MediaInformationDialog;

public:
    explicit VuraMainWindow(MediaController *controller, QWidget *parent = nullptr);
    ~VuraMainWindow() override;

    void maximized();
    void setMainWindowVisibility(bool state);
    void openFile(const QString &file);
    void openFolder(const QString &path);
    void openNetworkStream(const QString& networkUrl);
    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    void closeEvent(QCloseEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void changeEvent(QEvent *event) override;

signals:
    void quitProgram();

private slots:
    // File Menu
    void actionFileOpenFile();
    void actionFileOpenMultipleFiles();
    void actionFileOpenFolder();
    void actionOpenNetworkStream();
    void actionFileOpenPlaylist();
    void actionFileSavePlaylist();
    void actionEmergencyClose();
    void actionShowConvertMedia();
    void actionExit();

    // View Menu
    void actionViewTogglePlaylist();
    void actionShowSettings();
    void actionToggleVideoControls();
    void actionViewToggleStatusBar() const;
    void actionViewToggleVideoResolution();
    void actionViewMediaInformation();

    // Playback Menu

    // Markers Menu
    void actionMarkersClearIn();
    void actionMarkersClearInOut();
    void actionMarkersClearMarkers();
    void actionMarkersClearOut();
    void actionMarkersEditSelectedMarker();
    void actionMarkersGoToIn();
    void actionMarkersGoToOut();
    void actionMarkersMarkIn();
    void actionMarkersMarkOut();

    // Audio Menu

    // Video Menu
    void actionToggleFullscreen();

    // Subtitles Menu
    void actionSubtitlesOpenSubtitlesFile();
    void actionSubtitlesToggleSubtitles(bool checked);

    // Tools Menu
    void actionTestFunction();

    // Help Menu
    void actionHelpCheckForUpdates();
    void actionShowLogViewer();


public slots:
    void restartApplication();
    void openPaths(const QList<QUrl> &urls);
    void stateChanged(media::PlaybackState state);
    void sourceChanged(const QUrl &source);
    void errorOccurred(const QString &errorMessage);
    void hideVideoSlider();
    void resetVideoSliderVisibility();
    void crashReportScanFinished(bool crashFileExists);
    void crashReportUploadStarted();
    void crashReportUploadFinished(bool success, const QString& message);
    void continuePlaybackDeclined();
    void continuePlaybackAccepted(qint64 savedPosition);
    void continuePlaybackDelete();
    void systemTray_Clicked();
    void systemTray_Hide(bool hiding);
    void updaterErrorOccurred(QString errorMessage);
    void updateAvailable(bool available);
    void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updateDownloadFinished(bool success, const QString &message);

private:
    void initSystemTray();
    void buildMenus();
    void buildPlaylistDock();
    void initUI();
    void connectController();
    void initMisc();

    void applyCapabilities(const media::Capabilities &capabilities);
    void rebuildTrackMenus();
    void rebuildAudioDeviceMenu();

    void setTrackInfo(const QString &trackInfo);
    static QString trackName(const QMediaMetaData &metaData, int index);
    void setApplicationWindowTitle();

    void updateMarkerMenuItems();

    void showResumeOverlay(qint64 savedPosition);
    void addMedia(const QList<QUrl> &mediaList);

    template <typename Dialog, typename... Args>
    Dialog *showDialog(QPointer<Dialog> &slot, Args &&...args)
    {
        if (slot)
            slot->close();

        slot = new Dialog(std::forward<Args>(args)...);
        slot->setAttribute(Qt::WA_DeleteOnClose);
        slot->show();
        return slot;
    }

    void openMarkerEditor(const VideoMarkerRecord &marker);

    Ui::VuraMainWindow *ui;
    SystemTrayWidget *m_systemTray = nullptr;
    MediaController *m_controller = nullptr;
    SleepInhibitor *m_sleepInhibitor = nullptr;
    VideoStage *m_stage = nullptr;
    QDockWidget *m_playlistDock = nullptr;
    PlaylistWidget *m_playlistWidget = nullptr;
    VideoSliderWidget *m_videoSliderWidget = nullptr;
    VideoControlWidget *m_videoControlWidget = nullptr;
    ContinuePlaybackWidget *m_continuePlaybackWidget = nullptr;
    VideoMarkerController *m_videoMarkerController = nullptr;
    VideoSlider *m_videoSlider = nullptr;
    SubtitleTrack* m_subtitleTrack = nullptr;
    SubtitleCue* m_currentCue = nullptr;
    CrashReporter *m_crashReporter = nullptr;
    UpdateManager *m_updateManager = nullptr;
    RecentFilesMenu *m_recentFiles = nullptr;
    QProgressDialog *m_updateProgressDialog = nullptr;

    QTimer *m_videoSliderHideTimer = nullptr;
    QTimer *m_continuePlaybackBannerTimer = nullptr;
    QUrl m_currentSource;
    QPointer<QFrame> m_resumeOverlay;
    QHash<QString, QAction *> m_markerToggleActions;

    bool m_replacePlaylist = false;
    bool m_wasMaximized = false;
    bool m_subtitlesEnabled = false;
    bool m_showingVideoControls = false;
    bool m_wasPlaylistShowing = false;
    qint64 m_lastPosition = 0;
    qint64 m_subtitleOffsetMs = 0;
    int m_inMarker = 0;
    int m_outMarker = 0;
    QString m_trackInfo;
    QString m_statusInfo;

    QPointer<HelpDialog> m_helpDialog;
    QPointer<AboutDialog> m_aboutDialog;
    QPointer<UpdateDialog> m_updateDialog;
    QPointer<SettingsDialog> m_settingsDialog;
    QPointer<FeedbackDialog> m_feedbackDialog;
    QPointer<LogViewerDialog> m_logViewerDialog;
    QPointer<MarkerEditDialog> m_markerEditDialog;
    QPointer<ConvertMediaDialog> m_convertMediaDialog;
    QPointer<MediaInformationDialog> m_mediaInformationDialog;

};
