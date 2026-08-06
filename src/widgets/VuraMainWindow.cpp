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
#include "PlaylistEmptyStateWidget.h"
#include "ui_VuraMainWindow.h"

#include <ui-config.h>
#include <qglobal.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif


static Logger* globalRedirector = nullptr;

VuraMainWindow::VuraMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::VuraMainWindow)
{
    ui->setupUi(this);

    ui->mediaAreaWidget->setCurrentIndex(0);
    ui->playlistWidget->setCurrentIndex(1);

    const QSettings settings;

    ui->actionRendererVideoWidget->setChecked(!settings.value("useHardwareAcceleration", false).toBool());
    ui->actionRendererOpenGL->setChecked(settings.value("useHardwareAcceleration", false).toBool());

    setAcceptDrops(true);

    ui->playlistWidget->setStyleSheet("QStackedWidget { border: 1px solid #878787; border-right: none; border-bottom: none; }");

    qInstallMessageHandler(Logger::messageHandler);
    globalRedirector = Logger::instance();

    m_playbackController = new PlaybackController(ui->mediaAreaWidget, this);
    m_playlistController = new PlaylistController(
            ui->playlistView,
            ui->emptyPlaylistView,
            ui->playlistWidget,
            ui->actionViewTogglePlaylist,
            this
        );

    if (settings.value("useHardwareAcceleration", false).toBool()) {
        m_openGLWidget = new VuraMediaEngine(this);
        ui->verticalLayout_8->addWidget(m_openGLWidget);
        m_playbackController->setOpenGLWidget(m_openGLWidget);
    } else {
        initializeVideoWidget();
        m_playbackController->setVideoWidget(ui->videoWidget);
    }

    connect(m_playlistController, &PlaylistController::playTrackRequested, m_playbackController, &PlaybackController::playTrack);

    if (settings.value("showPlaylistOnStart", true).toBool()) {
        m_playlistController->showPlaylist();
        ui->actionViewTogglePlaylist->setChecked(true);
    } else {
        m_playlistController->hidePlaylist();
        ui->actionViewTogglePlaylist->setChecked(false);
    }

    setConnections();
    //configureUpdater();

    qCDebug(Core) << "Application Initialized!";
    qCInfo(Core) << "Vura Version: " << VURA_VERSION_STRING;
}

void VuraMainWindow::setConnections()
{
    const QSettings settings;

    // Playback Actions
    connect(ui->actionPlaybackNext, &QAction::triggered, m_playlistController, &PlaylistController::nextTrack);
    this->addAction(ui->actionPlaybackNext);
    ui->actionPlaybackNext->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackPrevious, &QAction::triggered, m_playlistController, &PlaylistController::previousTrack);
    this->addAction(ui->actionPlaybackPrevious);
    ui->actionPlaybackPrevious->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackTogglePlay, &QAction::triggered, m_playbackController, &PlaybackController::togglePlayPause);
    this->addAction(ui->actionPlaybackTogglePlay);
    ui->actionPlaybackTogglePlay->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackRestartVideo, &QAction::triggered, m_playbackController, &PlaybackController::restart);
    this->addAction(ui->actionPlaybackRestartVideo);
    ui->actionPlaybackRestartVideo->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewTogglePlaylist, &QAction::triggered, m_playlistController, &PlaylistController::togglePlaylist);

    // Audio Actions
    connect(ui->actionAudioToggleMute, &QAction::triggered, m_playbackController, &PlaybackController::toggleMute);
    this->addAction(ui->actionAudioToggleMute);
    ui->actionAudioToggleMute->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionAudioVolumeDown, &QAction::triggered, m_playbackController, &PlaybackController::volumeDown);
    this->addAction(ui->actionAudioVolumeDown);
    ui->actionAudioVolumeDown->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionAudioVolumeUp, &QAction::triggered, m_playbackController, &PlaybackController::volumeUp);
    this->addAction(ui->actionAudioVolumeUp);
    ui->actionAudioVolumeUp->setShortcutContext(Qt::WindowShortcut);

    m_videoMarkerController = new VideoMarkerController(this);

    // Video Slider
    m_videoSlider = new VideoSlider(m_videoMarkerController, this);
    m_videoSliderWidget = new VideoSliderWidget(*m_videoSlider, *m_playbackController, this);
    connect(m_videoMarkerController, &VideoMarkerController::markerAdded, m_videoSlider, &VideoSlider::updateVideoSlider);
    connect(m_videoMarkerController, &VideoMarkerController::markersLoaded, m_videoSlider, &VideoSlider::loadVideoMarkers);

    ui->verticalLayout->addWidget(m_videoSliderWidget);
    ui->verticalLayout->setStretch(0, 1);

    connect(m_playbackController, &PlaybackController::positionChanged, m_videoSlider, &VideoSlider::setValue);
    connect(m_playbackController, &PlaybackController::durationChanged, m_videoSlider, &VideoSlider::setMaximum);
    connect(m_playbackController, &PlaybackController::sourceChanged, this, &VuraMainWindow::sourceChanged);
    connect(m_playbackController, &PlaybackController::stateChanged, this, &VuraMainWindow::stateChanged);
    connect(m_playbackController, &PlaybackController::jumpCompleted, this, &VuraMainWindow::resetVideoSliderVisibility);
    connect(m_videoSlider, &VideoSlider::valueChanged, m_playbackController, &PlaybackController::setPosition);
    connect(m_videoSlider, &VideoSlider::sliderPressed, m_playbackController, &PlaybackController::setPaused);

    const int autoHideTimer = settings.value("sliderAutohideTime", 5).toInt() * 1000;
    m_videoSliderHideTimer = new QTimer(this);
    m_videoSliderHideTimer->setInterval(autoHideTimer);
    m_videoSliderHideTimer->setSingleShot(true);
    connect(m_videoSliderHideTimer, &QTimer::timeout, this, &VuraMainWindow::hideVideoSlider);

    connect(ui->actionFileEmergencyClose, &QAction::triggered, this, &VuraMainWindow::actionEmergencyClose);
    this->addAction(ui->actionFileEmergencyClose);
    ui->actionFileEmergencyClose->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileExit, &QAction::triggered, this, &VuraMainWindow::actionExit);
    this->addAction(ui->actionFileExit);
    ui->actionFileExit->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenFile, &QAction::triggered, m_playlistController, &PlaylistController::requestFileImport);
    this->addAction(ui->actionFileOpenFile);
    ui->actionFileOpenFile->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenFolder, &QAction::triggered, m_playlistController, &PlaylistController::requestFolderImport);
    this->addAction(ui->actionFileOpenFolder);
    ui->actionFileOpenFolder->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenNetworkStream, &QAction::triggered, this, &VuraMainWindow::actionOpenNetworkStream);
    this->addAction(ui->actionFileOpenNetworkStream);
    ui->actionFileOpenNetworkStream->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenMultipleFiles, &QAction::triggered, m_playlistController, &PlaylistController::requestMultipleFileImport);
    this->addAction(ui->actionFileOpenMultipleFiles);
    ui->actionFileOpenMultipleFiles->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileSavePlaylist, &QAction::triggered, m_playlistController, &PlaylistController::savePlaylistAs);
    this->addAction(ui->actionFileSavePlaylist);
    ui->actionFileSavePlaylist->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenPlaylist, &QAction::triggered, m_playlistController, &PlaylistController::loadPlaylistFile);
    this->addAction(ui->actionFileOpenPlaylist);
    ui->actionFileOpenPlaylist->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleStatusBar, &QAction::triggered, this, &VuraMainWindow::actionViewToggleStatusBar);
    this->addAction(ui->actionViewToggleStatusBar);
    ui->actionViewToggleStatusBar->setShortcutContext(Qt::WindowShortcut);
    if (settings.value("showStatusBarOnStart", false).toBool()) {
        ui->actionViewToggleStatusBar->setChecked(true);
        ui->statusBar->show();
    } else {
        ui->actionViewToggleStatusBar->setChecked(false);
        ui->statusBar->hide();
    }

    connect(ui->actionViewPreferences, &QAction::triggered, this, &VuraMainWindow::actionShowSettings);
    connect(ui->actionHelpViewCurrentLog, &QAction::triggered, this, &VuraMainWindow::actionShowLogViewer);

    connect(ui->actionToolsTestFunction, &QAction::triggered, this, &VuraMainWindow::actionTestFunction);
    this->addAction(ui->actionToolsTestFunction);
    ui->actionToolsTestFunction->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionVideoFullscreen, &QAction::triggered, this, &VuraMainWindow::actionToggleFullscreen);
    this->addAction(ui->actionVideoFullscreen);
    ui->actionVideoFullscreen->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleVideoControls, &QAction::triggered, this, &VuraMainWindow::actionToggleVideoControls);

    connect(ui->actionPlaybackJumpForwardExtraLarge, &QAction::triggered, m_playbackController, &PlaybackController::jumpForwardExtraLarge);
    this->addAction(ui->actionPlaybackJumpForwardExtraLarge);
    ui->actionPlaybackJumpForwardExtraLarge->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardExtraLarge, &QAction::triggered, m_playbackController, &PlaybackController::jumpBackwardExtraLarge);
    this->addAction(ui->actionPlaybackJumpBackwardExtraLarge);
    ui->actionPlaybackJumpBackwardExtraLarge->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpForwardLarge, &QAction::triggered, m_playbackController, &PlaybackController::jumpForwardLarge);
    this->addAction(ui->actionPlaybackJumpForwardLarge);
    ui->actionPlaybackJumpForwardLarge->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardLarge, &QAction::triggered, m_playbackController, &PlaybackController::jumpBackwardLarge);
    this->addAction(ui->actionPlaybackJumpBackwardLarge);
    ui->actionPlaybackJumpBackwardLarge->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpForwardMedium, &QAction::triggered, m_playbackController, &PlaybackController::jumpForwardMedium);
    this->addAction(ui->actionPlaybackJumpForwardMedium);
    ui->actionPlaybackJumpForwardMedium->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardMedium, &QAction::triggered, m_playbackController, &PlaybackController::jumpBackwardMedium);
    this->addAction(ui->actionPlaybackJumpBackwardMedium);
    ui->actionPlaybackJumpBackwardMedium->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpForwardSmall, &QAction::triggered, m_playbackController, &PlaybackController::jumpForwardSmall);
    this->addAction(ui->actionPlaybackJumpForwardSmall);
    ui->actionPlaybackJumpForwardSmall->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardSmall, &QAction::triggered, m_playbackController, &PlaybackController::jumpBackwardSmall);
    this->addAction(ui->actionPlaybackJumpBackwardSmall);
    ui->actionPlaybackJumpBackwardSmall->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpForwardExtraSmall, &QAction::triggered, m_playbackController, &PlaybackController::jumpForwardExtraSmall);
    this->addAction(ui->actionPlaybackJumpForwardExtraSmall);
    ui->actionPlaybackJumpForwardExtraSmall->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardExtraSmall, &QAction::triggered, m_playbackController, &PlaybackController::jumpBackwardExtraSmall);
    this->addAction(ui->actionPlaybackJumpBackwardExtraSmall);
    ui->actionPlaybackJumpBackwardExtraSmall->setShortcutContext(Qt::WindowShortcut);

    // Marker Actions
    connect(ui->actionMarkersAddCumshotMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddCumshotMarker);
    this->addAction(ui->actionMarkersAddCumshotMarker);
    ui->actionMarkersAddCumshotMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddCyanMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddCyanMarker);
    this->addAction(ui->actionMarkersAddCyanMarker);
    ui->actionMarkersAddCyanMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddDialogMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddDialogMarker);
    this->addAction(ui->actionMarkersAddDialogMarker);
    ui->actionMarkersAddDialogMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddMagentaMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddMagentaMarker);
    this->addAction(ui->actionMarkersAddMagentaMarker);
    ui->actionMarkersAddMagentaMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddMarker);
    this->addAction(ui->actionMarkersAddMarker);
    ui->actionMarkersAddMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddOrangeMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddOrangeMarker);
    this->addAction(ui->actionMarkersAddOrangeMarker);
    ui->actionMarkersAddOrangeMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddSceneMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddSceneMarker);
    this->addAction(ui->actionMarkersAddSceneMarker);
    ui->actionMarkersAddSceneMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddStripMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddStripMarker);
    this->addAction(ui->actionMarkersAddStripMarker);
    ui->actionMarkersAddStripMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersClearIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearIn);
    this->addAction(ui->actionMarkersClearIn);
    ui->actionMarkersClearIn->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersClearInOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearInOut);
    this->addAction(ui->actionMarkersClearInOut);
    ui->actionMarkersClearInOut->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersClearMarkers, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearMarkers);
    this->addAction(ui->actionMarkersClearMarkers);
    ui->actionMarkersClearMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersClearOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearOut);
    this->addAction(ui->actionMarkersClearOut);
    ui->actionMarkersClearOut->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersClearSelectedMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearSelectedMarker);
    this->addAction(ui->actionMarkersClearSelectedMarker);
    ui->actionMarkersClearSelectedMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersEditSelectedMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersEditSelectedMarker);
    this->addAction(ui->actionMarkersEditSelectedMarker);
    ui->actionMarkersEditSelectedMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToIn);
    this->addAction(ui->actionMarkersGoToIn);
    ui->actionMarkersGoToIn->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToNextMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToNextMarker);
    this->addAction(ui->actionMarkersGoToNextMarker);
    ui->actionMarkersGoToNextMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToOut);
    this->addAction(ui->actionMarkersGoToOut);
    ui->actionMarkersGoToOut->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToPreviousMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToPreviousMarker);
    this->addAction(ui->actionMarkersGoToPreviousMarker);
    ui->actionMarkersGoToPreviousMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersMarkIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersMarkIn);
    this->addAction(ui->actionMarkersMarkIn);
    ui->actionMarkersMarkIn->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersMarkOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersMarkOut);
    this->addAction(ui->actionMarkersMarkOut);
    ui->actionMarkersMarkOut->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionRendererVideoWidget, &QAction::toggled, this, &VuraMainWindow::actionRendererVideoWidget_toggled);
    connect(ui->actionRendererOpenGL, &QAction::toggled, this, &VuraMainWindow::actionRendererOpenGLWidget_toggled);

    connect(&m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, &VuraMainWindow::populateAudioDevicesMenu);
    populateAudioDevicesMenu();

    m_crashReporter = new CrashReporter(this);
    connect(m_crashReporter, &CrashReporter::scanFinished, this, &VuraMainWindow::crashReportScanFinished);
    connect(m_crashReporter, &CrashReporter::uploadStarted, this, &VuraMainWindow::crashReportUploadStarted);
    connect(m_crashReporter, &CrashReporter::finished, this, &VuraMainWindow::crashReportUploadFinished);
    m_crashReporter->checkForPreviousCrashes();
}

void VuraMainWindow::setMainWindowVisibility(const bool state)
{
    if (state) {
        this->show();
        this->showNormal();
        this->raise();
        this->activateWindow();
    }
}

void VuraMainWindow::openFile(const QString &file) const
{
    QStringList fileList;
    if (!file.isEmpty()) {
        fileList << file;
        m_playlistController->filesDropped(fileList, true);
    }
}

void VuraMainWindow::openFolder(const QString &path) const
{
    QStringList fileList;

    if (!path.isEmpty()) {
        QDirIterator folderIterator(path, QDir::Files | QDir::NoDotAndDotDot);
        while (folderIterator.hasNext()) {
            folderIterator.next();
            fileList << folderIterator.filePath();
        }

        m_playlistController->filesDropped(fileList, true);
    }
}

void VuraMainWindow::openNetworkStream(QString networkUrl) const
{
    qDebug() << "Open with network stream requested. Network URL: " << networkUrl;
    if (!networkUrl.isEmpty()) {
        m_playlistController->hidePlaylist();

        networkUrl.remove('"');
        networkUrl.remove(0, 7);
        networkUrl.insert(5, ":");
        qDebug() << "Cleaned Network URL: " << networkUrl;
        m_playlistController->addNetworkVideo(networkUrl);
    }
}

void VuraMainWindow::initializeVideoWidget()
{
    ui->videoWidget->setMouseTracking(true);
    if (!ui->videoWidget->children().isEmpty()) {
        const auto videoChild = qobject_cast<QWidget*>(ui->videoWidget->children().first());
        if (videoChild) {
            videoChild->setMouseTracking(true);
            videoChild->installEventFilter(this);
        }
    }
}

void VuraMainWindow::closeEvent(QCloseEvent *event)
{
    m_videoMarkerController->saveVideoMarkers();
    event->accept();
}

bool VuraMainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        const auto msg = static_cast<MSG *>(message);
        if (msg->message == WM_NCLBUTTONDBLCLK)
        {
            this->resize(1200, 700);
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
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QStringList droppedFiles;
        for (const QUrl &url : mimeData->urls()) {
            droppedFiles << url.toLocalFile();
        }
        m_playlistController->filesDropped(droppedFiles);
        event->acceptProposedAction();
    }
}

void VuraMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (this->isFullScreen() && event->key() == Qt::Key_Escape) {
        actionToggleFullscreen();
        event->accept();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

bool VuraMainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        const auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);

        QPointF localPos = mouseEvent->position();

        this->unsetCursor();
        m_videoSliderWidget->show();
        if (m_currentPlaybackState == Playing) {
            m_videoSliderHideTimer->start();
        }
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        if (m_currentPlaybackState == Playing) {
            m_playbackController->pause();
        } else if (m_currentPlaybackState == Paused) {
            m_playbackController->play();
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void VuraMainWindow::stateChanged(const PlaybackState state)
{
    m_currentPlaybackState = state;
    if (state == Playing && !m_videoSliderHideTimer->isActive()) {
        m_videoSliderHideTimer->start();
    } else {
        m_videoSliderHideTimer->stop();
        m_videoSliderWidget->show();
        this->unsetCursor();
    }
}

void VuraMainWindow::sourceChanged(const QUrl &source)
{
    qCDebug(Core) << "Source changed to: " << source.toLocalFile();
    m_videoMarkerController->loadVideoMarkers(source);
    setApplicationWindowTitle();
}

void VuraMainWindow::errorOccurred(const QString &errorMessage)
{
    qCCritical(Core) << "QMediaPlayer Error: " << errorMessage;
    QMessageBox::critical(this, "Media Player Error", errorMessage);
}

void VuraMainWindow::hideVideoSlider()
{
    const QSettings settings;
    if (m_currentPlaybackState == Playing && settings.value("autohideSlider", true).toBool()) {
        m_videoSliderWidget->hide();
        this->setCursor(Qt::BlankCursor);
    }
}

void VuraMainWindow::resetVideoSliderVisibility()
{
    const QSettings settings;

    if (settings.value("autohideSlider", true).toBool()) {
        m_videoSliderHideTimer->stop();
        m_videoSliderWidget->show();
        this->unsetCursor();

        if (m_currentPlaybackState == Playing && !m_videoSliderHideTimer->isActive())
            m_videoSliderHideTimer->start();
    }
}

void VuraMainWindow::updateCheckReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Network Error:" << reply->errorString();
        return;
    }

    const QByteArray response = reply->readAll();
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();

    bool isRemoteBeta = jsonObj["is_beta"].toBool();
    const QString remoteVersion = jsonObj["version"].toString();
    const QString releaseDate = jsonObj["release_date"].toString();

    if (remoteVersion != VURA_VERSION_STRING) {
        QJsonObject platforms = jsonObj["platforms"].toObject();

#if defined(Q_OS_WIN)
        QJsonObject currentPlatform = platforms["windows"].toObject();
#elif defined(Q_OS_MAC)
        QJsonObject currentPlatform = platforms["mac"].toObject();
#else
        QJsonObject currentPlatform = platforms["linux"].toObject();
#endif

        const QString downloadUrl = currentPlatform["url"].toString();
        const QString expectedHash = currentPlatform["sha256"].toString();
        const QString changelogUrl = jsonObj["changelog_url"].toString();


        const QSettings settings;
        const QString lastCheckedVersion = settings.value("lastCheckedVersion", "").toString();
        if (remoteVersion != lastCheckedVersion) {
            if (m_updateDialog)
                m_updateDialog->close();

            m_updateDialog = new UpdateDialog(remoteVersion, releaseDate, downloadUrl, changelogUrl, expectedHash, this);
            connect(m_updateDialog, &UpdateDialog::updateNow, this, &VuraMainWindow::onUpdateConfirmed);
            m_updateDialog->show();
            m_updateDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        }
    }

    reply->deleteLater();
}

void VuraMainWindow::actionHelpCheckForUpdates()
{
    const QSettings settings;
    const bool acceptBetas = settings.value("updates/accept_betas", false).toBool();

    const QString manifestFile = acceptBetas ? "beta.json" : "stable.json";

    m_updateNetworkManager = new QNetworkAccessManager(this);
    connect(m_updateNetworkManager, &QNetworkAccessManager::finished, this, &VuraMainWindow::updateCheckReplyFinished);

    const QUrl url(QString("https://storage.hale-tech.net").arg(manifestFile));
    const QNetworkRequest request(url);

    qDebug() << "Checking for updates using URL: " << url << "...";
    m_updateNetworkManager->get(request);
}

void VuraMainWindow::actionTestFunction() {}

void VuraMainWindow::actionOpenNetworkStream()
{
    bool ok;
    const QString networkUrl = QInputDialog::getText(this,
            tr("Open Network Stream"),
            tr("Network Stream URL:"),
            QLineEdit::Normal,
            QString(),
            &ok);

    if (ok && !networkUrl.isEmpty()) {
        m_playlistController->addNetworkVideo(networkUrl);
    }
}

void VuraMainWindow::actionEmergencyClose()
{
    m_playbackController->getVideoWidget()->pause();
    this->setWindowState(Qt::WindowMinimized);
}

void VuraMainWindow::actionShowLogViewer()
{
    if (m_logViewerDialog)
        m_logViewerDialog->close();

    m_logViewerDialog = new LogViewerDialog(this);
    m_logViewerDialog->show();
    m_logViewerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void VuraMainWindow::actionToggleFullscreen()
{
    if (this->isFullScreen()) {
        this->showNormal();

        ui->menubar->show();
        m_videoSliderWidget->show();
        if (m_wasPlaylistShowing) m_playlistController->showPlaylist();

    } else {
        m_wasPlaylistShowing = m_playlistController->isPlaylistVisible();
        this->showFullScreen();

        ui->menubar->hide();
        m_videoSliderWidget->hide();
        m_playlistController->hidePlaylist();
    }
}

void VuraMainWindow::actionShowSettings()
{
    if (m_settingsDialog)
        m_settingsDialog->close();

    m_settingsDialog = new SettingsDialog(this);
    m_settingsDialog->show();
    m_settingsDialog->setAttribute(Qt::WA_DeleteOnClose, true);
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

void VuraMainWindow::actionToggleVideoControls()
{
    if (m_showingVideoControls) {
        ui->verticalLayout->removeWidget(m_videoControlWidget);
        delete m_videoControlWidget;
        m_showingVideoControls = false;
    } else {
        m_videoControlWidget = new VideoControlWidget(this);
        m_videoControlWidget->setMuted(m_playbackController->getAudioOutput()->isMuted());
        m_videoControlWidget->setVolume(m_playbackController->getAudioOutput()->volume());

        ui->verticalLayout->addWidget(m_videoControlWidget);

        connect(m_playbackController->getVideoWidget(), &QMediaPlayer::playbackStateChanged, m_videoControlWidget, &VideoControlWidget::setState);
        connect(m_videoControlWidget, &VideoControlWidget::play, m_playbackController, &PlaybackController::play);
        connect(m_videoControlWidget, &VideoControlWidget::pause, m_playbackController, &PlaybackController::pause);
        connect(m_videoControlWidget, &VideoControlWidget::stop, m_playbackController, &PlaybackController::stop);

        m_showingVideoControls = true;
    }
    ui->actionViewToggleVideoControls->setChecked(m_showingVideoControls);
}

void VuraMainWindow::populateAudioDevicesMenu()
{
    ui->menuAudioDevice->clear();

    auto* deviceGroup = new QActionGroup(this);
    deviceGroup->setExclusive(true);

    const QAudioDevice currentDevice = m_playbackController->getAudioOutput()->device();

    for (const QAudioDevice &device : QMediaDevices::audioOutputs()) {
        auto *action = new QAction(device.description(), this);
        action->setCheckable(true);

        if (device.id() == currentDevice.id()) {
            action->setChecked(true);
        }

        deviceGroup->addAction(action);
        ui->menuAudioDevice->addAction(action);

        connect(action, &QAction::triggered, this, [this, device]() {
            m_playbackController->getAudioOutput()->setDevice(device);
        });
    }
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

void VuraMainWindow::actionMarkersAddCumshotMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addCumshotMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddCyanMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addCyanMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddDialogMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addDialogMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddMagentaMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addMagentaMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddOrangeMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addOrangeMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddSceneMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addSceneMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddStripMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addStripMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersClearIn() {}

void VuraMainWindow::actionMarkersClearInOut() {}

void VuraMainWindow::actionMarkersClearMarkers() {}

void VuraMainWindow::actionMarkersClearOut() {}

void VuraMainWindow::actionMarkersClearSelectedMarker() {}

void VuraMainWindow::actionMarkersEditSelectedMarker() {}

void VuraMainWindow::actionMarkersGoToIn() {}

void VuraMainWindow::actionMarkersGoToNextMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoSlider->goToNextMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersGoToOut() {}

void VuraMainWindow::actionMarkersGoToPreviousMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoSlider->goToPreviousMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersMarkIn() {}

void VuraMainWindow::actionMarkersMarkOut() {}

void VuraMainWindow::actionRendererVideoWidget_toggled(const bool checked) const
{
    ui->actionRendererOpenGL->setChecked(!checked);
    ui->actionRendererVideoWidget->setChecked(checked);
}

void VuraMainWindow::actionRendererOpenGLWidget_toggled(const bool checked) const
{
    ui->actionRendererOpenGL->setChecked(checked);
    ui->actionRendererVideoWidget->setChecked(!checked);
}

void VuraMainWindow::setTrackInfo(const QString &trackInfo)
{
    m_trackInfo = trackInfo;
    this->setWindowTitle("Vura - " + trackInfo);
}

void VuraMainWindow::setApplicationWindowTitle()
{
    QString windowTitle;
    if (!m_playbackController->getVideoWidget()->source().isEmpty()) {
        windowTitle = QString("%1 - Vura %2")
                        .arg(Helpers::strippedFileName(m_playbackController->getVideoWidget()->source().toLocalFile()),
                                                        VURA_VERSION_STRING);
    } else {
        windowTitle = QString("Vura %1").arg(VURA_VERSION_STRING);
    }
    this->setWindowTitle(windowTitle);
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
    ui->actionMarkersEditSelectedMarker->setEnabled(checkMarkerProximity());
}

VuraVideoMarker VuraMainWindow::findNearestVisibleMarker(const double sliderPercent, const double markerRange) const
{
    VuraVideoMarker best;
    best.timestamp = std::numeric_limits<double>::quiet_NaN();

    for (const VuraVideoMarker &marker : m_videoMarkerController->getVideoMarkers()) {
        if (!m_videoSlider->getMarkerTypesVisible(marker.markerType)) continue;
        const double dist = std::abs(marker.timestamp - sliderPercent);
        if (dist > markerRange) continue;
        if (std::isnan(best.timestamp) || dist < std::abs(best.timestamp - sliderPercent))
            best = marker;
    }
    return best;
}

double VuraMainWindow::getSliderPercent() const
{
    const double distanceFromMin = m_videoSlider->GetValue() - m_videoSlider->GetMinimun();
    const double sliderRange = m_videoSlider->GetMaximun() - m_videoSlider->GetMinimun();
    return distanceFromMin / sliderRange;
}

bool VuraMainWindow::checkMarkerProximity() const
{
    const double sliderPercent = getSliderPercent();
    constexpr double markerProximityThreshold = 0.005;

    const auto isVisibleMarkerNearSlider = [this, sliderPercent](const VuraVideoMarker &marker) {
        if (!m_videoSlider->getMarkerTypesVisible(marker.markerType)) {
            return false;
        }

        const double distanceToSlider = std::abs(marker.timestamp - sliderPercent);
        return distanceToSlider <= markerProximityThreshold;
    };

    return std::any_of(
        m_videoMarkerController->getVideoMarkers().cbegin(),
        m_videoMarkerController->getVideoMarkers().cend(),
        isVisibleMarkerNearSlider);
}

bool VuraMainWindow::isPreviousMarkerAvailable(const VuraVideoMarker &videoMarker) const
{
    VuraVideoMarker previousMarker;
    previousMarker.timestamp = std::numeric_limits<double>::quiet_NaN();

    for (const VuraVideoMarker &marker : m_videoMarkerController->getVideoMarkers()) {
        if (marker.timestamp < videoMarker.timestamp) {
            if (std::isnan(previousMarker.timestamp)) {
                previousMarker = marker;

            } else {
                if (marker.timestamp > previousMarker.timestamp) {
                    previousMarker = marker;
                }
            }
        }
    }

    if (std::isnan(previousMarker.timestamp)) {
        return false;
    }
    return true;
}

bool VuraMainWindow::isNextMarkerAvailable(const VuraVideoMarker &videoMarker) const
{
    VuraVideoMarker nextMarker;
    nextMarker.timestamp = std::numeric_limits<double>::quiet_NaN();

    for (const VuraVideoMarker &marker : m_videoMarkerController->getVideoMarkers()) {
        if (marker.timestamp > videoMarker.timestamp) {
            if (std::isnan(nextMarker.timestamp)) {
                nextMarker = marker;

            } else {
                if (marker.timestamp < nextMarker.timestamp) {
                    nextMarker = marker;
                }
            }
        }
    }

    if (std::isnan(nextMarker.timestamp)) {
        return false;
    }
    return true;
}

void VuraMainWindow::onUpdateConfirmed(const QString &targetDownloadUrl, const QString &expectedHash)
{
    auto *progressDialog = new QProgressDialog(tr("Downloading Update..."), tr("Cancel"), 0, 100, this);
    progressDialog->setWindowModality(Qt::WindowModal);

    auto *updater = new Updater(this);

    connect(updater, &Updater::downloadProgress, this, [progressDialog](const qint64 received, const qint64 total) {
        if (total > 0) {
            const int percentage = static_cast<int>((received * 100) / total);
            progressDialog->setValue(percentage);
        }
    });

    connect(progressDialog, &QProgressDialog::canceled, updater, []() {
        // Handle download abortion if necessary
    });

    connect(updater, &Updater::downloadFinished, this, [progressDialog](const bool success, const QString &message) {
        progressDialog->close();
        if (!success) {
            QMessageBox::critical(nullptr, tr("Update Error"), message);
        }
    });

    updater->startDownload(targetDownloadUrl, expectedHash);
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

void VuraMainWindow::configureUpdater()
{
    /*
    auto* updater = new UpdateChecker(this);

    connect(updater, &UpdateChecker::noUpdateAvailable, this, []() {
        qDebug() << "Vura is up to date";
    });

    // Show a non-blocking notification when an update is found.
    // The download of updater.exe is already running in the background
    // at this point — we just let the user know what's happening.
    connect(updater, &UpdateChecker::updateAvailable,
            this,    [this](const QString& version) {
        // Use a non-modal notification so the user can keep using the app
        // while updater.exe downloads in the background.
                ui->statusBar->show();
        ui->statusBar->showMessage(
            QString("Update %1 found — downloading updater...").arg(version),
            0 // 0 = show until cleared
        );
    });

    // Updater is downloaded and is about to launch — inform the user
    // that the app will close.
    connect(updater, &UpdateChecker::updateReadyToInstall, this, [this]() {
        QMessageBox::information(
            this,
            "Update Ready",
            "A new version of Vura is ready to install.\n\n"
            "Vura will close now and the updater will run automatically.\n"
            "Vura will relaunch when the update is complete."
        );
        // UpdateChecker calls QCoreApplication::quit() after this signal,
        // so we don't need to do anything else here.
    });

    // Show errors in the status bar — don't bother the user with a dialog
    // for a background update check failure
    connect(updater, &UpdateChecker::error, this, [this](const QString& msg) {
        ui->statusBar->showMessage(QString("Update check: %1").arg(msg), 8000);
        qWarning() << "UpdateChecker:" << msg;
    });

    // Check on startup — slightly delayed so the main window appears first
    //QTimer::singleShot(3000, updater, &UpdateChecker::check);

    connect(ui->actionHelpCheckForUpdates, &QAction::triggered, updater, &UpdateChecker::check);
*/
}
