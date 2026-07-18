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

VuraMainWindow::VuraMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::VuraMainWindow)
{
    QSettings settings;

    setAcceptDrops(true);

    ui->setupUi(this);
    qInstallMessageHandler(Blogger::messageHandler);
    globalRedirector = Blogger::instance();

    ui->statusBar->hide();

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setVideoOutput(ui->videoWidget);
    m_videoSink = m_player->videoSink();

    connect(m_player, &QMediaPlayer::durationChanged, this, &VuraMainWindow::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &VuraMainWindow::positionChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &VuraMainWindow::statusChanged);
    connect(m_player, &QMediaPlayer::bufferProgressChanged, this, &VuraMainWindow::bufferingProgress);
    connect(m_player, &QMediaPlayer::errorChanged, this, &VuraMainWindow::displayErrorMessage);
    connect(m_player, &QMediaPlayer::sourceChanged, this, &VuraMainWindow::sourceChanged);
    connect(m_player, &QMediaPlayer::tracksChanged, this, &VuraMainWindow::tracksChanged);
    connect(m_player, &QMediaPlayer::playbackRateChanged, this, &VuraMainWindow::playbackRateChanged);
    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &VuraMainWindow::videoFrameChanged);

    m_playlistModel = new PlaylistModel(this);
    m_playlist = m_playlistModel->playlist();
    ui->playlistView->setModel(m_playlistModel);
    ui->playlistView->setCurrentIndex(m_playlistModel->index(m_playlist->currentIndex(), 0));
    if (settings.value("showPlaylistOnStart", false).toBool()) {
        ui->playlistWidget->show();
    } else {
        ui->playlistWidget->hide();
    }

    connect(m_playlist, &Playlist::currentIndexChanged, this, &VuraMainWindow::playlistPositionChanged);
    connect(ui->playlistView, &QListView::activated, this, &VuraMainWindow::jump);
    //connect(ui->playlistView, &QListView::customContextMenuRequested, this, &VuraMainWindow::showPlaylistContextMenu);

    //m_playlistController = new PlaylistController(
    //        ui->playlistView,
    //        ui->emptyPlaylistView,
    //        ui->playlistWidget,
    //        this
    //    );

    m_videoSlider = new VideoSlider(&m_videoMarkers, this);
    m_videoSliderWidget = new VideoSliderWidget(*m_videoSlider, *m_player, this);

    ui->verticalLayout->addWidget(m_videoSliderWidget);
    ui->verticalLayout->setStretch(0, 1);

    connect(this, &VuraMainWindow::updateVideoSlider, m_videoSlider, &VideoSlider::updateVideoSlider);
    connect(m_videoSlider, &VideoSlider::valueChanged, this, &VuraMainWindow::seek);
    connect(m_videoSlider, &VideoSlider::sliderPressed, this, &VuraMainWindow::seek);
    //connect(m_playbackController, &PlaybackController::positionChanged, m_videoSlider, &VideoSlider::setValue);
    //connect(m_playbackController, &PlaybackController::durationChanged, m_videoSlider, &VideoSlider::setMaximum);
    //connect(m_playbackController, &PlaybackController::sourceChanged, this, &VuraMainWindow::sourceChanged);
    //connect(m_playlistController, &PlaylistController::playTrackRequested, m_playbackController, &PlaybackController::playTrack);
    //connect(ui->emptyPlaylistView, &PlaylistEmptyStateWidget::requestFileImport, m_playlistController, &PlaylistController::requestFileImport);
    //connect(m_playbackController, &PlaybackController::positionChanged, this, &VuraMainWindow::);
    //connect(m_playbackController, &PlaybackController::durationChanged, this, &VuraMainWindow::);


    //connect(ui->actionAudioToggleMute, &QAction::triggered, m_playbackController, &PlaybackController::toggleMute);
    connect(ui->actionAudioVolumeDown, &QAction::triggered, this, &VuraMainWindow::actionVolumeDown);
    connect(ui->actionAudioVolumeUp, &QAction::triggered, this, &VuraMainWindow::actionVolumeUp);
    connect(ui->actionFileEmergencyClose, &QAction::triggered, this, &VuraMainWindow::actionEmergencyClose);
    connect(ui->actionFileExit, &QAction::triggered, this, &VuraMainWindow::actionExit);
    //connect(ui->actionFileOpenFile, &QAction::triggered, m_playlistController, &PlaylistController::requestFileImport);
    connect(ui->actionFileOpenFile, &QAction::triggered, this, &VuraMainWindow::actionOpenFile);
    connect(ui->actionFileOpenFolder, &QAction::triggered, this, &VuraMainWindow::actionOpenFolder);
    connect(ui->actionFileOpenMultipleFiles, &QAction::triggered, this, &VuraMainWindow::actionOpenMultipleFiles);
    connect(ui->actionViewPreferences, &QAction::triggered, this, &VuraMainWindow::actionShowSettings);
    connect(ui->actionHelpViewCurrentLog, &QAction::triggered, this, &VuraMainWindow::actionShowLogViewer);
    //connect(ui->actionPlaybackNext, &QAction::triggered, m_playlistController, &PlaylistController::nextTrack);
    //connect(ui->actionPlaybackPrevious, &QAction::triggered, m_playlistController, &PlaylistController::previousTrack);
    connect(ui->actionPlaybackNext, &QAction::triggered, this, &VuraMainWindow::actionNext);
    connect(ui->actionPlaybackPrevious, &QAction::triggered, this, &VuraMainWindow::actionPrevious);
    connect(ui->actionPlaybackRestartVideo, &QAction::triggered, this, &VuraMainWindow::actionRestart);
    connect(ui->actionPlaybackTogglePlay, &QAction::triggered, this, &VuraMainWindow::actionTogglePlay);
    connect(ui->actionToolsTestFunction, &QAction::triggered, this, &VuraMainWindow::actionTestFunction);
    connect(ui->actionVideoFullscreen, &QAction::triggered, this, &VuraMainWindow::actionToggleFullscreen);
    connect(ui->actionViewTogglePlaylist, &QAction::triggered, this, &VuraMainWindow::actionTogglePlaylist);
    connect(ui->actionViewToggleVideoControls, &QAction::triggered, this, &VuraMainWindow::actionToggleVideoControls);
    connect(ui->actionPlaybackJumpForwardExtraLarge, &QAction::triggered, this, &VuraMainWindow::actionJumpForwardExtraLarge);
    connect(ui->actionPlaybackJumpBackwardExtraLarge, &QAction::triggered, this, &VuraMainWindow::actionJumpBackwardExtraLarge);
    connect(ui->actionPlaybackJumpForwardLarge, &QAction::triggered, this, &VuraMainWindow::actionJumpForwardLarge);
    connect(ui->actionPlaybackJumpBackwardLarge, &QAction::triggered, this, &VuraMainWindow::actionJumpBackwardLarge);
    connect(ui->actionPlaybackJumpForwardMedium, &QAction::triggered, this, &VuraMainWindow::actionJumpForwardMedium);
    connect(ui->actionPlaybackJumpBackwardMedium, &QAction::triggered, this, &VuraMainWindow::actionJumpBackwardMedium);
    connect(ui->actionPlaybackJumpForwardSmall, &QAction::triggered, this, &VuraMainWindow::actionJumpForwardSmall);
    connect(ui->actionPlaybackJumpBackwardSmall, &QAction::triggered, this, &VuraMainWindow::actionJumpBackwardSmall);
    connect(ui->actionPlaybackJumpForwardExtraSmall, &QAction::triggered, this, &VuraMainWindow::actionJumpForwardExtraSmall);
    connect(ui->actionPlaybackJumpBackwardExtraSmall, &QAction::triggered, this, &VuraMainWindow::actionJumpBackwardExtraSmall);

    //connect(ui->action, &QAction::triggered, this, &VuraMainWindow::);

    //m_playlistController->hidePlaylist();


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




    qCDebug(Core) << "Application Initialized!";
    qCInfo(Core) << "Vura Version: " << VURA_VERSION_STRING;
}

void VuraMainWindow::setMainWindowVisibility(bool state)
{
    if (state) {
        this->show();
        this->showNormal();
        this->raise();
        this->activateWindow();
    }
}

void VuraMainWindow::openFolderContextMenu(const QString &path) {}

void VuraMainWindow::openFileContextMenu(const QString &file)
{
    //const int previousMediaCount = m_playlistController->getModel()->rowCount();
    const int previousMediaCount = m_playlist->mediaCount();
    if (!file.isEmpty()) {
        QUrl url = QUrl::fromLocalFile(file);
        if (!MediaFunctions::isPlaylist(url)) {
            m_playlist->addMedia(url);
            if (m_playlist->mediaCount() > previousMediaCount) {
                const auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                jump(index);
            }
            //m_playlistController->getModel()->addItem({url.toString(), url.toString(), 0, 0, false});
            //if (m_playlistController->getModel()->rowCount() > previousMediaCount) {
            //    auto index = m_playlistController->getModel()->index(previousMediaCount, 0);
            //    ui->playlistView->setCurrentIndex(index);
            //    jump(index);
            //}
        } else {
            //m_playlist->loadPlaylist(file);
            //if (m_playlist->mediaCount() > previousMediaCount) {
            //    const auto index = m_playlistModel->index(previousMediaCount, 0);
            //    ui->playlistView->setCurrentIndex(index);
            //    jump(index);
            //}
        }
    }
}

void VuraMainWindow::addFileToPlaylistContextMenu(const QString &file) const
{
    if (!file.isEmpty())
        m_playlist->addMedia(QUrl::fromLocalFile(file));
    //if (!file.isEmpty())
    //    m_playlistController->getModel()->addItem({file, file, 0, 0, false});
}

void VuraMainWindow::addFolderToPlaylistContextMenu(const QString &path)
{
    qDebug() << "Add Folder to Playlist path: " << path;

    const int previousMediaCount = m_playlist->mediaCount();

    if (!path.isEmpty()) {
        QList<QUrl> filesList;
        QDirIterator folderIterator(path, QDir::Files | QDir::NoDotAndDotDot);
        while (folderIterator.hasNext()) {
            folderIterator.next();
            filesList.append(QUrl::fromLocalFile(folderIterator.filePath()));
        }

        for (auto &fileUrl : filesList) {
            if (!MediaFunctions::isPlaylist(fileUrl)) {
                m_playlist->addMedia(fileUrl);
            } else {
                QMessageBox::information(this, "Vura", "Playlist file in folder is being skipped.");
            }
        }

        if (m_playlist->mediaCount() > previousMediaCount) {
            auto index = m_playlistModel->index(previousMediaCount, 0);
            ui->playlistView->setCurrentIndex(index);
            jump(index);
        }
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

    // For other events, or if not handled, call the base implementation
    return QWidget::nativeEvent(eventType, message, result);
}

void VuraMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qCDebug(Core) << "Drop event received in VuraMainWindow";
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
        QList<QUrl> urlList = mimeData->urls();

        // Iterate through all dropped files/folders
        for (const QUrl &url : urlList) {
            // Convert file protocol (file://) to local OS path string
            QString filePath = url.toLocalFile();

            if (!filePath.isEmpty()) {
                qCDebug(Core) << "Dropped File Path:" << filePath;

                // Optional: Check if item is a file or folder
                QFileInfo fileInfo(filePath);
                if (fileInfo.isDir()) {
                    //m_playbackController->loadMedia(filePath);
                    // Handle folder entry
                } else {
                    // Handle file entry
                    m_player->setSource(filePath);
                }
            }
        }
        event->acceptProposedAction();
    }
}

void VuraMainWindow::sourceChanged(const QUrl &source)
{
    QSettings settings;

    m_videoMarkers.clear();
    m_videoMarkers = VideoMarkers::read("debug/global.json", source.toString());

    m_videoSlider->setSource(source.toLocalFile());
    qCDebug(Core) << "Source changed to: " << source.toLocalFile();
}

void VuraMainWindow::errorOccurred(const QString &errorMessage)
{
    qCCritical(Core) << "QMediaPlayer Error: " << errorMessage;
    QMessageBox::critical(this, "Media Player Error", errorMessage);
    this->close();
}

void VuraMainWindow::playlistPositionChanged(int index)
{
    ui->playlistView->setCurrentIndex(m_playlistModel->index(index, 0));
    m_player->setSource(m_playlist->currentMedia());
}

void VuraMainWindow::actionTestFunction()
{
    qCDebug(Core) << "Test function called.";
    qCInfo(Core) << "Test function called.";
    qCWarning(Core) << "Test function called.";
    qCCritical(Core) << "Test function called.";
}

void VuraMainWindow::actionOpenFile()
{
    QSettings settings;
    const int previousMediaCount = m_playlist->mediaCount();
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
        "All Files (*)");

    if (!fileName.isEmpty()) {
        QUrl url = QUrl::fromLocalFile(fileName);
        if (!MediaFunctions::isPlaylist(url)) {
            m_playlist->addMedia(url);
            if (previousMediaCount > 0) {
                if (m_playlist->mediaCount() > previousMediaCount) {
                    const auto index = m_playlistModel->index(previousMediaCount, 0);
                    ui->playlistView->setCurrentIndex(index);
                    jump(index);
                }
            } else {
                m_playlist->next();
            }
        } else {

        }
        settings.setValue("lastFileDirectory", QFileInfo(fileName).path());
    }
}

void VuraMainWindow::actionOpenFolder()
{
    QSettings settings;
    const int previousMediaCount = m_playlist->mediaCount();
    const QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Open Folder"),
            settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!dir.isEmpty()) {
        QStringList filePaths;
        const QDir directory(dir);
        QStringList filters;
        filters << "*.mp4" << "*.mkv" << "*.avi" << "*.mp3" << "*.wav" << "*.flac";

        QFileInfoList fileInfoList = directory.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);
        for (const QFileInfo& fileInfo : fileInfoList) {
            filePaths << fileInfo.absoluteFilePath();
        }

        for (auto &filePath : filePaths) {
            QUrl url = QUrl::fromLocalFile(filePath);
            if (!MediaFunctions::isPlaylist(url)) {
                m_playlist->addMedia(url);
            } else {

            }
        }

        if (m_playlist->mediaCount() > previousMediaCount) {
            const auto index = m_playlistModel->index(previousMediaCount, 0);
            ui->playlistView->setCurrentIndex(index);
            jump(index);

            settings.setValue("lastFileDirectory", QFileInfo(dir).path());
        }
    }
}

void VuraMainWindow::actionOpenMultipleFiles()
{
    QSettings settings;
    const int previousMediaCount = m_playlist->mediaCount();
    const QStringList files = QFileDialog::getOpenFileNames(
            this,
            tr("Open Media Files"),
            settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
            "All Files (*)"
        );

    for (const QString& fileName : files) {
        QUrl url = QUrl::fromLocalFile(fileName);
        if (!MediaFunctions::isPlaylist(url)) {
            m_playlist->addMedia(url);
        } else {

        }
    }

    if (m_playlist->mediaCount() > previousMediaCount) {
        const auto index = m_playlistModel->index(previousMediaCount, 0);
        ui->playlistView->setCurrentIndex(index);
        jump(index);

        QString lastFile = files.last();
        settings.setValue("lastFileDirectory", QFileInfo(lastFile).path());
    }
}

void VuraMainWindow::actionVolumeUp()
{
    float oldVolume = m_audioOutput->volume();
    if (oldVolume >= 1.0) return;
    m_audioOutput->setVolume(oldVolume + 0.1);
}

void VuraMainWindow::actionVolumeDown()
{
    float oldVolume = m_audioOutput->volume();
    if (oldVolume <= 0.1) return;
    m_audioOutput->setVolume(oldVolume - 0.1);
}

void VuraMainWindow::actionNext()
{
    m_playlist->next();
}

void VuraMainWindow::actionPrevious()
{
    if (m_player->position() <= 5000) {
        m_playlist->previous();
    } else {
        m_player->setPosition(0);
    }
}

void VuraMainWindow::actionJumpForward()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 30000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpBackward()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 30000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionEmergencyClose()
{
    m_player->pause();
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
    if (ui->videoWidget->isFullScreen()) {
        ui->videoWidget->setFullScreen(false);
        ui->videoWidget->showNormal();
    } else {
        ui->videoWidget->setFullScreen(true);
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

void VuraMainWindow::actionTogglePlaylist() const
{
    //m_playlistController->togglePlaylist();
    if (ui->playlistWidget->isVisible()) {
        ui->playlistWidget->hide();
    } else {
        ui->playlistWidget->show();
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
        m_videoControlWidget->setMuted(m_audioOutput->isMuted());
        m_videoControlWidget->setVolume(m_audioOutput->volume());
        ui->verticalLayout->addWidget(m_videoControlWidget);
        connect(m_player, &QMediaPlayer::playbackStateChanged, m_videoControlWidget, &VideoControlWidget::setState);
        connect(m_videoControlWidget, &VideoControlWidget::play, m_player, &QMediaPlayer::play);
        connect(m_videoControlWidget, &VideoControlWidget::pause, m_player, &QMediaPlayer::pause);
        connect(m_videoControlWidget, &VideoControlWidget::stop, m_player, &QMediaPlayer::stop);
        //connect(m_videoControlWidget, &VideoControlWidget::changeVolume, m_playbackController, &PlaybackController::changeVolume);
        //connect(m_videoControlWidget, &VideoControlWidget::changeMuting, m_playbackController, &PlaybackController::setMute);
        //connect(m_playbackController->getAudioOutput(), &QAudioOutput::volumeChanged, m_videoControlWidget, &VideoControlWidget::setVolume);
        //connect(m_playbackController->getAudioOutput(), &QAudioOutput::mutedChanged, m_videoControlWidget, &VideoControlWidget::setMuted);
        m_showingVideoControls = true;
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

    if (!m_player->source().isEmpty()) {
        windowTitle = QString("%1 - Vura %2").arg(MediaFunctions::strippedFileName(m_player->source().toLocalFile()), VURA_VERSION_STRING);
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

void VuraMainWindow::jump(const QModelIndex &index) const
{
    if (index.isValid())
        ui->playlistView->setCurrentIndex(index);
}

void VuraMainWindow::actionJumpForwardExtraLarge()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 90000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpBackwardExtraLarge()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 90000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpForwardLarge()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 60000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpBackwardLarge()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 60000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpForwardMedium()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 30000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpBackwardMedium()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 30000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpForwardSmall()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 15000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpBackwardSmall()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 15000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpForwardExtraSmall()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 5000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionJumpBackwardExtraSmall()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 5000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
}

void VuraMainWindow::actionTogglePlay()
{
    
}

void VuraMainWindow::actionRestart()
{
    
}

void VuraMainWindow::durationChanged(qint64 duration) {}

void VuraMainWindow::positionChanged(qint64 progress) {}

void VuraMainWindow::tracksChanged() {}

void VuraMainWindow::statusChanged(QMediaPlayer::MediaStatus status) {}

void VuraMainWindow::bufferingProgress(float progress) {}

void VuraMainWindow::displayErrorMessage() {}

void VuraMainWindow::playbackRateChanged(qreal rate) {}

void VuraMainWindow::videoFrameChanged(const QVideoFrame &frame) {}

void VuraMainWindow::seek(int mseconds) {}
