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

#include <libvura/media-io/media-functions.h>

#include <ui-config.h>
#include <qglobal.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif


static Blogger* globalRedirector = nullptr;

VuraMainWindow::VuraMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::VuraMainWindow)
{
    const QSettings settings;

    setAcceptDrops(true);

    ui->setupUi(this);

    qInstallMessageHandler(Blogger::messageHandler);
    globalRedirector = Blogger::instance();

    m_playbackController = new PlaybackController(this);
    m_playlistController = new PlaylistController(
            ui->playlistView,
            ui->emptyPlaylistView,
            ui->playlistWidget,
            this
        );

    m_playbackController->getPlayer()->setVideoOutput(ui->videoWidget);
    connect(m_playlistController, &PlaylistController::playTrackRequested, m_playbackController, &PlaybackController::playTrack);

    if (settings.value("showPlaylistOnStart", false).toBool()) {
        m_playlistController->showPlaylist();
    } else {
        m_playlistController->hidePlaylist();
    }

    //connect(ui->playlistView, &QListView::customContextMenuRequested, this, &VuraMainWindow::showPlaylistContextMenu);


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

    // Video Slider
    m_videoSlider = new VideoSlider(&m_videoMarkers, this);
    m_videoSliderWidget = new VideoSliderWidget(*m_videoSlider, *(m_playbackController->getPlayer()), this);

    ui->verticalLayout->addWidget(m_videoSliderWidget);
    ui->verticalLayout->setStretch(0, 1);

    connect(this, &VuraMainWindow::updateVideoSlider, m_videoSlider, &VideoSlider::updateVideoSlider);
    connect(m_playbackController, &PlaybackController::positionChanged, m_videoSlider, &VideoSlider::setValue);
    connect(m_playbackController, &PlaybackController::durationChanged, m_videoSlider, &VideoSlider::setMaximum);
    connect(m_playbackController, &PlaybackController::sourceChanged, this, &VuraMainWindow::sourceChanged);
    connect(m_videoSlider, &VideoSlider::valueChanged, m_playbackController, &PlaybackController::setPosition);
    connect(m_videoSlider, &VideoSlider::sliderPressed, m_playbackController, &PlaybackController::setPaused);

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

    connect(ui->actionFileOpenMultipleFiles, &QAction::triggered, m_playlistController, &PlaylistController::requestMultipleFileImport);
    this->addAction(ui->actionFileOpenMultipleFiles);
    ui->actionFileOpenMultipleFiles->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileSavePlaylist, &QAction::triggered, m_playlistController, &PlaylistController::savePlaylistAs);
    this->addAction(ui->actionFileSavePlaylist);
    ui->actionFileSavePlaylist->setShortcutContext(Qt::WindowShortcut);
    
    connect(ui->actionFileOpenPlaylist, &QAction::triggered, m_playlistController, &PlaylistController::loadPlaylistFile);
    this->addAction(ui->actionFileOpenPlaylist);
    ui->actionFileOpenPlaylist->setShortcutContext(Qt::WindowShortcut);

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

    connect(&m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, &VuraMainWindow::populateAudioDevicesMenu);
    populateAudioDevicesMenu();

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
    qCDebug(Core) << "Application Initialized!";
    qCInfo(Core) << "Vura Version: " << VURA_VERSION_STRING;
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

void VuraMainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

bool VuraMainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG *msg = static_cast<MSG *>(message);
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

void VuraMainWindow::sourceChanged(const QUrl &source)
{
    QSettings settings;

    m_videoMarkers.clear();
    m_videoMarkers = VideoMarkers::read("debug/global.json", source.toString());

    m_videoSlider->setSource(source.toLocalFile());
    qCDebug(Core) << "Source changed to: " << source.toLocalFile();

    setApplicationWindowTitle();
}

void VuraMainWindow::errorOccurred(const QString &errorMessage)
{
    qCCritical(Core) << "QMediaPlayer Error: " << errorMessage;
    QMessageBox::critical(this, "Media Player Error", errorMessage);
    //this->close();
}

void VuraMainWindow::actionTestFunction()
{
    qCDebug(Core) << "Test function called.";
    qCInfo(Core) << "Test function called.";
    qCWarning(Core) << "Test function called.";
    qCCritical(Core) << "Test function called.";
}

void VuraMainWindow::actionEmergencyClose()
{
    m_playbackController->getPlayer()->pause();
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
        if (m_wasPlaylistShowing) m_playlistController->showPlaylist();

    } else {
        m_wasPlaylistShowing = m_playlistController->isPlaylistVisible();
        this->showFullScreen();

        ui->menubar->hide();
        m_playlistController->hidePlaylist();
    }
}

void VuraMainWindow::actionShowSettings()
{
    if (m_settingsWindow)
        m_settingsWindow->close();

    m_settingsWindow = new SettingsWindow(this);
    m_settingsWindow->show();
    m_settingsWindow->setAttribute(Qt::WA_DeleteOnClose, true);
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

        // Use getters from PlaybackController to set initial states
        m_videoControlWidget->setMuted(m_playbackController->getAudioOutput()->isMuted());
        m_videoControlWidget->setVolume(m_playbackController->getAudioOutput()->volume());

        ui->verticalLayout->addWidget(m_videoControlWidget);

        // Connect UI controls directly to PlaybackController
        connect(m_playbackController->getPlayer(), &QMediaPlayer::playbackStateChanged, m_videoControlWidget, &VideoControlWidget::setState);
        connect(m_videoControlWidget, &VideoControlWidget::play, m_playbackController, &PlaybackController::play);
        connect(m_videoControlWidget, &VideoControlWidget::pause, m_playbackController, &PlaybackController::pause);
        connect(m_videoControlWidget, &VideoControlWidget::stop, m_playbackController, &PlaybackController::stop);

        m_showingVideoControls = true;
    }
}

void VuraMainWindow::populateAudioDevicesMenu()
{
    ui->menuAudioDevice->clear();

    QActionGroup* deviceGroup = new QActionGroup(this);
    deviceGroup->setExclusive(true);

    QAudioDevice currentDevice = m_playbackController->getAudioOutput()->device();

    for (const QAudioDevice &device : QMediaDevices::audioOutputs()) {
        QAction *action = new QAction(device.description(), this);
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

void VuraMainWindow::setTrackInfo(const QString &trackInfo)
{
    m_trackInfo = trackInfo;
    this->setWindowTitle("Vura - " + trackInfo);
}

void VuraMainWindow::setApplicationWindowTitle()
{
    QString windowTitle;

    if (!m_playbackController->getPlayer()->source().isEmpty()) {
        windowTitle = QString("%1 - Vura %2").arg(MediaFunctions::strippedFileName(m_playbackController->getPlayer()->source().toLocalFile()), VURA_VERSION_STRING);
    } else {
        windowTitle = QString("Vura %1").arg(VURA_VERSION_STRING);
    }

    this->setWindowTitle(windowTitle);
}

QString VuraMainWindow::trackName(const QMediaMetaData &metaData, int index)
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
