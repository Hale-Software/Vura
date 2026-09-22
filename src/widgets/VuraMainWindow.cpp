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

#include "VuraMainWindow.h"
#include "ui_VuraMainWindow.h"

#include "PlaylistEmptyStateWidget.h"
#include "RecentFilesMenu.h"

#include <ui-config.h>

#include <libvura/io/playlist-io.h>
#include <libvura/logging/logger.h>
#include <libvura/media-controller.h>
#include <libvura/models/playlist.h>
#include <libvura/media-engine/video-stage.h>

#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QListView>
#include <QMenuBar>
#include <QMimeData>
#include <QSettings>
#include <QSlider>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtMath>
#include <QDir>
#include <QDirIterator>
#include <QTimer>


#ifdef VURA_HAVE_QTMULTIMEDIA
#include <QAudio>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#endif


VuraMainWindow::VuraMainWindow(MediaController *controller, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::VuraMainWindow),
      m_controller(controller),
      m_sleepInhibitor(new SleepInhibitor(this))
{
    ui->setupUi(this);
    addActions(findChildren<QAction*>());
    setAcceptDrops(true);

    m_stage = new VideoStage(this);
    ui->verticalLayout->addWidget(m_stage);

    m_controller->setVideoOutputProvider([this](media::Engine *engine) { return m_stage->outputFor(engine); });

    initSystemTray();
    buildMenus();
    buildPlaylistDock();
    initUI();
    connectController();
    initMisc();

    applyCapabilities(m_controller->capabilities());

    HotkeyManager::instance()->registerWindow(this);
    HotkeyManager::instance()->load();

    qCDebug(Core) << "Application Initialized!";
    qCInfo(Core) << "Vura Version: " << VURA_VERSION_STRING;
}

VuraMainWindow::~VuraMainWindow() = default;

// Application Events
void VuraMainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    m_videoMarkerController->saveVideoMarkers();
    event->accept();
}

bool VuraMainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    if (HotkeyManager::instance()->handleNativeEvent(eventType, message, result))
        return true;

#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        const auto msg = static_cast<MSG *>(message);
        if (msg->message == WM_NCLBUTTONDBLCLK)
        {
            resize(887, 530);
            return true;
        }
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void VuraMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void VuraMainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    addMedia(urls);
    event->acceptProposedAction();
}

void VuraMainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Escape:
            if (isFullScreen()) {
                actionToggleFullscreen();
                return;
            }
            break;

        default:
            break;
    }
    QMainWindow::keyPressEvent(event);
}

void VuraMainWindow::changeEvent(QEvent *event)
{
    const QSettings settings;

    if (event->type() == QEvent::WindowStateChange) {
        const auto *stateEvent = dynamic_cast<QWindowStateChangeEvent*>(event);
        if (!(stateEvent->oldState() & Qt::WindowMinimized) && (windowState() & Qt::WindowMinimized)) {
            if (m_controller && settings.value("pausePlaybackWhenMinimized", true).toBool())
                m_controller->pause();
        }
    }
    QMainWindow::changeEvent(event);
}

bool VuraMainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        const auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);

        QPointF localPos = mouseEvent->position();

        this->unsetCursor();
        m_videoSliderWidget->show();
        if (m_controller->playbackState() == media::PlaybackState::Playing) {
            m_videoSliderHideTimer->start();
        }
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        if (m_controller->playbackState() == media::PlaybackState::Playing) {
            m_controller->pause();
        } else if (m_controller->playbackState() == media::PlaybackState::Paused) {
            m_controller->play();
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void VuraMainWindow::maximized()
{
    setWindowState(windowState() | Qt::WindowMaximized);
}

void VuraMainWindow::setMainWindowVisibility(const bool state)
{
    if (state) {
        show();
        showNormal();
        raise();
        activateWindow();
    }
}

void VuraMainWindow::openFile(const QString &file)
{
    if (file.isEmpty()) {
        qDebug() << "No file specified.";
        return;
    }
    qDebug() << "Open file requested. File: " << file;
    addMedia({QUrl::fromLocalFile(file)});
}

void VuraMainWindow::openFolder(const QString &path)
{
    if (path.isEmpty()) {
        qDebug() << "No path specified.";
        return;
    }
    qDebug() << "Open folder requested. Path: " << path;

    QList<QUrl> urls;
    QDirIterator folderIterator(path, QDir::Files | QDir::NoDotAndDotDot);
    while (folderIterator.hasNext()) {
        folderIterator.next();
        urls.append(QUrl::fromLocalFile(folderIterator.filePath()));
    }
    addMedia(urls);
}

void VuraMainWindow::openNetworkStream(const QString& networkUrl)
{
    if (networkUrl.isEmpty()) {
        qDebug() << "No network URL specified.";
        return;
    }
    qDebug() << "Open with network stream requested. Network URL: " << networkUrl;
    m_playlistDock->hide();

    const QString formattedUrl = Helpers::networkUrlFormatter(networkUrl);
    qDebug() << "Cleaned Network URL: " << formattedUrl;
    addMedia({QUrl(formattedUrl)});
}

void VuraMainWindow::stateChanged(const media::PlaybackState state)
{
    if (state == media::PlaybackState::Playing && !m_videoSliderHideTimer->isActive()) {
        m_videoSliderHideTimer->start();
    } else {
        m_videoSliderHideTimer->stop();
        m_videoSliderWidget->show();
        this->unsetCursor();
    }
}

void VuraMainWindow::sourceChanged(const QUrl &source)
{
    qCDebug(Core) << "Source changed to: " << source.toString();

    QSettings settings;

    m_currentSource = source;
    m_videoMarkerController->loadVideoMarkers(source);
    setApplicationWindowTitle();

    // Media Change Alert
    const int showMediaChangeNotification = settings.value("showMediaChangeNotification", 1).toInt();
    switch (showMediaChangeNotification) {
        case 0:
            // Never alert
            break;
        case 1:
            // Alert when minimized
            if (windowState() & Qt::WindowMinimized) {
                QApplication::alert(this);
            }
            break;
        case 2:
            // Always alert
            QApplication::alert(this);
            break;
        default:
            break;
    }

    // Continue playback
    //if (source.isLocalFile()) {
    //    const QByteArray pathBytes = source.toLocalFile().toUtf8();
    //    const QString fileHash = QCryptographicHash::hash(pathBytes, QCryptographicHash::Md5).toHex();
    //
    //    settings.beginGroup("ResumeData");
    //    const qint64 savedPosition = settings.value(fileHash, 0).toLongLong();
    //    settings.endGroup();
    //
    //    if (savedPosition > 5000) {
    //        const int continuePlayback = settings.value("continuePlayback", 1).toInt();
    //        switch (continuePlayback) {
    //            // Never continue
    //            case 0:
    //                break;
    //            // Ask user
    //            case 1:
    //                showResumeOverlay(savedPosition);
    //                break;
    //            // Always continue
    //            case 2:
    //                m_playbackController->seek(savedPosition);
    //                break;
    //            default:
    //                break;
    //        }
    //    }
    //}
}

void VuraMainWindow::errorOccurred(const QString &errorMessage)
{
    qCCritical(Core) << "MediaPlayer Error: " << errorMessage;
    QMessageBox::critical(this, "Media Player Error", errorMessage);
}

void VuraMainWindow::hideVideoSlider()
{
    const QSettings settings;

    if (m_controller->playbackState() != media::PlaybackState::Playing)
        return;

    int autohideSlider = settings.value("autohideSlider", 1).toInt();
    switch (autohideSlider) {
        case 0:
            break;
        case 1:
            if (isFullScreen()) {
                m_videoSliderWidget->hide();
                setCursor(Qt::BlankCursor);
            }
            break;
        case 2:
            if (isMaximized()) {
                m_videoSliderWidget->hide();
                setCursor(Qt::BlankCursor);
            }
            break;
        case 3:
            m_videoSliderWidget->hide();
            setCursor(Qt::BlankCursor);
            break;
        default:
            break;
    }
}

void VuraMainWindow::resetVideoSliderVisibility()
{
    m_videoSliderHideTimer->stop();
    m_videoSliderWidget->show();
    unsetCursor();

    if (m_controller->playbackState() == media::PlaybackState::Playing && !m_videoSliderHideTimer->isActive())
        m_videoSliderHideTimer->start();
}

void VuraMainWindow::actionFileOpenFile()
{
    QSettings settings;
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), Helpers::getLastOpenedDirectory(), "All Files (*)");

    if (!fileName.isEmpty()) {
        Helpers::setLastOpenedDirectory(QFileInfo(fileName).path());
        addMedia({QUrl::fromLocalFile(fileName)});
    }
}

void VuraMainWindow::actionFileOpenMultipleFiles()
{
    QSettings settings;
    QList<QUrl> urls;

    const QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Media Files"), Helpers::getLastOpenedDirectory(), "All Files (*)");

    if (!files.isEmpty()) {
        for (const QString& fileName : files) {
            urls << QUrl::fromLocalFile(fileName);
        }

        const QString& lastFile = files.last();
        Helpers::setLastOpenedDirectory(QFileInfo(lastFile).path());
        addMedia(urls);
    }
}

void VuraMainWindow::actionFileOpenFolder()
{
    QSettings settings;
    QList<QUrl> urls;

    const QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Open Folder"),
            Helpers::getLastOpenedDirectory(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!dir.isEmpty()) {
        const QDir directory(dir);
        QStringList filters;
        filters << "*.mp4" << "*.mkv" << "*.avi" << "*.mp3" << "*.wav" << "*.flac";

        QFileInfoList fileInfoList = directory.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);
        for (const QFileInfo& fileInfo : fileInfoList) {
            urls << QUrl::fromLocalFile(fileInfo.absoluteFilePath());
        }
        Helpers::setLastOpenedDirectory(QFileInfo(dir).path());
        addMedia(urls);
    }
}

void VuraMainWindow::actionFileOpenPlaylist()
{
    QSettings settings;

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        Helpers::getLastOpenedDirectory(),
        "XSPF playlist (*.xspf);;M3U playlist (*.m3u);;M3U8 playlist (*.m3u8);;All Files (*.*)");

    if (!fileName.isEmpty()) {
        Helpers::setLastOpenedDirectory(QFileInfo(fileName).path());

        QString error;
        if (!playlistio::loadInto(m_controller->playlist(), fileName, &error)) {
            qWarning() << "Error loading playlist: " << error;
        }
    }
}

void VuraMainWindow::actionFileSavePlaylist()
{
    QSettings settings;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Playlist As"),
        Helpers::getLastOpenedDirectory(),
        tr("XSPF playlist (*.xspf);;M3U playlist (*.m3u);;M3U8 playlist (*.m3u8);;All Files (*.*)"));

    if (!fileName.isEmpty()) {
        QString error;
        if (!playlistio::saveFrom(m_controller->playlist(), fileName, &error)) {
            qWarning() << "Error saving playlist: " << error;
        }
    }
}

void VuraMainWindow::actionHelpCheckForUpdates()
{
    m_updateManager = new UpdateManager(this);
    connect(m_updateManager, &UpdateManager::errorOccurred, this, &VuraMainWindow::updaterErrorOccurred);
    connect(m_updateManager, &UpdateManager::updateAvailable, this, &VuraMainWindow::updateAvailable);
    connect(m_updateManager, &UpdateManager::downloadProgress, this, &VuraMainWindow::updateDownloadProgress);
    connect(m_updateManager, &UpdateManager::downloadFinished, this, &VuraMainWindow::updateDownloadFinished);
    m_updateManager->checkForUpdates();
}

void VuraMainWindow::actionTestFunction()
{
    //qDebug() << "Current Height: " << QString::number(height()) << ". Current Width: " << QString::number(width());
    addMedia({QUrl::fromLocalFile("C:\\Users\\halea\\Vura-Testing\\test1.mp4")});
}

void VuraMainWindow::actionOpenNetworkStream()
{
    bool ok;
    const QString networkUrl = QInputDialog::getText(this,
            tr("Open Network Stream"),
            tr("Network Stream URL:"),
            QLineEdit::Normal,
            QString(),
            &ok);

    if (ok && !networkUrl.isEmpty())
        addMedia({QUrl(networkUrl)});
}

void VuraMainWindow::actionEmergencyClose()
{
    m_controller->pause();
    setWindowState(Qt::WindowMinimized);
}

void VuraMainWindow::actionShowLogViewer()
{
    showDialog(m_logViewerDialog, this);
}

void VuraMainWindow::actionToggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
        if (m_wasMaximized)
            showMaximized();

        unsetCursor();
        ui->menubar->show();
        m_videoSliderWidget->show();
        if (m_wasPlaylistShowing)
            m_playlistDock->show();

        return;
    }

    m_wasMaximized = isMaximized();
    m_wasPlaylistShowing = m_playlistDock->isVisible();
    ui->menubar->hide();
    m_videoSliderWidget->hide();
    m_playlistDock->hide();
    showFullScreen();
}

void VuraMainWindow::actionShowSettings()
{
    auto *dialog = showDialog(m_settingsDialog, this);
    connect(dialog, &SettingsDialog::requiresRestart, this, &VuraMainWindow::restartApplication);
}

void VuraMainWindow::actionShowConvertMedia()
{
    showDialog(m_convertMediaDialog, this);
}

void VuraMainWindow::actionExit()
{
    const QMessageBox::StandardButton confirmationBox = QMessageBox::question(this,
        tr("Exit Application"),
        tr("Are you sure you want to exit?"),
        QMessageBox::Yes | QMessageBox::No);

    if (confirmationBox == QMessageBox::Yes) {
        this->close();
    }
}

void VuraMainWindow::actionViewTogglePlaylist()
{
    if (m_playlistDock->isVisible())
        m_playlistDock->hide();
    else
        m_playlistDock->show();

    ui->actionViewTogglePlaylist->setChecked(m_playlistDock->isVisible());
}

void VuraMainWindow::actionToggleVideoControls()
{
    if (m_showingVideoControls) {
        ui->verticalLayout->removeWidget(m_videoControlWidget);
        delete m_videoControlWidget;
        m_showingVideoControls = false;
    } else {
        m_videoControlWidget = new VideoControlWidget(this);
        m_videoControlWidget->setMuted(m_controller->isMuted());
        m_videoControlWidget->setVolume(m_controller->volume());

        ui->verticalLayout->addWidget(m_videoControlWidget);

        connect(m_controller, &MediaController::playbackStateChanged, m_videoControlWidget, &VideoControlWidget::setState);
        connect(m_videoControlWidget, &VideoControlWidget::play, m_controller, &MediaController::play);
        connect(m_videoControlWidget, &VideoControlWidget::pause, m_controller, &MediaController::pause);
        connect(m_videoControlWidget, &VideoControlWidget::stop, m_controller, &MediaController::stop);
        connect(m_videoControlWidget, &VideoControlWidget::changeVolume, this, [this](int value) {
            m_controller->setVolume(Helpers::sliderToLinear(value));
        });

        m_showingVideoControls = true;
    }
    ui->actionViewToggleVideoControls->setChecked(m_showingVideoControls);
}

void VuraMainWindow::actionViewToggleStatusBar() const
{
    if (ui->statusBar->isVisible()) {
        ui->statusBar->hide();
    } else {
        ui->statusBar->show();
    }
    ui->actionViewToggleStatusBar->setChecked(ui->statusBar->isVisible());
}

void VuraMainWindow::actionViewToggleVideoResolution()
{
    QSettings settings;
    settings.setValue("showVideoResolutionOnStart", ui->actionViewToggleVideoResolution->isChecked());
    setApplicationWindowTitle();
}

void VuraMainWindow::actionViewMediaInformation()
{
    QMessageBox::information(this, tr("Information"), tr("This function is not implemented yet."));
    return;

    auto *dialog = showDialog(m_mediaInformationDialog, this);
    //dialog->setMetaData(*m_playbackController->getMetadata());
}

void VuraMainWindow::actionMarkersClearIn() {}

void VuraMainWindow::actionMarkersClearInOut() {}

void VuraMainWindow::actionMarkersClearMarkers()
{
    const QMessageBox::StandardButton confirmationBox = QMessageBox::question(
        this,
        tr("Clear Markers"),
        tr("Are you sure you want to clear all markers? This cannot be undone."),
        QMessageBox::Yes | QMessageBox::No
        );

    if (confirmationBox == QMessageBox::Yes) {
        m_videoMarkerController->clearMarkers();
    }
}

void VuraMainWindow::actionMarkersClearOut() {}

void VuraMainWindow::actionMarkersEditSelectedMarker()
{
    const VideoMarkerRecord marker = m_videoMarkerController->getSelectedMarker();
    if (marker.id > 0)
        openMarkerEditor(marker);
}

void VuraMainWindow::actionMarkersGoToIn() {}

void VuraMainWindow::actionMarkersGoToOut() {}

void VuraMainWindow::actionMarkersMarkIn() {}

void VuraMainWindow::actionMarkersMarkOut() {}

void VuraMainWindow::actionSubtitlesOpenSubtitlesFile()
{
    QSettings settings;

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Subtitle File"),
        Helpers::getLastOpenedDirectory(),
        tr("Subtitles (*.srt *.ass *.ssa *.vtt *.sub);;All files (*)"));

    if (fileName.isEmpty())
        return;

    if (m_controller->loadExternalSubtitle(QUrl::fromLocalFile(fileName)))
        qDebug() << "Loaded external subtitle file: " << fileName;
    else
        qWarning() << "Failed to load external subtitle file: " << fileName;

    Helpers::setLastOpenedDirectory(QFileInfo(fileName).path());
}

void VuraMainWindow::actionSubtitlesToggleSubtitles(bool checked)
{
    m_subtitlesEnabled = checked;
}

void VuraMainWindow::restartApplication()
{
    qInfo() << "Restarting application...";
    qApp->exit(0xA1);
}

void VuraMainWindow::openPaths(const QList<QUrl> &urls)
{
    if (urls.isEmpty())
        return;
    m_controller->openUrls(urls);
}

void VuraMainWindow::initSystemTray()
{
    QSettings settings;

    m_systemTray = new SystemTrayWidget(this);

    connect(m_systemTray, &SystemTrayWidget::clicked, this, &VuraMainWindow::systemTray_Clicked);
    connect(m_systemTray, &SystemTrayWidget::hiding, this, &VuraMainWindow::systemTray_Hide);
    connect(m_systemTray, &SystemTrayWidget::togglePlayPause, m_controller, &MediaController::togglePlayPause);
    connect(m_systemTray, &SystemTrayWidget::stop, m_controller, &MediaController::stop);
    connect(m_systemTray, &SystemTrayWidget::nextVideo, m_controller, &MediaController::next);
    connect(m_systemTray, &SystemTrayWidget::previousVideo, m_controller, &MediaController::previous);
    connect(m_systemTray, &SystemTrayWidget::setMuted, m_controller, &MediaController::setMuted);
    connect(m_systemTray, &SystemTrayWidget::toggleFullscreen, this, &VuraMainWindow::actionToggleFullscreen);
    connect(m_systemTray, &SystemTrayWidget::exit, this, &VuraMainWindow::actionExit);

    bool systemTrayIcon = settings.value("systemTrayIcon", true).toBool();
    m_systemTray->setVisibility(systemTrayIcon);
}

void VuraMainWindow::buildMenus()
{
    const QSettings settings;

    auto *backendGroup = new QActionGroup(this);
    for (const media::BackendInfo &info : media::availableBackends()) {
        QAction *action = ui->menuPlaybackEngine->addAction(info.displayName);
        action->setCheckable(true);
        action->setEnabled(info.available);
        action->setChecked(info.backend == m_controller->backend());
        backendGroup->addAction(action);

        connect(action, &QAction::triggered, this, [this, info] {
            QString error;
            if (!m_controller->setBackend(info.backend, &error))
                qCritical() << "Could not switch backend: " << error;
            else if (!error.isEmpty())
                qCritical() << error;
            else
                qInfo() << "Now using the " << info.displayName << " backend.";
        });
    }

    connect(ui->actionPlaybackModeShuffle, &QAction::toggled, this, [this](bool checked) {
        m_controller->playlist()->setShuffled(checked);
    });

    // File Actions
    connect(ui->actionFileEmergencyClose, &QAction::triggered, this, &VuraMainWindow::actionEmergencyClose);
    connect(ui->actionFileExit, &QAction::triggered, this, &VuraMainWindow::actionExit);
    connect(ui->actionFileOpenFile, &QAction::triggered, this, &VuraMainWindow::actionFileOpenFile);
    connect(ui->actionFileOpenFolder, &QAction::triggered, this, &VuraMainWindow::actionFileOpenFolder);
    connect(ui->actionFileOpenNetworkStream, &QAction::triggered, this, &VuraMainWindow::actionOpenNetworkStream);
    connect(ui->actionFileOpenMultipleFiles, &QAction::triggered, this, &VuraMainWindow::actionFileOpenMultipleFiles);
    connect(ui->actionFileSavePlaylist, &QAction::triggered, this, &VuraMainWindow::actionFileSavePlaylist);
    connect(ui->actionFileOpenPlaylist, &QAction::triggered, this, &VuraMainWindow::actionFileOpenPlaylist);
    connect(ui->actionFileConvertSave, &QAction::triggered, this, &VuraMainWindow::actionShowConvertMedia);

    m_recentFiles = new RecentFilesMenu(ui->menuFileOpenRecent, ui->actionFileOpenRecentClear, this);
    connect(m_recentFiles, &RecentFilesMenu::fileSelected, this, [this](const QUrl &url) {
        addMedia({url});
    });


    // View Actions
    connect(ui->actionViewToggleStatusBar, &QAction::triggered, this, &VuraMainWindow::actionViewToggleStatusBar);
    if (settings.value("showStatusBarOnStart", false).toBool()) {
        ui->actionViewToggleStatusBar->setChecked(true);
        ui->statusBar->show();
    } else {
        ui->actionViewToggleStatusBar->setChecked(false);
        ui->statusBar->hide();
    }

    connect(ui->actionViewToggleVideoResolution, &QAction::triggered, this, &VuraMainWindow::actionViewToggleVideoResolution);
    if (settings.value("showVideoResolutionOnStart", false).toBool()) {
        ui->actionViewToggleVideoResolution->setChecked(true);
    } else {
        ui->actionViewToggleVideoResolution->setChecked(false);
    }

    connect(ui->actionViewToggleVideoControls, &QAction::triggered, this, &VuraMainWindow::actionToggleVideoControls);
    connect(ui->actionViewMediaInformation, &QAction::triggered, this, &VuraMainWindow::actionViewMediaInformation);
    connect(ui->actionViewPreferences, &QAction::triggered, this, &VuraMainWindow::actionShowSettings);


    // Playback Actions
    connect(ui->actionPlaybackNext, &QAction::triggered, m_controller, &MediaController::next);
    connect(ui->actionPlaybackPrevious, &QAction::triggered, m_controller, &MediaController::previous);
    connect(ui->actionPlaybackTogglePlay, &QAction::triggered, m_controller, &MediaController::togglePlayPause);
    connect(ui->actionPlaybackRestartVideo, &QAction::triggered, m_controller, &MediaController::restart);
    connect(ui->actionViewTogglePlaylist, &QAction::triggered, this, &VuraMainWindow::actionViewTogglePlaylist);

    auto *repeatGroup = new QActionGroup(this);   // exclusive by default
    for (auto [action, mode] : {
        std::pair{ui->actionPlaybackModeDoNotLoopPlaylist, Playlist::RepeatMode::RepeatNone},
        std::pair{ui->actionPlaybackModeLoopCurrentTrack, Playlist::RepeatMode::RepeatOne},
        std::pair{ui->actionPlaybackModeLoopPlaylist, Playlist::RepeatMode::RepeatAll}
    }) {
        repeatGroup->addAction(action);
        connect(action, &QAction::triggered, this, [this, mode] {
            m_controller->playlist()->setRepeatMode(mode);
        });
    }

    struct Jump { QAction *fwd, *back; const char *key; int def; };
    const Jump jumps[] = {
        {ui->actionPlaybackJumpForwardExtraLarge, ui->actionPlaybackJumpBackwardExtraLarge, "extraLargeJump", 90},
        {ui->actionPlaybackJumpForwardLarge, ui->actionPlaybackJumpBackwardLarge, "largeJump", 30},
        {ui->actionPlaybackJumpForwardMedium, ui->actionPlaybackJumpBackwardMedium, "mediumJump", 15},
        {ui->actionPlaybackJumpForwardSmall, ui->actionPlaybackJumpBackwardSmall, "smallJump", 5},
        {ui->actionPlaybackJumpForwardExtraSmall, ui->actionPlaybackJumpBackwardExtraSmall, "extraSmallJump", 1},
    };
    for (const Jump &j : jumps) {
        auto seek = [this, j](int sign) {
            m_controller->seekRelative(sign * QSettings().value(j.key, j.def).toInt() * 1000);
        };
        connect(j.fwd,  &QAction::triggered, this, [seek] { seek(+1); });
        connect(j.back, &QAction::triggered, this, [seek] { seek(-1); });
    }

    struct Rate { QAction *faster, *slower; const char *key; double def; };
    const Rate rates[] = {
        {ui->actionPlaybackSpeedFaster, ui->actionPlaybackSpeedSlower, "playbackSpeedAdjustment", 0.5},
        {ui->actionPlaybackSpeedFasterFine, ui->actionPlaybackSpeedSlowerFine, "playbackSpeedAdjustmentFine", 0.25}
    };
    for (const Rate &r : rates) {
        auto adjust = [this, r](int sign) {
            double maxPlaybackRate = QSettings().value("playbackSpeedMax", 10.0).toDouble();
            qreal newRate = m_controller->rate() + (sign * QSettings().value(r.key, r.def).toDouble());
            if (newRate < 0.1)
                newRate = 0.1;
            if (newRate > QSettings().value("playbackSpeedMax", 10.0).toDouble())
                newRate = QSettings().value("playbackSpeedMax", 10.0).toDouble();
            m_controller->setRate(newRate);
        };
        connect(r.faster, &QAction::triggered, this, [adjust] { adjust(+1); });
        connect(r.slower, &QAction::triggered, this, [adjust] { adjust(-1); });
    }

    connect(ui->actionPlaybackSpeedNormal, &QAction::triggered, this, [this]() {
        m_controller->setRate(1.0);
    });


    // Markers
    connect(ui->actionMarkersClearIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearIn);
    connect(ui->actionMarkersClearInOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearInOut);
    connect(ui->actionMarkersClearMarkers, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearMarkers);
    connect(ui->actionMarkersClearOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearOut);
    connect(ui->actionMarkersEditSelectedMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersEditSelectedMarker);
    connect(ui->actionMarkersGoToIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToIn);
    connect(ui->actionMarkersGoToOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToOut);
    connect(ui->actionMarkersMarkIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersMarkIn);
    connect(ui->actionMarkersMarkOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersMarkOut);


    // Audio Actions
    connect(ui->actionAudioToggleMute, &QAction::toggled, m_controller, &MediaController::setMuted);
    //connect(ui->actionAudioVolumeDown, &QAction::triggered, m_playbackController, &PlaybackController::volumeDown);
    //connect(ui->actionAudioVolumeUp, &QAction::triggered, m_playbackController, &PlaybackController::volumeUp);
    //m_videoMarkerController = new VideoMarkerController(this);


    // Video
    connect(ui->actionVideoFullscreen, &QAction::triggered, this, &VuraMainWindow::actionToggleFullscreen);


    // Subtitles
    connect(ui->actionSubtitlesOpenSubtitlesFile, &QAction::triggered, this, &VuraMainWindow::actionSubtitlesOpenSubtitlesFile);
    connect(ui->actionSubtitlesToggleSubtitles, &QAction::toggled, this, &VuraMainWindow::actionSubtitlesToggleSubtitles);


    // Tools
    connect(ui->actionToolsTestFunction, &QAction::triggered, this, &VuraMainWindow::actionTestFunction);


    // Help
    connect(ui->actionHelpViewCurrentLog, &QAction::triggered, this, &VuraMainWindow::actionShowLogViewer);
    connect(ui->actionHelpCheckForUpdates, &QAction::triggered, this, &VuraMainWindow::actionHelpCheckForUpdates);
}

void VuraMainWindow::buildPlaylistDock()
{
    QSettings settings;

    m_playlistDock = new QDockWidget(tr("Playlist"), this);
    m_playlistDock->setObjectName(QStringLiteral("playlistDock"));

    m_playlistWidget = new PlaylistWidget(this);
    m_playlistWidget->setPlaylistModel(m_controller->playlist());

    connect(m_playlistWidget, &PlaylistWidget::doubleClicked, this, [this](const QModelIndex &index) {
        m_controller->playIndex(index.row());
    });

    m_playlistDock->setWidget(m_playlistWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_playlistDock);

    if (settings.value("showPlaylistOnStart", true).toBool()) {
        m_playlistDock->show();
        ui->actionViewTogglePlaylist->setChecked(true);
    } else {
        m_playlistDock->hide();
        ui->actionViewTogglePlaylist->setChecked(false);
    }
}

void VuraMainWindow::initUI()
{
    QSettings settings;

    const int defaultWindowHeight = settings.value("defaultWindowHeight", 530).toInt();
    const int defaultWindowWidth = settings.value("defaultWindowWidth", 887).toInt();
    resize(defaultWindowWidth, defaultWindowHeight);

    if (settings.value("rememberWindowSize", false).toBool()) {
        qDebug() << "Rememmber window size setting set to true. Restoring previous window size.";
        restoreGeometry(settings.value("geometry").toByteArray());
    }

    m_videoSlider = new VideoSlider(this);
    connect(m_videoSlider, &VideoSlider::scrubbed, m_controller, [this](qint64 ms) {
        m_controller->seek(ms);
    });
    connect(m_videoSlider, &VideoSlider::scrubFinished, m_controller, [this](qint64 ms) {
        m_controller->seek(ms);
    });
    connect(m_videoSlider, &VideoSlider::markerEditRequested, this, &VuraMainWindow::openMarkerEditor);

    m_videoMarkerController = new VideoMarkerController(*m_videoSlider, this);
    connect(ui->actionMarkersClearSelectedMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::clearSelectedMarker);
    connect(ui->actionMarkersGoToNextMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::goToNextMarker);
    connect(ui->actionMarkersGoToPreviousMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::goToPreviousMarker);

    connect(m_videoSlider, &VideoSlider::markerDeleteRequested, m_videoMarkerController, &VideoMarkerController::deleteVideoMarker);

    struct MarkerTypeActions { const char *type; QAction *toggle; QAction *add; };
    const MarkerTypeActions markerTypes[] = {
        {"marker", ui->actionViewToggleMarkersMarkers, ui->actionMarkersAddMarker},
        {"cumshot", ui->actionViewToggleMarkersCumshotMarkers, ui->actionMarkersAddCumshotMarker},
        {"cyan", ui->actionViewToggleMarkersCyanMarkers, ui->actionMarkersAddCyanMarker},
        {"dialog", ui->actionViewToggleMarkersDialogMarkers, ui->actionMarkersAddDialogMarker},
        {"magenta", ui->actionViewToggleMarkersMagentaMarkers, ui->actionMarkersAddMagentaMarker},
        {"orange", ui->actionViewToggleMarkersOrangeMarkers, ui->actionMarkersAddOrangeMarker},
        {"scene", ui->actionViewToggleMarkersSceneMarkers, ui->actionMarkersAddSceneMarker},
        {"strip", ui->actionViewToggleMarkersStripMarkers, ui->actionMarkersAddStripMarker}
    };

    for (const auto &m : markerTypes) {
        const QString type = QString::fromLatin1(m.type);
        m_markerToggleActions.insert(type, m.toggle);

        connect(m.toggle, &QAction::toggled, m_videoMarkerController, [this, type](bool visible) {
            m_videoMarkerController->setTypeVisible(type, visible);
        });
        connect(m.add, &QAction::triggered, m_videoMarkerController, [this, type] {
            m_videoMarkerController->addMarkerOfType(type);
        });
    }

    connect(m_videoSlider, &VideoSlider::markerTypeHidden, this, [this](const QString &type) {
        if (QAction *action = m_markerToggleActions.value(type))
            action->setChecked(false);
    });

    m_videoSliderWidget = new VideoSliderWidget(*m_videoSlider, this);
    connect(m_controller, &MediaController::rateChanged, m_videoSliderWidget, &VideoSliderWidget::playbackRateChanged);

    ui->verticalLayout->addWidget(m_videoSliderWidget);
    ui->verticalLayout->setStretch(0, 1);

    const int autoHideTimer = settings.value("sliderAutohideTime", 5).toInt() * 1000;
    m_videoSliderHideTimer = new QTimer(this);
    m_videoSliderHideTimer->setInterval(autoHideTimer);
    m_videoSliderHideTimer->setSingleShot(true);
    connect(m_videoSliderHideTimer, &QTimer::timeout, this, &VuraMainWindow::hideVideoSlider);

    if (settings.value("showVideoControlsOnStart", false).toBool())
        actionToggleVideoControls();
}

void VuraMainWindow::connectController()
{
    connect(m_controller, &MediaController::positionChanged, this, [this](media::Msec ms) {
        m_lastPosition = ms;
        m_videoSlider->setPositionFromEngine(ms);
        m_videoSliderWidget->positionChanged(ms);
    });

    connect(m_controller, &MediaController::durationChanged, this, [this](media::Msec ms) {
        m_videoSlider->setRange(0, int(ms));
        m_videoSliderWidget->durationChanged(ms);
    });

    connect(m_controller, &MediaController::playbackStateChanged, this, [this](media::PlaybackState state) {
        stateChanged(state);
        const bool playing = state == media::PlaybackState::Playing;
        const bool hasVideo = !m_controller->engine() || !m_controller->engine()->tracks(media::TrackType::Video).isEmpty();
        m_sleepInhibitor->setInhibited(playing && hasVideo, tr("Playing video"));
    });

    connect(m_controller, &MediaController::mediaStatusChanged, this, [this](media::MediaStatus status) {
        switch (status) {
            case media::MediaStatus::Loading:
                qCDebug(Playback) << "Media Status Changed: Loading...";
                break;

            case media::MediaStatus::Buffering:
                qCDebug(Playback) << "Media Status Changed: Buffering...";
                break;

            case media::MediaStatus::Buffered:
                qCDebug(Playback) << "Media Status Changed: Buffered";
                break;

            case media::MediaStatus::Loaded:
                qCDebug(Playback) << "Media Status Changed: Loaded";
                break;

            case media::MediaStatus::Invalid:
                qCWarning(Playback) << "This file could not be played.";
                QMessageBox::critical(this, tr("Error"), tr("This file could not be played."));
                break;

            default:
                break;
        }
    });

    connect(m_controller, &MediaController::tracksChanged, this, &VuraMainWindow::rebuildTrackMenus);
    connect(m_controller, &MediaController::audioDevicesChanged, this, &VuraMainWindow::rebuildAudioDeviceMenu);
    connect(m_controller, &MediaController::backendChanged, this, [this] { rebuildAudioDeviceMenu(); });
    rebuildAudioDeviceMenu();
    connect(m_controller, &MediaController::capabilitiesChanged, this, &VuraMainWindow::applyCapabilities);

    connect(m_controller, &MediaController::currentItemChanged, this, [this](const PlaylistItem &item) {
        m_recentFiles->add(item.url);
        sourceChanged(item.url);
        m_stage->setCaption(item.displayTitle());
        setApplicationWindowTitle();
    });

    connect(m_controller, &MediaController::metaDataChanged, this, [this](const QVariantMap &) {
        setApplicationWindowTitle();
    });

    connect(m_controller, &MediaController::errorOccurred, this, [this](media::ErrorKind kind, const QString &detail) {
        qCWarning(Playback) << "Error occurred: " << QString(detail.isEmpty() ? media::describe(kind) : detail);
    });
}

void VuraMainWindow::initMisc()
{
    m_crashReporter = new CrashReporter(this);
    connect(m_crashReporter, &CrashReporter::scanFinished, this, &VuraMainWindow::crashReportScanFinished);
    connect(m_crashReporter, &CrashReporter::uploadStarted, this, &VuraMainWindow::crashReportUploadStarted);
    connect(m_crashReporter, &CrashReporter::finished, this, &VuraMainWindow::crashReportUploadFinished);
    m_crashReporter->checkForPreviousCrashes();
}

void VuraMainWindow::applyCapabilities(const media::Capabilities &capabilities)
{
    ui->menuVideoTrack->setEnabled(capabilities.videoTrackSelection);
    ui->menuAudioTrack->setEnabled(capabilities.audioTrackSelection);
    //ui->menuSubtitleTrack->setEnabled(capabilities.subtitleTrackSelection);
    ui->menuAudioDevice->setEnabled(capabilities.audioDeviceSelection);
}

void VuraMainWindow::rebuildTrackMenus()
{
    media::Engine *engine = m_controller->engine();
    if (!engine)
        return;

    const auto populate = [this, engine](QMenu *menu, media::TrackType type, bool allowNone) {
        menu->clear();
        auto *group = new QActionGroup(menu);
        const QString active = engine->activeTrack(type);

        if (allowNone) {
            QAction *off = menu->addAction(tr("Off"));
            off->setCheckable(true);
            off->setChecked(active.isEmpty());
            group->addAction(off);
            connect(off, &QAction::triggered, this,
                    [this, type] { m_controller->selectTrack(type, {}); });
        }

        const auto tracks = engine->tracks(type);
        for (int i = 0; i < tracks.size(); ++i) {
            const media::TrackInfo &track = tracks.at(i);
            QAction *action = menu->addAction(track.displayName(i));
            action->setCheckable(true);
            action->setChecked(track.id == active);
            group->addAction(action);
            connect(action, &QAction::triggered, this,
                    [this, type, id = track.id] { m_controller->selectTrack(type, id); });
        }

        if (tracks.isEmpty() && !allowNone)
            menu->addAction(tr("No tracks"))->setEnabled(false);
    };

    populate(ui->menuVideoTrack, media::TrackType::Video, true);
    populate(ui->menuAudioTrack, media::TrackType::Audio, true);
    populate(ui->menuSubtitleTrack, media::TrackType::Subtitle, true);
}

void VuraMainWindow::rebuildAudioDeviceMenu()
{
    QMenu *menu = ui->menuAudioDevice;
    menu->clear();

    auto *group = new QActionGroup(menu);
    const QString active = m_controller->activeAudioDevice();

    QAction *systemDefault = menu->addAction(tr("System Default"));
    systemDefault->setCheckable(true);
    systemDefault->setChecked(active.isEmpty());
    group->addAction(systemDefault);
    connect(systemDefault, &QAction::triggered, this, [this] {
        m_controller->setAudioDevice({});
    });

    const auto devices = m_controller->audioDevices();
    if (!devices.isEmpty())
        menu->addSeparator();

    for (const media::AudioDeviceInfo &device : devices) {
        QAction *action = menu->addAction(device.displayName());
        action->setCheckable(true);
        action->setChecked(device.id == active);
        group->addAction(action);
        connect(action, &QAction::triggered, this, [this, id = device.id] {
            m_controller->setAudioDevice(id);
        });
    }

    if (devices.isEmpty())
        menu->addAction(tr("No audio devices"))->setEnabled(false);

    menu->setEnabled(m_controller->capabilities().audioDeviceSelection);
}

void VuraMainWindow::setTrackInfo(const QString &trackInfo)
{
    m_trackInfo = trackInfo;
}

void VuraMainWindow::setApplicationWindowTitle()
{
    const QString title = m_controller->currentTitle();
    setWindowTitle(title.isEmpty() ? QString("Vura %1").arg(VURA_VERSION_STRING) : QString("Vura %1 - %2").arg(VURA_VERSION_STRING).arg(title));
}

QString VuraMainWindow::trackName(const QMediaMetaData &metaData, const int index)
{
    QString name;
    const QString title = metaData.stringValue(QMediaMetaData::Title);
    const auto lang = metaData.value(QMediaMetaData::Language).value<QLocale::Language>();

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

void VuraMainWindow::updateMarkerMenuItems()
{
    ui->actionMarkersEditSelectedMarker->setEnabled(m_videoMarkerController->checkMarkerProximity());
}

void VuraMainWindow::crashReportScanFinished(const bool crashFileExists)
{
    if (crashFileExists) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Vura Crash Recovery"),
            tr("Vura Video Player closed unexpectedly during your last session.\n\n"
            "Would you like to send the crash dump to the developers to help fix the issue?"),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            qDebug() << "User selected to send crash files.";
            m_crashReporter->uploadCrashFile(true);
        } else {
            qDebug() << "User selected not to send crash files.";
            m_crashReporter->uploadCrashFile(false);
        }
    }
}

void VuraMainWindow::crashReportUploadStarted() {}

void VuraMainWindow::crashReportUploadFinished(const bool success, const QString& message)
{
    if (success) {
        QMessageBox::information(this, tr("Vura Crash Recovery"), tr("Crash report uploaded successfully!"));
    } else {
        QMessageBox::warning(this, tr("Vura Crash Recovery"),
            tr("Failed to upload crash report.\n\n"
            "Error message: ") + message);
    }
}

void VuraMainWindow::showResumeOverlay(const qint64 savedPosition)
{
    QSettings settings;

    if (m_continuePlaybackWidget)
        m_continuePlaybackWidget->deleteLater();

    m_continuePlaybackWidget = new ContinuePlaybackWidget(savedPosition, this);
    connect(m_continuePlaybackWidget, &ContinuePlaybackWidget::continuePlayback, this, &VuraMainWindow::continuePlaybackAccepted);
    connect(m_continuePlaybackWidget, &ContinuePlaybackWidget::closeWidget, this, &VuraMainWindow::continuePlaybackDeclined);

    // Stop timer when mouse enters the widget
    connect(m_continuePlaybackWidget, &ContinuePlaybackWidget::mouseEntered, this, [this]() {
        if (m_continuePlaybackBannerTimer && m_continuePlaybackBannerTimer->isActive()) {
            m_continuePlaybackBannerTimer->stop();
        }
    });

    // Restart timer when mouse leaves the widget
    connect(m_continuePlaybackWidget, &ContinuePlaybackWidget::mouseLeft, this, [this]() {
        if (m_continuePlaybackBannerTimer) {
            m_continuePlaybackBannerTimer->start();
        }
    });

    ui->verticalLayout->insertWidget(0, m_continuePlaybackWidget);
    ui->verticalLayout->setStretch(1, 1);

    if (m_continuePlaybackBannerTimer) {
        m_continuePlaybackBannerTimer->stop();
        delete m_continuePlaybackBannerTimer;
        m_continuePlaybackBannerTimer = nullptr;
    }

    int continuePlaybackBannerTime = settings.value("continuePlaybackBannerTime", 5).toInt();
    m_continuePlaybackBannerTimer = new QTimer(this);
    m_continuePlaybackBannerTimer->setSingleShot(true);
    m_continuePlaybackBannerTimer->setInterval(continuePlaybackBannerTime * 1000);
    connect(m_continuePlaybackBannerTimer, &QTimer::timeout, this, &VuraMainWindow::continuePlaybackDelete);
    m_continuePlaybackBannerTimer->start();
}

void VuraMainWindow::continuePlaybackDeclined()
{
    continuePlaybackDelete();
}

void VuraMainWindow::continuePlaybackAccepted(const qint64 savedPosition)
{
    m_controller->seek(savedPosition);
    continuePlaybackDelete();
}

void VuraMainWindow::continuePlaybackDelete()
{
    if (ui->verticalLayout->indexOf(m_continuePlaybackWidget) != -1)
        ui->verticalLayout->removeWidget(m_continuePlaybackWidget);

    if (m_continuePlaybackWidget) {
        m_continuePlaybackWidget->deleteLater();
        m_continuePlaybackWidget = nullptr;
    }
}

void VuraMainWindow::systemTray_Clicked()
{
    if (isHidden() || isMinimized()) {
        showNormal();
        activateWindow();
    }
}

void VuraMainWindow::systemTray_Hide(const bool hiding)
{
    if (hiding) {
        hide();
        m_controller->pause();

    } else {
        show();
        showNormal();
        raise();
        activateWindow();
    }
}

void VuraMainWindow::updaterErrorOccurred(QString errorMessage)
{
    QMessageBox::critical(this, "Update Error", errorMessage);
}

void VuraMainWindow::updateAvailable(bool available)
{
    if (available) {
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, "Update Available", "Download newest update?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Ignore);

        if (reply == QMessageBox::Yes) {
            m_updateProgressDialog = new QProgressDialog("Downloading update...", "Cancel", 0, 100, this);
            m_updateProgressDialog->setWindowModality(Qt::WindowModal);
            m_updateManager->downloadUpdate();
        } else if (reply == QMessageBox::No) {

        } else if (reply == QMessageBox::Ignore) {
            //QSettings settings;
            //settings.setValue("lastCheckedVersion", "")
        } else {

        }
    } else {
        QMessageBox::information(this, "Update", "No Update Available");
    }
}

void VuraMainWindow::updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (!m_updateProgressDialog)
        return;

    if (bytesTotal > 0) {
        const int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
        m_updateProgressDialog->setValue(percentage);
    }
}

void VuraMainWindow::updateDownloadFinished(bool success, const QString &message)
{
    if (m_updateProgressDialog)
        m_updateProgressDialog->close();

    if (success) {
        QMessageBox::information(this, "Update Finished", "Finished downloading update");
    }
}

void VuraMainWindow::addMedia(const QList<QUrl> &mediaList)
{
    if (mediaList.isEmpty())
        return;

    if (m_replacePlaylist)
        openPaths(mediaList);
    else
        m_controller->enqueue(mediaList);
}

void VuraMainWindow::openMarkerEditor(const VideoMarkerRecord &marker)
{
    auto *dialog = showDialog(m_markerEditDialog, marker, m_controller->duration(), this);
    connect(dialog, &MarkerEditDialog::markerEdited, m_videoMarkerController, &VideoMarkerController::addVideoMarker);
    connect(dialog, &MarkerEditDialog::markerDeleted, m_videoMarkerController, &VideoMarkerController::deleteVideoMarker);
}
