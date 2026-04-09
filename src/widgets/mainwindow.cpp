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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <config.h>
#include <qglobal.h>


#ifdef Q_OS_WIN
#include <windows.h>
#endif


// Global pointer to Logger for use in messageHandler
static Blogger* globalRedirector = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    vuraSettings = new VuraSettings();
    emit setOverrideWindowsHotkeys(vuraSettings->setOverrideWindowsHotkeys());
    initApplication();

    loadMarkersData();

    initSystemTrayIcon();
    initMenuBar();
    initStatusBar();
    initVideoControls();
    initVideoPlayer();
    initUI();
    initAudioDevices();

    if (!isPlayerAvailable())
        VMessageBox::critical(this, "Vura", "The QMediaPlayer object does not have a valid service. Please check the media service plugins are installed.");

    setToolTips();
    setStyleSheet();

    qDebug() << "Application startup complete.";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testFunction()
{
    //VuraHelpers::simulateApplicationCrash();
    VMessageBox::critical(this, "Vura", "Test of critical message box.");
}


void MainWindow::loadApplicationData()
{

}

void MainWindow::loadMarkersData()
{
    if (m_currentFile.isEmpty()) {
        qDebug() << "Current file empty. Skipping load markers data";
        return;
    }

    QString markersDataFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/global.vvm";

    markersDataList.clear();
    markersDataList = DataFileHandler::readMarkersDataFromFile(markersDataFile, m_currentFile);

    //if (!VuraHelpers::fileExists(markersDataFile)) {
        //markersDataList = DataFileHandler::readMarkersDataFromFile(markersDataFile);
        //DataFileHandler::createBlankMarkersDataFile(markersDataFile);
    //}

    //markersData.clear();
    //markersData = DataFileHandler::searchEntryInMarkersDataFile(markersDataFile, VuraHelpers::QStringToChar(m_currentFile));

    //int i = 1;
    //for (MarkersData mdata : markersData) {
    //    qDebug() << "Marker [" << QString::number(i) << "] - Type: [" << mdata.markerType << "] Position: [" << QString::number(mdata.markerTimestamp) << "]";
    //}
}


void MainWindow::saveMarkersData()
{
    if (markersDataList.count() == 0) {
        qDebug() << "Markers data list is empty. Skipping save markers data";
        return;
    }

    QString markersDataFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/global.vvm";
    DataFileHandler::writeMarkersDataToFile(markersDataFile, markersDataList);

    //if (!VuraHelpers::fileExists(markersDataFile)) {
    //    DataFileHandler::createBlankMarkersDataFile(markersDataFile);
    //}
}


#pragma region STARTUP FUNCTIONS


void MainWindow::initApplication()
{
    qDebug() << "Initializing Application...";

    this->setMouseTracking(true);
    this->statusBar()->setSizeGripEnabled(true);

    // Set the global redirector and install the custom message handler
    qInstallMessageHandler(Blogger::messageHandler);
    globalRedirector = Blogger::instance();

    mediaFunctions = new MediaFunctions();

    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }
    m_currentUser = name;

    createUserDirs();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::hideCursor);

    // Application startup initialization
    VuraStartup startup;
    startup.Initialize();

    qDebug() << "Application initialized.";
}

void MainWindow::initSystemTrayIcon()
{
    qDebug() << "Initializing system tray icon...";

    m_systemTrayIcon = new SystemTray(this);
    if (vuraSettings->systemTray()) {
        m_systemTrayIcon->show();
    } else {
        m_systemTrayIcon->hide();
    }

    connect(m_systemTrayIcon, &SystemTray::clicked, this, &MainWindow::systemTray_Clicked);
    connect(m_systemTrayIcon, &SystemTray::hiding, this, &MainWindow::systemTray_Hide);
    connect(m_systemTrayIcon, &SystemTray::stop, m_player, &QMediaPlayer::stop);
    connect(m_systemTrayIcon, &SystemTray::changePlaybackSpeed, this, &MainWindow::changePlaybackSpeed);
    connect(m_systemTrayIcon, &SystemTray::setPlaybackSpeedNormal, this, &MainWindow::setPlaybackSpeedNormal);
    connect(m_systemTrayIcon, &SystemTray::changeVolume, this, &MainWindow::changeVolume);
    connect(m_systemTrayIcon, &SystemTray::toggleMute, this, &MainWindow::toggleMute);
    connect(m_systemTrayIcon, &SystemTray::openFiles, this, &MainWindow::openFiles);
    connect(m_systemTrayIcon, &SystemTray::togglePlayPause, this, &MainWindow::togglePlayPause);
    connect(m_systemTrayIcon, &SystemTray::nextVideo, this, &MainWindow::nextVideo);
    connect(m_systemTrayIcon, &SystemTray::previousVideo, this, &MainWindow::previousVideo);
    connect(m_systemTrayIcon, &SystemTray::exit, this, &MainWindow::exitApplication);

    qDebug() << "System tray icon initialized.";
}

void MainWindow::initMenuBar()
{
    qDebug() << "Initializing menu bar...";

    m_menuBar = new MenuBar(this);
    connect(this, &MainWindow::setPlayerStatus, m_menuBar, &MenuBar::setPlayerStatus);
    connect(this, &MainWindow::refreshSettings, m_menuBar, &MenuBar::refreshSettings);
    connect(this, &MainWindow::setActiveAudioDevice, m_menuBar, &MenuBar::setActiveAudioDevice);
    connect(this, &MainWindow::setActiveAudioTrack, m_menuBar, &MenuBar::setActiveAudioTrack);
    connect(this, &MainWindow::setActiveVideoTrack, m_menuBar, &MenuBar::setActiveVideoTrack);
    connect(this, &MainWindow::setActiveSubtitleTrack, m_menuBar, &MenuBar::setActiveSubtitleTrack);
    connect(this, &MainWindow::updateAudioOutputs, m_menuBar, &MenuBar::updateAudioOutputs);
    connect(this, &MainWindow::updateAudioTracks, m_menuBar, &MenuBar::updateAudioTracks);
    connect(this, &MainWindow::updateVideoTracks, m_menuBar, &MenuBar::updateVideoTracks);
    connect(this, &MainWindow::updateSubtitleTracks, m_menuBar, &MenuBar::updateSubtitleTracks);
    connect(this, &MainWindow::updateRecentFiles, m_menuBar, &MenuBar::updateRecentFiles);

    connect(m_menuBar, &MenuBar::showMediaInformation, this, &MainWindow::showMediaInformation);
    connect(m_menuBar, &MenuBar::emergencyCollapse, this, &MainWindow::emergencyCollapse);
    connect(m_menuBar, &MenuBar::exitApplication, this, &MainWindow::exitApplication);
    connect(m_menuBar, &MenuBar::openFiles, this, &MainWindow::openFiles);
    connect(m_menuBar, &MenuBar::closeFile, this, &MainWindow::closeFile);
    connect(m_menuBar, &MenuBar::closeAllFiles, this, &MainWindow::closeAllFiles);
    connect(m_menuBar, &MenuBar::openFolder, this, &MainWindow::openFolder);
    connect(m_menuBar, &MenuBar::saveFile, this, &MainWindow::saveFile);
    connect(m_menuBar, &MenuBar::savePlaylist, this, &MainWindow::savePlaylist);
    connect(m_menuBar, &MenuBar::togglePlaylist, this, &MainWindow::togglePlaylist);
    connect(m_menuBar, &MenuBar::toggleStatusBar, this, &MainWindow::toggleStatusBar);
    connect(m_menuBar, &MenuBar::toggleMarkers, this, &MainWindow::toggleMarkers);
    connect(m_menuBar, &MenuBar::toggleVideoControls, this, &MainWindow::toggleVideoControls);
    connect(m_menuBar, &MenuBar::togglePlayPause, this, &MainWindow::togglePlayPause);
    connect(m_menuBar, &MenuBar::nextVideo, this, &MainWindow::nextVideo);
    connect(m_menuBar, &MenuBar::previousVideo, this, &MainWindow::previousVideo);
    connect(m_menuBar, &MenuBar::changePlaybackSpeed, this, &MainWindow::changePlaybackSpeed);
    connect(m_menuBar, &MenuBar::setPlaybackSpeedNormal, this, &MainWindow::setPlaybackSpeedNormal);
    connect(m_menuBar, &MenuBar::videoSeek, this, &MainWindow::videoSeek);
    connect(m_menuBar, &MenuBar::videoJumpToTime, this, &MainWindow::videoJumpToTime);
    connect(m_menuBar, &MenuBar::videoJumpToEnd, this, &MainWindow::jumpToEnd);
    connect(m_menuBar, &MenuBar::restartVideo, this, &MainWindow::restartVideo);
    connect(m_menuBar, &MenuBar::changeVolume, this, &MainWindow::changeVolume);
    connect(m_menuBar, &MenuBar::toggleMute, this, &MainWindow::toggleMute);
    connect(m_menuBar, &MenuBar::toggleFullscreen, this, &MainWindow::toggleFullscreen);
    connect(m_menuBar, &MenuBar::setAudioOutput, this, &MainWindow::setAudioOutput);
    connect(m_menuBar, &MenuBar::setAudioTrack, this, &MainWindow::setAudioTrack);
    connect(m_menuBar, &MenuBar::setVideoTrack, this, &MainWindow::setVideoTrack);
    connect(m_menuBar, &MenuBar::setSubtitleTrack, this, &MainWindow::setSubtitleTrack);
    connect(m_menuBar, &MenuBar::addMarker, this, &MainWindow::addMarker);
    connect(m_menuBar, &MenuBar::nextMarker, this, &MainWindow::nextMarker);
    connect(m_menuBar, &MenuBar::previousMarker, this, &MainWindow::previousMarker);
    connect(m_menuBar, &MenuBar::clearSelectedMarker, this, &MainWindow::clearSelectedMarker);
    connect(m_menuBar, &MenuBar::clearMarkers, this, &MainWindow::clearMarkers);
    connect(m_menuBar, &MenuBar::clearInMarker, this, &MainWindow::clearInMarker);
    connect(m_menuBar, &MenuBar::clearOutMarker, this, &MainWindow::clearOutMarker);
    connect(m_menuBar, &MenuBar::goToInMarker, this, &MainWindow::goToInMarker);
    connect(m_menuBar, &MenuBar::goToOutMarker, this, &MainWindow::goToOutMarker);
    connect(m_menuBar, &MenuBar::createSubclip, this, &MainWindow::createSubclip);
    connect(m_menuBar, &MenuBar::testFunction, this, &MainWindow::testFunction);
    connect(m_menuBar, &MenuBar::takeSnapshot, this, &MainWindow::takeSnapshot);
    connect(m_menuBar, &MenuBar::showVideoResolution, this, &MainWindow::showVideoResolution);
    connect(m_menuBar, &MenuBar::convertSave, this, &MainWindow::convertSave);
    connect(m_menuBar, &MenuBar::stream, this, &MainWindow::streamMedia);

    this->setMenuBar(m_menuBar);

    qDebug() << "Menu bar initialized.";
}

void MainWindow::initStatusBar()
{
    qDebug() << "Initializing status bar...";

    m_statusLabel = new QLabel;
    ui->statusBar->addPermanentWidget(m_statusLabel);
    ui->statusBar->setSizeGripEnabled(false);
    ui->statusBar->setVisible(vuraSettings->showStatusBarOnStart());
    m_showingStatusBar = vuraSettings->showStatusBarOnStart();
    emit setStatusBarShowing(m_showingStatusBar);

    qDebug() << "Status bar initialized.";
}

void MainWindow::initVideoControls()
{
    qDebug() << "Initializing video controls...";

    if (vuraSettings->showVideoControlsOnStart())
        toggleVideoControls();

    qDebug() << "Video controls initialized.";
}

void MainWindow::initVideoPlayer()
{
    qDebug() << "Initializing video player...";

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setVideoOutput(ui->videoWidget);
    m_videoSink = m_player->videoSink();

    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::statusChanged);
    connect(m_player, &QMediaPlayer::bufferProgressChanged, this, &MainWindow::bufferingProgress);
    connect(m_player, &QMediaPlayer::errorChanged, this, &MainWindow::displayErrorMessage);
    connect(m_player, &QMediaPlayer::sourceChanged, this, &MainWindow::sourceChanged);
    connect(m_player, &QMediaPlayer::tracksChanged, this, &MainWindow::tracksChanged);
    connect(m_player, &QMediaPlayer::playbackRateChanged, this, &MainWindow::playbackRateChanged);
    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &MainWindow::videoFrameChanged);

    qDebug() << "Video player initialized.";
}

void MainWindow::initUI()
{
    qDebug() << "Initializing UI...";

    m_videoSlider = new VideoSlider(this);
    ui->horizontalLayout_3->removeWidget(ui->placeholder);
    ui->horizontalLayout_3->insertWidget(1, m_videoSlider);
    ui->horizontalLayout_3->setStretch(1, 2);
    m_playlistModel = new PlaylistModel(this);
    m_playlist = m_playlistModel->playlist();
    ui->playlistView->setModel(m_playlistModel);
    ui->playlistView->setCurrentIndex(m_playlistModel->index(m_playlist->currentIndex(), 0));

    if (!vuraSettings->showPlaylistOnStart())
        ui->playlistView->hide();

    m_showingPlaylist = vuraSettings->showPlaylistOnStart();
    emit setPlaylistShowing(m_showingPlaylist);

    connect(m_videoSlider, &VideoSlider::sliderMoved, this, &MainWindow::seek);
    connect(m_videoSlider, &VideoSlider::sliderClicked, this, &MainWindow::seek);
    connect(m_videoSlider, &VideoSlider::markerSelected, this, &MainWindow::seek);
    connect(m_playlist, &Playlist::currentIndexChanged, this, &MainWindow::playlistPositionChanged);
    connect(ui->playlistView, &QListView::activated, this, &MainWindow::jump);
    connect(ui->playlistView, &QListView::customContextMenuRequested, this, &MainWindow::showPlaylistContextMenu);
    connect(ui->duration, &ClickableLabel::clicked, this, &MainWindow::durationLabel_Clicked);

    qDebug() << "UI Initialized.";
}

void MainWindow::initAudioDevices()
{
    qDebug() << "Initializing audio devices...";

    QList<QAudioDevice> audioDevices;
    audioDevices.append(QAudioDevice());
    for (auto &device : QMediaDevices::audioOutputs()) {
        audioDevices.append(device);
    }
    emit updateAudioOutputs(audioDevices);
    emit setActiveAudioDevice(m_audioOutput->device());

    QObject::connect(&m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this] {
        QList<QAudioDevice> audioDevices;
        audioDevices.append(QAudioDevice());
        for (auto &device : QMediaDevices::audioOutputs()) {
            audioDevices.append(device);
        }
        emit updateAudioOutputs(audioDevices);
        emit setActiveAudioDevice(m_audioOutput->device());
    });

    qDebug() << "Audio devices initialized.";
}


#pragma endregion


#pragma region CONTEXT MENUS


void MainWindow::openFolderContextMenu(const QString &path)
{
    if (m_playlist->mediaCount() > 0) {
        const QMessageBox::StandardButton confirmationBox = VMessageBox::question(
            this,
            tr("Save Playlist"),
            tr("Do you want to save your playlist before its closed?"));

        if (confirmationBox == QMessageBox::Yes) {
            // TODO: Save playlist
        }
    }
    m_playlist->clear();

    QList<QUrl> fileList;

    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        it.next();
        fileList.append(QUrl::fromLocalFile(it.filePath()));
    }

    const int previousMediaCount = m_playlist->mediaCount();
    for (auto &url : fileList) {
        if (!mediaFunctions->isPlaylist(url)) {
            m_playlist->addMedia(url);
        }
    }

    if (m_playlist->mediaCount() > previousMediaCount) {
        const auto index = m_playlistModel->index(previousMediaCount, 0);
        ui->playlistView->setCurrentIndex(index);
        jump(index);
    }
}

void MainWindow::openFileContextMenu(const QString &file)
{
    if (m_playlist->mediaCount() > 0) {
        const QMessageBox::StandardButton confirmationBox = VMessageBox::question(
            this,
            tr("Save Playlist"),
            tr("Do you want to save your playlist before its closed?"));

        if (confirmationBox == QMessageBox::Yes) {
            // TODO: Save playlist
        }
    }
    m_playlist->clear();

    const int previousMediaCount = m_playlist->mediaCount();
    if (!file.isEmpty()) {
        QUrl url = QUrl::fromLocalFile(file);
        if (!mediaFunctions->isPlaylist(url)) {
            m_playlist->addMedia(url);
            if (m_playlist->mediaCount() > previousMediaCount) {
                const auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                jump(index);
            }
        } else {
            m_playlist->loadPlaylist(file);
            if (m_playlist->mediaCount() > previousMediaCount) {
                const auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                jump(index);
            }
        }
    }
}

void MainWindow::addFileToPlaylistContextMenu(const QString &file)
{
    const int previousMediaCount = m_playlist->mediaCount();
    if (!file.isEmpty()) {
        QUrl url = QUrl::fromLocalFile(file);
        if (!mediaFunctions->isPlaylist(url)) {
            m_playlist->addMedia(url);
            if (m_playlist->mediaCount() > previousMediaCount) {
                const auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                jump(index);
            }
        }
    }
}

void MainWindow::addFolderToPlaylistContextMenu(const QString &path)
{
    QList<QUrl> fileList;

    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        it.next();
        fileList.append(QUrl::fromLocalFile(it.filePath()));
    }

    const int previousMediaCount = m_playlist->mediaCount();
    for (auto &url : fileList) {
        if (!mediaFunctions->isPlaylist(url)) {
            m_playlist->addMedia(url);
        }
    }

    if (m_playlist->mediaCount() > previousMediaCount) {
        const auto index = m_playlistModel->index(previousMediaCount, 0);
        ui->playlistView->setCurrentIndex(index);
        jump(index);
    }
}

void MainWindow::showPlaylistContextMenu(const QPoint &pos)
{
    const QPoint globalPos = ui->playlistView->mapToGlobal(pos); // Map to global position
    m_pos = pos;

    if (m_playlist->mediaCount() > 0) {
        QModelIndex index = ui->playlistView->indexAt(pos);
        if (index.isValid()) {
            QMenu videoMenu;
            QAction *playAction = videoMenu.addAction("Play");
            QAction *streamAction = videoMenu.addAction("Stream");
            QAction *saveVideoAction = videoMenu.addAction("Save");
            QAction *informationAction = videoMenu.addAction("Information");
            videoMenu.addSeparator();
            QAction *showFolderAction = videoMenu.addAction("Show Containing Folder");
            videoMenu.addSeparator();
            QAction *addFileAction = videoMenu.addAction("Add File");
            QAction *addFolderAction = videoMenu.addAction("Add Folder");
            QAction *advancedOpenAction = videoMenu.addAction("Advanced Open");
            videoMenu.addSeparator();
            QAction *saveAction = videoMenu.addAction("Save Playlist to File");
            videoMenu.addSeparator();
            QAction *removeSelectedAction = videoMenu.addAction("Remove Selected");
            QAction *clearAction = videoMenu.addAction("Clear the playlist");
            videoMenu.addSeparator();
            QAction *shuffleAction = videoMenu.addAction("Shuffle");

            connect(playAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_PlayVideoAction);
            connect(streamAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_StreamVideoAction);
            connect(saveVideoAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_SaveVideoAction);
            connect(informationAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_InformationVideoAction);
            connect(showFolderAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_ShowFolderVideoAction);
            connect(removeSelectedAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_RemoveSelectedVideoAction);
            connect(addFileAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFileAction);
            connect(addFolderAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFolderAction);
            connect(advancedOpenAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AdvancedOpenAction);
            connect(saveAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_SaveAction);
            connect(clearAction, &QAction::triggered, this, &MainWindow::clearPlaylist);
            connect(shuffleAction, &QAction::triggered, this, &MainWindow::toggleShuffle);

            videoMenu.exec(globalPos);

        } else {
            QMenu videoMenu;
            QAction *addFileAction = videoMenu.addAction("Add File");
            QAction *addFolderAction = videoMenu.addAction("Add Folder");
            QAction *advancedOpenAction = videoMenu.addAction("Advanced Open");
            videoMenu.addSeparator();
            QAction *saveAction = videoMenu.addAction("Save Playlist to File");
            videoMenu.addSeparator();
            QAction *clearAction = videoMenu.addAction("Clear the playlist");
            videoMenu.addSeparator();
            QAction *shuffleAction = videoMenu.addAction("Shuffle");

            connect(addFileAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFileAction);
            connect(addFolderAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFolderAction);
            connect(advancedOpenAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AdvancedOpenAction);
            connect(saveAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_SaveAction);
            connect(clearAction, &QAction::triggered, this, &MainWindow::clearPlaylist);
            connect(shuffleAction, &QAction::triggered, this, &MainWindow::toggleShuffle);

            videoMenu.exec(globalPos);
        }
    } else {
        QMenu emptyMenu;
        QAction *addFileAction = emptyMenu.addAction("Add File");
        QAction *addFolderAction = emptyMenu.addAction("Add Folder");
        QAction *advancedOpenAction = emptyMenu.addAction("Advanced Open");

        connect(addFileAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFileAction);
        connect(addFolderAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFolderAction);
        connect(advancedOpenAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AdvancedOpenAction);

        emptyMenu.exec(globalPos);
    }

}

void MainWindow::playlistContextMenu_AddFileAction() {}

void MainWindow::playlistContextMenu_AddFolderAction() {}

void MainWindow::playlistContextMenu_AdvancedOpenAction() {}

void MainWindow::playlistContextMenu_SaveAction() {}

void MainWindow::playlistContextMenu_PlayVideoAction()
{
    const QModelIndex index = ui->playlistView->indexAt(m_pos);
    m_playlist->setCurrentIndex(index.row());
}

void MainWindow::playlistContextMenu_StreamVideoAction() {}

void MainWindow::playlistContextMenu_SaveVideoAction() {}

void MainWindow::playlistContextMenu_InformationVideoAction() {}

void MainWindow::playlistContextMenu_ShowFolderVideoAction()
{
    const QModelIndex index = ui->playlistView->indexAt(m_pos);
    const QVariant data = index.data(); // Get the data
    const QString text = data.toString();
    const QFileInfo info(text);
    const QString directoryPath = info.absoluteDir().absolutePath();
    QUrl folderUrl = QUrl::fromLocalFile(directoryPath);
    if (!QDesktopServices::openUrl(folderUrl)) {
        const QString warningMessage = tr("Could not open folder: ") + directoryPath;
        VMessageBox::warning(this, "Vura", warningMessage);
    }
}

void MainWindow::playlistContextMenu_RemoveSelectedVideoAction()
{
    const QModelIndex index = ui->playlistView->indexAt(m_pos);
    if (index.row() == m_playlist->currentIndex()) {
        m_playlist->next();
    }
    m_playlist->removeMedia(index.row());
}

void MainWindow::systemTray_Clicked()
{
    if (this->isHidden() || this->isMinimized()) {
        this->showNormal();
        this->activateWindow();
    }
}

void MainWindow::systemTray_Hide(bool hiding)
{
    if (hiding) {
        this->hide();
        m_player->pause();

    } else {
        this->show();
        this->showNormal();
        this->raise();
        this->activateWindow();
    }
}


#pragma endregion


#pragma region MEDIA PLAYER


bool MainWindow::isPlayerAvailable() const
{
    return m_player->isAvailable();
}

void MainWindow::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    m_videoSlider->setMaximum(static_cast<int>(duration));
}

void MainWindow::positionChanged(qint64 progress)
{
    if (!m_videoSlider->isSliderDown())
        m_videoSlider->setValue(static_cast<int>(progress));

    updateDurationInfo(progress / 1000);
}

QString MainWindow::trackName(const QMediaMetaData &metaData, const int index)
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

void MainWindow::tracksChanged()
{
    emit updateAudioTracks(m_player->audioTracks());
    emit updateVideoTracks(m_player->videoTracks());
    emit updateSubtitleTracks(m_player->subtitleTracks());
}

void MainWindow::playlistPositionChanged(const int currentItem)
{
    ui->playlistView->setCurrentIndex(m_playlistModel->index(currentItem, 0));
    m_player->setSource(m_playlist->currentMedia());
}

void MainWindow::sourceChanged(const QUrl &media)
{
    if (vuraSettings->hashFile()) {
        if (!m_currentFileHash.isEmpty()) {
            // TODO: Add file hash to video marker
            //m_videoMarkers->saveMarkers(m_currentFileHash, m_videoMarkersList);
        }

        m_currentFileHash = QString::number(fileHash(media.toLocalFile()));
        //m_videoMarkersList = m_videoMarkers->getMarkers(m_currentFileHash);
    } else {
        if (!m_currentFile.isEmpty()) {
            saveMarkersData();
            //m_videoMarkers->saveMarkers(m_currentFile, m_videoMarkersList);
        }

        //loadMarkersData();
        //m_videoMarkersList = m_videoMarkers->getMarkers(media.toString());
    }
    m_currentFile = media.toString();
    qInfo() << "New media source loaded: " << m_currentFile;
    loadMarkersData();
    m_videoSlider->setMarkers(markersDataList);
    //m_videoSlider->setMarkers(m_videoMarkersList);

    setApplicationWindowTitle();

    QByteArray byteArray = m_currentFile.toUtf8();
    m_inMarker = 0;
    m_outMarker = 0;
}

void MainWindow::statusChanged(const QMediaPlayer::MediaStatus status)
{
    // handle status message
    switch (status) {
        case QMediaPlayer::NoMedia:
            emit setPlayerStatus(false);
        case QMediaPlayer::LoadedMedia:
            setStatusInfo(QString());
            updateDurationInfo(m_player->position() / 1000);
            emit setPlayerStatus(true);
            break;
        case QMediaPlayer::LoadingMedia:
            setStatusInfo(tr("Loading..."));
            break;
        case QMediaPlayer::BufferingMedia:
        case QMediaPlayer::BufferedMedia:
            setStatusInfo(tr("Buffering %1%").arg(qRound(m_player->bufferProgress() * 100.)));
            break;
        case QMediaPlayer::StalledMedia:
            setStatusInfo(tr("Stalled %1%").arg(qRound(m_player->bufferProgress() * 100.)));
            break;
        case QMediaPlayer::EndOfMedia:
            QApplication::alert(this);
            m_playlist->next();
            break;
        case QMediaPlayer::InvalidMedia:
            displayErrorMessage();
            break;
    }
}

void MainWindow::bufferingProgress(const float progress)
{
    if (m_player->mediaStatus() == QMediaPlayer::StalledMedia)
        setStatusInfo(tr("Stalled %1%").arg(qRound(progress * 100.)));
    else
        setStatusInfo(tr("Buffering %1%").arg(qRound(progress * 100.)));
}

void MainWindow::playbackRateChanged(qreal rate)
{
    ui->playbackRate->setText("x" + QString::number(m_playbackSpeed));
}

void MainWindow::videoFrameChanged(const QVideoFrame &frame)
{
    if (frame.isValid()) {
        m_videoResolution = QString::number(frame.height());
        setApplicationWindowTitle();
    }
}


#pragma endregion


#pragma region PUBLIC SLOTS


void MainWindow::showMediaInformation()
{
    if (m_mediaInformation)
        m_mediaInformation->close();

    m_mediaInformation = new MediaInformation(this);
    m_mediaInformation->show();
    m_mediaInformation->setAttribute(Qt::WA_DeleteOnClose, true);
    m_mediaInformation->setMediaInformation(m_currentFile, m_player->metaData());
}

void MainWindow::showPreferences()
{
    if (m_settingsWindow)
        m_settingsWindow->close();

    m_settingsWindow = new SettingsWindow(this);
    m_settingsWindow->show();
    m_settingsWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(m_settingsWindow, &SettingsWindow::updateSettings, this, &MainWindow::loadSettings);
}

void MainWindow::showAbout()
{
    if (m_aboutDialog)
        m_aboutDialog->close();

    m_aboutDialog = new AboutDialog(this);
    m_aboutDialog->show();
    m_aboutDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MainWindow::showHelp()
{
    if (m_helpDialog)
        m_helpDialog->close();

    m_helpDialog = new HelpDialog(this);
    m_helpDialog->show();
    m_helpDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MainWindow::showUpdates()
{
    if (m_updateDialog)
        m_updateDialog->close();

    m_updateDialog = new UpdateDialog(this);
    m_updateDialog->show();
    m_updateDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MainWindow::showFeedback()
{
    VMessageBox::information(this,
        "Vura",
        "Sorry this function has not been implemented yet.");
}

void MainWindow::emergencyCollapse()
{
    if (m_player->isPlaying()) {
        m_player->pause();
    }

    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::exitApplication()
{
    this->close();
}

void MainWindow::openFiles(const QStringList &fileList, const bool localFile)
{
    QSettings settings;

    QStringList files = settings.value("recentFileList").toStringList();

    const int previousMediaCount = m_playlist->mediaCount();
    bool loadedNewPlaylist = false;

    for (const QString& fileName : fileList) {
        if (localFile) {
            QUrl url = QUrl::fromLocalFile(fileName);

            if (!mediaFunctions->isPlaylist(url)) {
                files.removeAll(fileName);
                files.prepend(fileName);
                m_playlist->addMedia(url);

            } else {
                loadedNewPlaylist = loadPlaylist(url);
            }

        } else {
            m_playlist->addMedia(fileName);
        }
    }

    if (!loadedNewPlaylist) {
        if (m_playlist->mediaCount() > previousMediaCount) {
            const auto index = m_playlistModel->index(previousMediaCount, 0);
            ui->playlistView->setCurrentIndex(index);
            jump(index);
        }
    } else {
        // TODO: Added opened playlist code
    }

    while (files.size() > vuraSettings->maxRecentFiles()) {
        files.removeLast();
    }
    settings.setValue("recentFileList", files);
    emit updateRecentFiles();
    this->showNormal();
    this->raise();
    this->activateWindow();
}

void MainWindow::closeFile()
{
    if (m_playlist->mediaCount() == 1) {
        m_playlist->clear();
        m_player->setSource(QUrl());
    } else {
        m_playlist->removeMedia(m_playlist->currentIndex());
        m_playlist->next();
    }
}

void MainWindow::closeAllFiles()
{
    m_playlist->clear();
    m_player->setSource(QUrl());
}

void MainWindow::openFolder(const QString &folderPath)
{
    const int previousMediaCount = m_playlist->mediaCount();

    if (!folderPath.isEmpty()) {
        QList<QUrl> filesList;
        QDirIterator folderIterator(folderPath, QDir::Files | QDir::NoDotAndDotDot);
        while (folderIterator.hasNext()) {
            folderIterator.next();
            filesList.append(QUrl::fromLocalFile(folderIterator.filePath()));
        }

        for (auto &fileUrl : filesList) {
            if (!mediaFunctions->isPlaylist(fileUrl)) {
                m_playlist->addMedia(fileUrl);
            } else {
                VMessageBox::information(this, "Vura", "Playlist file in folder is being skipped.");
            }
        }

        if (m_playlist->mediaCount() > previousMediaCount) {
            const auto index = m_playlistModel->index(previousMediaCount, 0);
            ui->playlistView->setCurrentIndex(index);
            jump(index);
        }
    }
}

void MainWindow::saveFile(const QString &filePath)
{
    VMessageBox::information(this,
        "Vura",
        "Sorry this function has not been implemented yet.");
}

void MainWindow::savePlaylist(const QString &filePath, const QString &type)
{
    VMessageBox::information(this,
        "Vura",
        "Sorry this function has not been implemented yet.");
}

void MainWindow::togglePlaylist()
{
    if (m_showingPlaylist) {
        ui->playlistView->hide();
        m_showingPlaylist = false;
    } else {
        ui->playlistView->show();
        m_showingPlaylist = true;
    }
    emit setPlaylistShowing(m_showingPlaylist);
}

void MainWindow::toggleStatusBar()
{
    if (m_showingStatusBar) {
        ui->statusBar->setVisible(false);
        m_showingStatusBar = false;
    } else {
        ui->statusBar->setVisible(true);
        m_showingStatusBar = true;
    }
    emit setStatusBarShowing(m_showingStatusBar);
}

void MainWindow::toggleMarkers(const QString &markerType)
{
    if (markerType == "marker") {
        if (m_videoSlider->showMarkers) {
            m_videoSlider->showMarkers = false;
        } else {
            m_videoSlider->showMarkers = true;
        }

        emit setMarkerShowing(markerType, m_videoSlider->showMarkers);

    } else if (markerType == "cumshot") {
        if (m_videoSlider->showCumshotMarkers) {
            m_videoSlider->showCumshotMarkers = false;
        } else {
            m_videoSlider->showCumshotMarkers = true;
        }

        emit setMarkerShowing(markerType, m_videoSlider->showCumshotMarkers);

    } else if (markerType == "cyan") {
        if (m_videoSlider->showCyanMarkers) {
            m_videoSlider->showCyanMarkers = false;
        } else {
            m_videoSlider->showCyanMarkers = true;
        }

        emit setMarkerShowing(markerType, m_videoSlider->showCyanMarkers);

    } else if (markerType == "dialog") {
        if (m_videoSlider->showDialogMarkers) {
            m_videoSlider->showDialogMarkers = false;
        } else {
            m_videoSlider->showDialogMarkers = true;
        }

        emit setMarkerShowing(markerType, m_videoSlider->showDialogMarkers);

    } else if (markerType == "magenta") {
        if (m_videoSlider->showMagentaMarkers) {
            m_videoSlider->showMagentaMarkers = false;
        } else {
            m_videoSlider->showMagentaMarkers = true;
        }

        emit setMarkerShowing(markerType, m_videoSlider->showMagentaMarkers);

    } else if (markerType == "orange") {
        if (m_videoSlider->showOrangeMarkers) {
            m_videoSlider->showOrangeMarkers = false;
        } else {
            m_videoSlider->showOrangeMarkers = true;
        }

        emit setMarkerShowing(markerType, m_videoSlider->showOrangeMarkers);

    } else if (markerType == "scene") {
        if (m_videoSlider->showSceneMarkers) {
            m_videoSlider->showSceneMarkers = false;
        } else {
            m_videoSlider->showSceneMarkers = true;
        }

        emit setMarkerShowing(markerType, m_videoSlider->showSceneMarkers);

    } else if (markerType == "strip") {
        if (m_videoSlider->showStripMarkers) {
            m_videoSlider->showStripMarkers = false;
        } else {
            m_videoSlider->showStripMarkers = true;
        }

        emit setMarkerShowing(markerType, m_videoSlider->showStripMarkers);
    }
}

void MainWindow::showLogFileViewer()
{
    if (m_logViewer)
        m_logViewer->close();

    m_logViewer = new LogViewer(this);
    m_logViewer->show();
    m_logViewer->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MainWindow::toggleVideoControls()
{
    if (m_showingVideoControls) {
        ui->verticalLayout->removeWidget(m_videoControlWidget);
        delete m_videoControlWidget;
        m_showingVideoControls = false;
    } else {
        m_videoControlWidget = new VideoControlWidget(this);
        m_videoControlWidget->setMuted(m_isMuted);
        m_videoControlWidget->setVolume(m_volume);
        ui->verticalLayout->addWidget(m_videoControlWidget);
        connect(m_player, &QMediaPlayer::playbackStateChanged, m_videoControlWidget, &VideoControlWidget::setState);
        connect(m_videoControlWidget, &VideoControlWidget::play, m_player, &QMediaPlayer::play);
        connect(m_videoControlWidget, &VideoControlWidget::pause, m_player, &QMediaPlayer::pause);
        connect(m_videoControlWidget, &VideoControlWidget::stop, m_player, &QMediaPlayer::stop);
        connect(m_videoControlWidget, &VideoControlWidget::next, m_playlist, &Playlist::next);
        connect(m_videoControlWidget, &VideoControlWidget::previous, this, &MainWindow::previousVideo);
        connect(m_videoControlWidget, &VideoControlWidget::fullScreen, this, &MainWindow::toggleFullscreen);
        connect(m_videoControlWidget, &VideoControlWidget::togglePlaylist, this, &MainWindow::togglePlaylist);
        connect(m_videoControlWidget, &VideoControlWidget::setLoop, this, &MainWindow::setLoop);
        connect(m_videoControlWidget, &VideoControlWidget::shuffle, this, &MainWindow::toggleShuffle);
        connect(m_videoControlWidget, &VideoControlWidget::changeVolume, m_audioOutput, &QAudioOutput::setVolume);
        connect(m_videoControlWidget, &VideoControlWidget::changeMuting, m_audioOutput, &QAudioOutput::setMuted);
        connect(m_audioOutput, &QAudioOutput::volumeChanged, m_videoControlWidget, &VideoControlWidget::setVolume);
        connect(m_audioOutput, &QAudioOutput::mutedChanged, m_videoControlWidget, &VideoControlWidget::setMuted);
        connect(this, &MainWindow::refreshSettings, m_videoControlWidget, &VideoControlWidget::refreshUI);
        m_showingVideoControls = true;
    }
    emit setVideoControlsShowing(m_showingVideoControls);
}

void MainWindow::togglePlayPause()
{
    if (m_player->isAvailable()) {
        if (m_player->isPlaying()) {
            m_player->pause();
        } else {
            m_player->play();
        }
    }
}

void MainWindow::nextVideo()
{
    m_playlist->next();
}

void MainWindow::previousVideo()
{
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if (m_player->position() <= 5000) {
        m_playlist->previous();
    } else {
        m_player->setPosition(0);
    }
}

void MainWindow::changePlaybackSpeed(const double mrate)
{
    double newSpeed = m_playbackSpeed + mrate;
    if (newSpeed <= 0) {
        newSpeed = 0.05;

    } else if (newSpeed > 7.5) {
        newSpeed = 7.5;
    }

    m_player->setPlaybackRate(newSpeed);
    m_playbackSpeed = m_player->playbackRate();
}

void MainWindow::setPlaybackSpeedNormal()
{
    m_player->setPlaybackRate(1.0);
    m_playbackSpeed = m_player->playbackRate();
}

void MainWindow::videoSeek(const int mseconds)
{
    qint64 zeroNum = 0;
    if (m_player->isAvailable()) {
        const qint64 duration = m_player->duration();
        qint64 newPosition = m_player->position() + mseconds;

        if (newPosition < duration && newPosition > zeroNum) {
            m_player->setPosition(newPosition);

        } else if (newPosition >= duration) {
            m_player->setPosition(duration);

        } else if (newPosition <= zeroNum) {
            m_player->setPosition(zeroNum);
        }
    }
}

void MainWindow::videoJumpToTime(const int position)
{
    if (position <= m_player->duration()) {
        m_player->setPosition(position);
    } else if (position > m_player->duration()) {
        m_player->setPosition(m_player->duration());
    }
}

void MainWindow::restartVideo()
{
    m_player->setPosition(0);
}

void MainWindow::changeVolume(const double mvolume)
{
    double newVolume = m_audioOutput->volume() + mvolume;
    if (newVolume > 1.0) {
        newVolume = 1.0;
    } else if (newVolume < 0.0) {
        newVolume = 0.0;
    }

    m_audioOutput->setVolume(newVolume);
    m_volume = m_audioOutput->volume();
}

void MainWindow::toggleMute()
{
    if (m_audioOutput->isMuted()) {
        m_audioOutput->setMuted(false);
    } else {
        m_audioOutput->setMuted(true);
    }
    m_isMuted = m_audioOutput->isMuted();

    emit setMuted(m_isMuted);
}

void MainWindow::toggleFullscreen()
{
    if (!this->isMaximized() && !ui->videoWidget->isFullScreen()) {
        this->showMaximized();

    } else if (this->isMaximized() && !ui->videoWidget->isFullScreen()) {
        if (!m_fromFullscreen) {
            ui->videoWidget->setFullScreen(true);
            ui->videoWidget->setCursor(QCursor(Qt::BlankCursor));
        } else {
            this->showNormal();
            m_fromFullscreen = false;
        }
    } else if (ui->videoWidget->isFullScreen()) {
        ui->videoWidget->setFullScreen(false);
        m_fromFullscreen = true;
        ui->videoWidget->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void MainWindow::setAudioOutput(const QAudioDevice &moutput)
{
    m_player->audioOutput()->setDevice(moutput);
}

void MainWindow::setAudioTrack(const int mtrack)
{
    m_player->setActiveAudioTrack(mtrack);
    emit setActiveAudioTrack(mtrack);
}

void MainWindow::setVideoTrack(const int mtrack)
{
    m_player->setActiveVideoTrack(mtrack);
    emit setActiveVideoTrack(mtrack);
}

void MainWindow::setSubtitleTrack(const int mtrack)
{
    m_player->setActiveSubtitleTrack(mtrack);
    emit setActiveSubtitleTrack(mtrack);
}

void MainWindow::addMarker(const QString &markerType)
{
    if (markerType == "in") {
        m_inMarker = m_player->position();

    } else if (markerType == "out") {
        m_outMarker = m_player->position();

    } else {
        const double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
        const double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
        const double sliderPercent = (distanceFromMin / sliderRange);

        MarkersData newData;
        newData.fileName = m_currentFile;
        newData.markerType = markerType;
        newData.markerTimestamp = sliderPercent;
        markersDataList.append(newData);

        m_videoSlider->setMarkers(markersDataList);
    }
}

void MainWindow::nextMarker()
{
    const double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    const double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    const double sliderPercent = (distanceFromMin / sliderRange);
    m_videoSlider->jumpToNextMarker(sliderPercent);
}

void MainWindow::previousMarker()
{
    const double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    const double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    const double sliderPercent = (distanceFromMin / sliderRange);
    m_videoSlider->jumpToPreviousMarker(sliderPercent);
}

void MainWindow::clearSelectedMarker()
{
    VMessageBox::information(this,
        "Vura",
        "Sorry this function has not been implemented yet.");
}

void MainWindow::clearMarkers()
{
    markersDataList.clear();
    m_videoSlider->setMarkers(markersDataList);
}

void MainWindow::clearInMarker()
{
    m_inMarker = 0;
}

void MainWindow::clearOutMarker()
{
    m_outMarker = 0;
}

void MainWindow::goToInMarker()
{
    if (m_inMarker > 0)
        m_player->setPosition(m_inMarker);
}

void MainWindow::goToOutMarker()
{
    if (m_outMarker > 0)
        m_player->setPosition(m_outMarker);
}

void MainWindow::createSubclip()
{
    if (m_inMarker == 0 || m_outMarker == 0) {
        VMessageBox::warning(this, "Create Subclip", "In or out marker is empty.");
    } else {
        mediaFunctions->extractSubclipFromVideo(m_currentFile, m_inMarker, m_outMarker);
    }
}

void MainWindow::setLoop(const int loopOption)
{
    if (loopOption == 1) {
        m_playlistLoopAll = true;
        m_playlistLoopOne = false;
        m_playlistLoopNone = false;
        m_playlist->setPlaybackMode(Playlist::Loop);

    } else if (loopOption == 2) {
        m_playlistLoopAll = false;
        m_playlistLoopOne = true;
        m_playlistLoopNone = false;
        m_playlist->setPlaybackMode(Playlist::CurrentItemInLoop);

    } else {
        m_playlistLoopAll = false;
        m_playlistLoopOne = false;
        m_playlistLoopNone = true;
        m_playlist->setPlaybackMode(Playlist::Sequential);
    }
}

void MainWindow::toggleShuffle()
{
    m_playlist->shuffle();
}

void MainWindow::clearPlaylist()
{
    m_playlist->clear();
}

void MainWindow::takeSnapshot()
{
    mediaFunctions->takeSnapshot(m_currentFile, static_cast<int>(m_player->position()), m_videoSink->videoFrame());
}

void MainWindow::jumpToEnd()
{
    if (m_player->isAvailable()) {
        const qint64 duration = m_player->duration();
        const qint64 subtractDuration = duration * vuraSettings->jumpToEndPercentage();
        qint64 newPosition = duration - subtractDuration;

        if (newPosition > 0 && newPosition < duration) {
            m_player->setPosition(newPosition);
        }
    }
}

void MainWindow::showVideoResolution(const bool showing)
{
    m_showingVideoResolution = showing;
}

void MainWindow::convertSave()
{
    if (m_convertMediaDialog)
        m_convertMediaDialog->close();

    m_convertMediaDialog = new ConvertMediaDialog(this, "Convert/Save");
    m_convertMediaDialog->show();
    m_convertMediaDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MainWindow::streamMedia()
{
    if (m_convertMediaDialog)
        m_convertMediaDialog->close();

    m_convertMediaDialog = new ConvertMediaDialog(this, "Stream");
    m_convertMediaDialog->show();
    m_convertMediaDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MainWindow::continuePlaybackRibbon(const bool con)
{
    if (con) {
        //m_player->setPosition(currentVideoData.position);
        ui->verticalLayout->removeWidget(m_continuePlaybackRibbon);
        delete m_continuePlaybackRibbon;

    } else {
        ui->verticalLayout->removeWidget(m_continuePlaybackRibbon);
        delete m_continuePlaybackRibbon;
    }
}


#pragma endregion


#pragma region VIDEO CONTROLS


void MainWindow::seek(const int mseconds)
{
    m_player->setPosition(mseconds);
}

void MainWindow::hideCursor()
{
    ui->videoWidget->setCursor(QCursor(Qt::BlankCursor));
    timer->stop();
}

void MainWindow::jump(const QModelIndex &index)
{
    if (index.isValid())
        m_playlist->setCurrentIndex(index.row());
}

void MainWindow::jumpTo(const int mseconds)
{
    qint64 zeroNum = 0;
    if (m_player->isAvailable()) {
        const qint64 duration = m_player->duration();
        qint64 newPosition = m_player->position() + mseconds;

        if (newPosition < duration && newPosition > zeroNum) {
            m_player->setPosition(newPosition);

        } else if (newPosition >= duration) {
            m_player->setPosition(duration);

        } else if (newPosition <= zeroNum) {
            m_player->setPosition(zeroNum);
        }
    }
}

void MainWindow::durationLabel_Clicked()
{
    if (m_durationLabelShowRemainingTime) {
        m_durationLabelShowRemainingTime = false;

    } else {
        m_durationLabelShowRemainingTime = true;
    }

    if (m_lastPosition > 0)
        updateDurationInfo(m_lastPosition);
}


#pragma endregion


#pragma region CORE APPLICATION FUNCTIONS


void MainWindow::loadSettings()
{
    vuraSettings->loadSettings();

    if (timer->isActive())
        timer->stop();

    emit refreshSettings();
    emit setOverrideWindowsHotkeys(vuraSettings->setOverrideWindowsHotkeys());
}

void MainWindow::setMainWindowVisibility(const bool state)
{
    if (state) {
        this->show();
        this->showNormal();
        this->raise();
        this->activateWindow();
    }
}

void MainWindow::processOpenParams(int argc, char *argv[])
{
    if (argc > 2) {
        const QString pathParam = QString::fromUtf8(argv[2]);

        QFileInfo pathParamInfo(pathParam);
        if (pathParamInfo.isFile()) {

            if (QString::fromLocal8Bit(argv[1]) == "playlist") {
                addFileToPlaylistContextMenu(pathParam);

            } else {
                addFileToPlaylistContextMenu(pathParam);
            }

        } else if (pathParamInfo.isDir()) {
            if (QString::fromLocal8Bit(argv[1]) == "playlist") {
                addFolderToPlaylistContextMenu(pathParam);

            } else {
                addFolderToPlaylistContextMenu(pathParam);
            }
        }

    } else if (argc > 1) {
        QString pathName = QString::fromUtf8(argv[1]);
        if (pathName.isEmpty()) {
            VMessageBox::critical(this, "Vura Error", "File requested is empty.");
            this->close();

        } else {
            openFileContextMenu(pathName);
        }
    }
}

void MainWindow::setTrackInfo(const QString &info)
{
    m_trackInfo = info;

    if (m_statusBar) {
        m_statusBar->showMessage(m_trackInfo);
        m_statusLabel->setText(m_statusInfo);
    } else {
        if (!m_statusInfo.isEmpty())
            setWindowTitle(QStringLiteral("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
        else
            setWindowTitle("Vura - " + m_trackInfo);
    }
}

void MainWindow::setStatusInfo(const QString &info)
{
    m_statusInfo = info;
    ui->statusBar->showMessage(m_trackInfo);
    m_statusLabel->setText(m_statusInfo);
}

void MainWindow::updateDurationInfo(const qint64 currentInfo)
{
    QString durationString;
    QString positionString;

    if (currentInfo || m_duration) {
        m_lastPosition = currentInfo;
        const int currentPosition = static_cast<int>(currentInfo);
        const int currentDuration = static_cast<int>(m_duration);

        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";

        const QTime currentTime(
            (currentPosition / 3600) % 60,
            (currentPosition / 60) % 60,
            currentPosition % 60,
            (currentPosition * 1000) % 1000);

        positionString = currentTime.toString(format);

        if (vuraSettings->durationLabelShowRemainingTime()) {
            const int remainingInfo = currentDuration - currentPosition;

            const QTime remainingTime(
                (remainingInfo / 3600) % 60,
                (remainingInfo / 60) % 60,
                remainingInfo % 60,
                (remainingInfo * 1000) % 1000);

            durationString = remainingTime.toString(format);

        } else {
            const QTime totalTime(
                (currentDuration / 3600) % 60,
                (currentDuration / 60) % 60,
                currentDuration % 60,
                (currentDuration * 1000) % 1000);

            durationString = totalTime.toString(format);
        }

    } else {
        positionString = "--:--";
        durationString = "--:--";
    }

    ui->duration->setText(durationString);
    ui->position->setText(positionString);
    ui->playbackRate->setText("x" + QString::number(m_playbackSpeed));

    if (currentInfo > 0) {
        m_videoSlider->setVideoLoaded(true);
    }
}

void MainWindow::displayErrorMessage()
{
    if (m_player->error() == QMediaPlayer::NoError)
        return;

    setStatusInfo(m_player->errorString());
    VMessageBox::warning(this, "Vura", m_player->errorString());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (vuraSettings->hashFile()) {
        //if (!m_currentFileHash.isEmpty())
            //m_videoMarkers->saveMarkers(m_currentFileHash, m_videoMarkersList);
    } else {
        //if (!m_currentFile.isEmpty())
            //m_videoMarkers->saveMarkers(m_currentFile, m_videoMarkersList);
    }
    saveMarkersData();

    event->accept();
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
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

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (this->isMinimized()) {
            if (vuraSettings->pausePlaybackWhenMinimized())
                m_player->pause();
        }
    }
    QMainWindow::changeEvent(event);
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::HoverMove:
            if (vuraSettings->hideCursorWhenPlaying()) {
                if (ui->videoWidget->rect().contains(QCursor::pos())) {
                    if (m_player->isPlaying()) {
                        timer->stop();
                        ui->videoWidget->setCursor(QCursor(Qt::ArrowCursor));
                        timer->start(vuraSettings->hideCursorTime());

                    } else {
                        timer->stop();
                        ui->videoWidget->setCursor(QCursor(Qt::ArrowCursor));
                    }

                } else {
                    timer->stop();
                    ui->videoWidget->setCursor(QCursor(Qt::ArrowCursor));
                }
            } else {
                timer->stop();
                ui->videoWidget->setCursor(QCursor(Qt::ArrowCursor));
            }

        case QEvent::HoverEnter:
            if (m_player->isPlaying())
                timer->start(vuraSettings->hideCursorTime());

        default:
            return QMainWindow::event(event);
    }
}


#pragma endregion


#pragma region FUNCTIONS


bool MainWindow::loadPlaylist(const QUrl &url)
{
    if (m_playlistLoaded) {
        const QMessageBox::StandardButton confirmationBox = QMessageBox::question(
            this,
            "Close Playlist",
            "Are you sure you want to close the current playlist and load the selected one?",
            QMessageBox::Yes | QMessageBox::No);

        if (confirmationBox == QMessageBox::Yes) {
            m_playlist->load(url);
            return true;
        }
    } else {
        m_playlist->load(url);
        m_playlistLoaded = true;
        return true;
    }
    return false;
}

void MainWindow::loadFile(const QString &fileName)
{
    const int previousMediaCount = m_playlist->mediaCount();
    m_playlist->addMedia(fileName);

    if (m_playlist->mediaCount() > previousMediaCount) {
        const auto index = m_playlistModel->index(previousMediaCount, 0);
        ui->playlistView->setCurrentIndex(index);
        jump(index);
    }
}

void MainWindow::setApplicationWindowTitle()
{
    QString windowTitle;

    if (!m_player->source().isEmpty()) {
        if (m_showingVideoResolution) {
            if (m_videoResolution.isEmpty()) {
                windowTitle = QString("%1 [%2] - Vura %3").arg(mediaFunctions->strippedFileName(m_currentFile), "UNKNOWN RES", VURA_VERSION_STRING);
            } else {
                windowTitle = QString("%1 [%2p] - Vura %3").arg(mediaFunctions->strippedFileName(m_currentFile), m_videoResolution, VURA_VERSION_STRING);
            }
        } else {
            windowTitle = QString("%1 - Vura %2").arg(mediaFunctions->strippedFileName(m_currentFile), VURA_VERSION_STRING);
        }
        m_systemTrayIcon->setToolTip(mediaFunctions->strippedFileName(m_currentFile));
        m_sourceLoaded = false;

    } else {
        windowTitle = QString("Vura %1").arg(VURA_VERSION_STRING);
        m_systemTrayIcon->setToolTip("Vura media player");
        m_sourceLoaded = true;
    }

    setWindowTitle(windowTitle);
}

void MainWindow::setToolTips()
{
    ui->position->setToolTip(tr("Elapsed time"));
    ui->duration->setToolTip(tr("Total/Remaining time\n -Click to toggle between total and remaining time"));
    ui->playbackRate->setToolTip(tr("Playback speed"));
}

void MainWindow::setStyleSheet()
{
    QFile file(":/styles/dark.qss");
    if (file.open(QFile::ReadOnly)) {
        const QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
    }
}

bool MainWindow::createUserDirs()
{
    // User Directories
    QStringList directoryList;
    const QString documentsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Vura";
    directoryList << documentsDir;
    directoryList << documentsDir + "/Clips";
    directoryList << documentsDir + "/Screenshots";
    directoryList << documentsDir + "/Vura Auto-Save";
    directoryList << documentsDir + "/Profile-" + m_currentUser;

    foreach (QString directory, directoryList)
    {
        if (!QDir(directory).exists()) {
            qDebug() << "Directory " << directory << " does not exist. Creating...";
            if (QDir().mkpath(directory)) {
                qDebug() << "Created directory: " << directory;
            } else {
                qWarning() << "Could not create directory: " << directory;
                return false;
            }
        }
    }

    return true;
}

qint64 MainWindow::fileHash(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return 0;

    XXH64_state_t* state = XXH64_createState();
    XXH64_reset(state, 0); // Seed 0

    char buffer[8192];
    qint64 bytesRead;
    while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0) {
        XXH64_update(state, buffer, bytesRead);
    }

    XXH64_hash_t hash = XXH64_digest(state);
    XXH64_freeState(state);
    file.close();
    return hash;
}


#pragma endregion
