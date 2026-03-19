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
#include "../constants.h"

#include <config.h>
#include <qglobal.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

// Global pointer to Logger for use in messageHandler
static Logger* globalRedirector = nullptr;


/*!
    @class MainWindow

    @brief The main window of the Vura application.

 */

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setMouseTracking(true);
    this->statusBar()->setSizeGripEnabled(true);

    // Set the global redirector and install the custom message handler
    qInstallMessageHandler(Logger::messageHandler);
    globalRedirector = Logger::instance();
    qInfo() << "Starting application...";

    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }
    m_currentUser = name;

    createUserDirs();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::hideCursor);

    // Application startup initialization
    Startup startup;
    startup.Initialize();

    loadSettings();
    m_videoMarkers = new VideoMarkers;


    // Configure system tray icon
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/img/images/vura.png"));
    systemTray_ToggleShow = new QAction(this);
    if (m_systemTray_Showing) {
        systemTray_ToggleShow->setText(tr("Hide Vura in taskbar"));
    } else {
        systemTray_ToggleShow->setText(tr("Show Vura"));
    }

    QMenu *menu = new QMenu(this);
    menu->addAction(systemTray_ToggleShow);
    menu->addSeparator();
    QAction *playAction = menu->addAction(tr("Play"));
    QAction *stopAction = menu->addAction(tr("Stop"));
    QAction *nextAction = menu->addAction(tr("Next"));
    QAction *previousAction = menu->addAction(tr("Previous"));
    QAction *recordAction = menu->addAction(tr("Record"));
    menu->addSeparator();

    QMenu *speedMenu = menu->addMenu(tr("Speed"));
    QAction *fasterAction = speedMenu->addAction(tr("Faster"));
    QAction *fasterFineAction = speedMenu->addAction(tr("Faster (fine)"));
    QAction *normalAction = speedMenu->addAction(tr("Normal"));
    QAction *slowerFineAction = speedMenu->addAction(tr("Slower (fine)"));
    QAction *slowerAction = speedMenu->addAction(tr("Slower"));

    menu->addSeparator();
    QAction *increaseVolumeAction = menu->addAction(tr("Increase Volume"));
    QAction *decreaseVolumeAction = menu->addAction(tr("Decrease Volume"));
    QAction *muteAction = menu->addAction(tr("Mute"));
    menu->addSeparator();
    QAction *openFileAction = menu->addAction(tr("Open File"));
    QAction *quitAction = menu->addAction(tr("Quit"));

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::systemTray_Clicked);
    connect(systemTray_ToggleShow, &QAction::triggered, this, &MainWindow::hideSystemTray);
    connect(stopAction, &QAction::triggered, this, &MainWindow::systemTray_Stop);
    connect(recordAction, &QAction::triggered, this, &MainWindow::systemTray_Record);
    connect(fasterAction, &QAction::triggered, this, &MainWindow::systemTray_Faster);
    connect(fasterFineAction, &QAction::triggered, this, &MainWindow::systemTray_FasterFine);
    connect(normalAction, &QAction::triggered, this, &MainWindow::systemTray_Normal);
    connect(slowerFineAction, &QAction::triggered, this, &MainWindow::systemTray_SlowerFine);
    connect(slowerAction, &QAction::triggered, this, &MainWindow::systemTray_Slower);
    connect(increaseVolumeAction, &QAction::triggered, this, &MainWindow::systemTray_IncreaseVolume);
    connect(decreaseVolumeAction, &QAction::triggered, this, &MainWindow::systemTray_DecreaseVolume);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::systemTray_OpenFile);
    connect(quitAction, &QAction::triggered, this, &MainWindow::exitApplication);
    connect(playAction, &QAction::triggered, this, &MainWindow::togglePlayPause);
    connect(nextAction, &QAction::triggered, this, &MainWindow::nextVideo);
    connect(previousAction, &QAction::triggered, this, &MainWindow::previousVideo);
    connect(muteAction, &QAction::triggered, this, &MainWindow::toggleMute);

    m_trayIcon->setContextMenu(menu);
    m_trayIcon->setToolTip(tr("Vura media player"));

    if (m_systemTray)
        m_trayIcon->show();


    // Configure menu bar
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
    // connect(this, &MainWindow::, m_menuBar, &MenuBar::);

    connect(m_menuBar, &MenuBar::showPreferences, this, &MainWindow::showPreferences);
    connect(m_menuBar, &MenuBar::showAbout, this, &MainWindow::showAbout);
    connect(m_menuBar, &MenuBar::showHelp, this, &MainWindow::showHelp);
    connect(m_menuBar, &MenuBar::showUpdates, this, &MainWindow::showUpdates);
    connect(m_menuBar, &MenuBar::showFeedback, this, &MainWindow::showFeedback);
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
    connect(m_menuBar, &MenuBar::toggleCumshotMarkers, this, &MainWindow::toggleCumshotMarkers);
    connect(m_menuBar, &MenuBar::toggleCyanMarkers, this, &MainWindow::toggleCyanMarkers);
    connect(m_menuBar, &MenuBar::toggleDialogMarkers, this, &MainWindow::toggleDialogMarkers);
    connect(m_menuBar, &MenuBar::toggleMagentaMarkers, this, &MainWindow::toggleMagentaMarkers);
    connect(m_menuBar, &MenuBar::toggleOrangeMarkers, this, &MainWindow::toggleOrangeMarkers);
    connect(m_menuBar, &MenuBar::toggleSceneTransitionMarkers, this, &MainWindow::toggleSceneTransitionMarkers);
    connect(m_menuBar, &MenuBar::toggleStripMarkers, this, &MainWindow::toggleStripMarkers);
    connect(m_menuBar, &MenuBar::showLogFileViewer, this, &MainWindow::showLogFileViewer);
    connect(m_menuBar, &MenuBar::toggleVideoControls, this, &MainWindow::toggleVideoControls);
    connect(m_menuBar, &MenuBar::togglePlayPause, this, &MainWindow::togglePlayPause);
    connect(m_menuBar, &MenuBar::nextVideo, this, &MainWindow::nextVideo);
    connect(m_menuBar, &MenuBar::previousVideo, this, &MainWindow::previousVideo);
    connect(m_menuBar, &MenuBar::changePlaybackSpeed, this, &MainWindow::changePlaybackSpeed);
    connect(m_menuBar, &MenuBar::setPlaybackSpeedNormal, this, &MainWindow::setPlaybackSpeedNormal);
    connect(m_menuBar, &MenuBar::videoSeek, this, &MainWindow::videoSeek);
    connect(m_menuBar, &MenuBar::videoJumpToTime, this, &MainWindow::videoJumpToTime);
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
    // connect(m_menuBar, &MenuBar::, this, &MainWindow::);

    this->setMenuBar(m_menuBar);


    // Configure status bar
    m_statusLabel = new QLabel;
    ui->statusBar->addPermanentWidget(m_statusLabel);
    ui->statusBar->setSizeGripEnabled(false);
    ui->statusBar->setVisible(m_showStatusBarOnStart);
    m_showingStatusBar = m_showStatusBarOnStart;
    emit setStatusBarShowing(m_showingStatusBar);


    // Configure Video Controls
    if (m_showVideoControlsOnStart)
        toggleVideoControls();


    // Configure Video Player
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


    // Configure UI items
    m_videoSlider = new VideoSlider(this);
    ui->horizontalLayout_3->removeWidget(ui->placeholder);
    ui->horizontalLayout_3->insertWidget(1, m_videoSlider);
    ui->horizontalLayout_3->setStretch(1, 2);
    m_playlistModel = new PlaylistModel(this);
    m_playlist = m_playlistModel->playlist();
    ui->playlistView->setModel(m_playlistModel);
    ui->playlistView->setCurrentIndex(m_playlistModel->index(m_playlist->currentIndex(), 0));

    if (!m_showPlaylistOnStart)
        ui->playlistView->hide();
    m_showingPlaylist = m_showPlaylistOnStart;
    emit setPlaylistShowing(m_showingPlaylist);

    connect(m_videoSlider, &VideoSlider::sliderMoved, this, &MainWindow::seek);
    connect(m_videoSlider, &VideoSlider::sliderClicked, this, &MainWindow::seek);
    connect(m_videoSlider, &VideoSlider::markerSelected, this, &MainWindow::seek);
    connect(m_playlist, &Playlist::currentIndexChanged, this, &MainWindow::playlistPositionChanged);
    connect(ui->playlistView, &QListView::activated, this, &MainWindow::jump);
    connect(ui->playlistView, &QListView::customContextMenuRequested, this, &MainWindow::showPlaylistContextMenu);
    connect(ui->duration, &ClickableLabel::clicked, this, &MainWindow::durationLabel_Clicked);


    //
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

    if (!isPlayerAvailable()) {
        qWarning() << "The QMediaPlayer object does not have a valid service. Please check the media service plugins are installed.";
    }

    setToolTips();
    setStyleSheet();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openedWithFile(QString file)
{
    const int previousMediaCount = m_playlist->mediaCount();
    qDebug() << "File " << file << " opened with application.";
    if (!file.isEmpty()) {
        QUrl url = QUrl::fromLocalFile(file);
        if (!isPlaylist(url)) {
            m_playlist->addMedia(url);
            if (m_playlist->mediaCount() > previousMediaCount) {
                auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                jump(index);
            }
        } else {
            m_playlist->loadPlaylist(file);
            if (m_playlist->mediaCount() > previousMediaCount) {
                auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                jump(index);
            }
        }
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;

    m_locale = settings.value("language", "en-US").toString();
    m_showStatusBarOnStart = settings.value("showStatusBarOnStart", false).toBool();
    m_showPlaylistOnStart = settings.value("showPlaylistOnStart", false).toBool();
    m_showVideoControlsOnStart = settings.value("showVideoControlsOnStart", false).toBool();
    m_hashFile = settings.value("hashFile", false).toBool();
    m_jumpSmall = settings.value("jumpSmall", 5).toInt();
    m_jumpMedium = settings.value("jumpMedium", 15).toInt();
    m_jumpLarge = settings.value("jumpLarge", 30).toInt();
    m_jumpExtraLarge = settings.value("jumpExtraLarge", 90).toInt();
    m_maxRecentFiles = settings.value("maxRecentFiles", 9).toInt();
    m_hideCursorWhenPlaying = settings.value("hideCursorWhenPlaying", true).toBool();
    m_hideCursorTime = settings.value("hideCursorTime", 2000).toInt();
    m_systemTray = settings.value("systemTray", true).toBool();
    m_mediaChangeNotification = settings.value("mediaChangeNotification", 1).toInt();
    m_showVideoControlsWhenFullscreen = settings.value("showVideoControlsWhenFullscreen", false).toBool();
    m_startInMinimalViewMode = settings.value("startInMinimalViewMode", false).toBool();
    m_pausePlaybackWhenMinimized = settings.value("pausePlaybackWhenMinimized", false).toBool();
    m_allowOnlyOneInstance = settings.value("allowOnlyOneInstance", false).toBool();
    m_oneInstanceFromFileManager = settings.value("oneInstanceFromFileManager", true).toBool();
    m_continuePlayback = settings.value("continuePlayback", 1).toInt();
    m_pauseOnLastFrameOfVideo = settings.value("pauseOnLastFrameOfVideo", false).toBool();
    m_playbackSpeedAdjustment = settings.value("playbackSpeedAdjustment", 0.5).toDouble();
    m_playbackSpeedFineAdjustment = settings.value("playbackSpeedFineAdjustment", 0.25).toDouble();
    m_volumeStep = settings.value("volumeStep", 0.10).toDouble();
    m_theme = settings.value("theme", "System").toString();
    m_setOverrideWindowsHotkeys = settings.value("setOverrideWindowsHotkeys", true).toBool();

    if (timer->isActive())
        timer->stop();

    emit refreshSettings();
    emit setOverrideWindowsHotkeys(m_setOverrideWindowsHotkeys);
    //setSystemTrayIcon();
}

void MainWindow::testFunction() {}


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

// TODO: Implement
void MainWindow::metaDataChanged()
{
    /*
    auto metaData = m_player->metaData();
    setTrackInfo(QStringLiteral("%1 - %2")
                         .arg(metaData.value(QMediaMetaData::AlbumArtist).toString())
                         .arg(metaData.value(QMediaMetaData::Title).toString()));

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    for (int i = 0; i < QMediaMetaData::NumMetaData; i++) {
        if (QLineEdit *field = qobject_cast<QLineEdit *>(m_metaDataFields[i]))
            field->clear();
        else if (QLabel *label = qobject_cast<QLabel *>(m_metaDataFields[i]))
            label->clear();
        m_metaDataFields[i]->setDisabled(true);
        m_metaDataLabels[i]->setDisabled(true);
    }

    for (auto &&[key, value] : metaData.asKeyValueRange()) {
        int i = int(key);
        if (key == QMediaMetaData::CoverArtImage) {
            if (QLabel *cover = qobject_cast<QLabel *>(m_metaDataFields[key])) {
                QImage coverImage = value.value<QImage>();
                cover->setPixmap(QPixmap::fromImage(coverImage));
            }
        } else if (key == QMediaMetaData::ThumbnailImage) {
            if (QLabel *thumbnail = qobject_cast<QLabel *>(m_metaDataFields[key])) {
                QImage thumbnailImage = value.value<QImage>();
                thumbnail->setPixmap(QPixmap::fromImage(thumbnailImage));
            }
        } else if (QLineEdit *field = qobject_cast<QLineEdit *>(m_metaDataFields[key])) {
            QString stringValue = metaData.stringValue(key);
            field->setText(stringValue);
        }
        m_metaDataFields[i]->setDisabled(false);
        m_metaDataLabels[i]->setDisabled(false);
    }

    const QList<QMediaMetaData> tracks = m_player->videoTracks();
    const int currentVideoTrack = m_player->activeVideoTrack();
    if (currentVideoTrack >= 0 && currentVideoTrack < tracks.size()) {
        const QMediaMetaData track = tracks.value(currentVideoTrack);
        for (const QMediaMetaData::Key &key : track.keys()) {
            if (QLineEdit *field = qobject_cast<QLineEdit *>(m_metaDataFields[key])) {
                QString stringValue = track.stringValue(key);
                field->setText(stringValue);
            }
            m_metaDataFields[key]->setDisabled(true);
            m_metaDataLabels[key]->setDisabled(true);
        }
    }
#endif
*/
}

QString MainWindow::trackName(const QMediaMetaData &metaData, int index)
{
    QString name;
    QString title = metaData.stringValue(QMediaMetaData::Title);
    QLocale::Language lang = metaData.value(QMediaMetaData::Language).value<QLocale::Language>();

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
    //updateAudioTrackActions();
    //updateVideoTrackActions();
    //updateSubtitleTrackActions();
    emit updateAudioTracks(m_player->audioTracks());
    emit updateVideoTracks(m_player->videoTracks());
    emit updateSubtitleTracks(m_player->subtitleTracks());
}

void MainWindow::playlistPositionChanged(int currentItem)
{
    ui->playlistView->setCurrentIndex(m_playlistModel->index(currentItem, 0));
    m_player->setSource(m_playlist->currentMedia());
}

void MainWindow::sourceChanged(const QUrl &media)
{
    if (m_hashFile) {
        if (!m_currentFileHash.isEmpty())
            m_videoMarkers->saveMarkers(m_currentFileHash, m_videoMarkersList);

        m_currentFileHash = QString::number(fileHash(media.toLocalFile()));
        m_videoMarkersList = m_videoMarkers->getMarkers(m_currentFileHash);
    } else {
        if (!m_currentFile.isEmpty())
            m_videoMarkers->saveMarkers(m_currentFile, m_videoMarkersList);

        m_videoMarkersList = m_videoMarkers->getMarkers(media.toString());
    }
    m_currentFile = media.toString();
    qInfo() << "New media source loaded: " << m_currentFile;
    m_videoSlider->setMarkers(m_videoMarkersList);

    setApplicationWindowTitle();

    QByteArray byteArray = m_currentFile.toUtf8();
    if (m_playing) {
        m_player->play();
    } else {
        m_player->pause();
    }
    m_inMarker = 0;
    m_outMarker = 0;
}

void MainWindow::statusChanged(QMediaPlayer::MediaStatus status)
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

void MainWindow::bufferingProgress(float progress)
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


#pragma endregion


#pragma region PUBLIC SLOTS


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

void MainWindow::showHelp() {}

void MainWindow::showUpdates()
{
    if (m_updateDialog)
        m_updateDialog->close();

    m_updateDialog = new UpdateDialog(this);
    m_updateDialog->show();
    m_updateDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MainWindow::showFeedback() {}

void MainWindow::emergencyCollapse()
{
    if (m_player->isPlaying()) {
        m_player->pause();
    }

    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::exitApplication()
{
    QMessageBox::StandardButton confirmationBox;
    confirmationBox = QMessageBox::question(this, "Exit Application", "Are you sure you want to exit?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (confirmationBox == QMessageBox::Yes) {
        qInfo() << "Exitting Application";
        this->close();
    }
}

void MainWindow::openFiles(const QStringList &fileList)
{
    QSettings settings;

    QStringList files = settings.value("recentFileList").toStringList();

    const int previousMediaCount = m_playlist->mediaCount();
    bool loadedNewPlaylist = false;

    for (const QString& fileName : fileList) {
        files.removeAll(fileName);
        files.prepend(fileName);

        QUrl url = QUrl::fromLocalFile(fileName);
        if (!isPlaylist(url)) {
            m_playlist->addMedia(url);
        } else {
            loadedNewPlaylist = loadPlaylist(url);
        }
    }

    if (!loadedNewPlaylist) {
        if (m_playlist->mediaCount() > previousMediaCount) {
            auto index = m_playlistModel->index(previousMediaCount, 0);
            ui->playlistView->setCurrentIndex(index);
            jump(index);
        }
    } else {
        // Added opened playlist code
    }

    while (files.size() > m_maxRecentFiles) {
        files.removeLast();
    }
    settings.setValue("recentFileList", files);
    emit updateRecentFiles();
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
            if (!isPlaylist(fileUrl)) {
                m_playlist->addMedia(fileUrl);
            } else {
                VuraMessageBox::information(this, "Vura", "Playlist file in folder is being skipped.");
            }
        }

        if (m_playlist->mediaCount() > previousMediaCount) {
            auto index = m_playlistModel->index(previousMediaCount, 0);
            ui->playlistView->setCurrentIndex(index);
            jump(index);
        }
    }
}

void MainWindow::saveFile(const QString &filePath) {}

void MainWindow::savePlaylist(const QString &filePath, const QString &type) {}

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

void MainWindow::toggleMarkers()
{
    if (m_videoSlider->showMarkers) {
        m_videoSlider->showMarkers = false;
    } else {
        m_videoSlider->showMarkers = true;
    }
    emit setMarkerShowing("marker", m_videoSlider->showMarkers);
}

void MainWindow::toggleCumshotMarkers()
{
    if (m_videoSlider->showCumshotMarkers) {
        m_videoSlider->showCumshotMarkers = false;
    } else {
        m_videoSlider->showCumshotMarkers = true;
    }
    emit setMarkerShowing("cumshot", m_videoSlider->showCumshotMarkers);
}

void MainWindow::toggleCyanMarkers()
{
    if (m_videoSlider->showCyanMarkers) {
        m_videoSlider->showCyanMarkers = false;
    } else {
        m_videoSlider->showCyanMarkers = true;
    }
    emit setMarkerShowing("cyan", m_videoSlider->showCyanMarkers);
}

void MainWindow::toggleDialogMarkers()
{
    if (m_videoSlider->showDialogMarkers) {
        m_videoSlider->showDialogMarkers = false;
    } else {
        m_videoSlider->showDialogMarkers = true;
    }
    emit setMarkerShowing("dialog", m_videoSlider->showDialogMarkers);
}

void MainWindow::toggleMagentaMarkers()
{
    if (m_videoSlider->showMagentaMarkers) {
        m_videoSlider->showMagentaMarkers = false;
    } else {
        m_videoSlider->showMagentaMarkers = true;
    }
    emit setMarkerShowing("magenta", m_videoSlider->showMagentaMarkers);
}

void MainWindow::toggleOrangeMarkers()
{
    if (m_videoSlider->showOrangeMarkers) {
        m_videoSlider->showOrangeMarkers = false;
    } else {
        m_videoSlider->showOrangeMarkers = true;
    }
    emit setMarkerShowing("orange", m_videoSlider->showOrangeMarkers);
}

void MainWindow::toggleSceneTransitionMarkers()
{
    if (m_videoSlider->showSceneMarkers) {
        m_videoSlider->showSceneMarkers = false;
    } else {
        m_videoSlider->showSceneMarkers = true;
    }
    emit setMarkerShowing("scene", m_videoSlider->showSceneMarkers);
}

void MainWindow::toggleStripMarkers()
{
    if (m_videoSlider->showStripMarkers) {
        m_videoSlider->showStripMarkers = false;
    } else {
        m_videoSlider->showStripMarkers = true;
    }
    emit setMarkerShowing("strip", m_videoSlider->showStripMarkers);
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
            m_playing = false;
        } else {
            m_player->play();
            m_playing = true;
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

void MainWindow::changePlaybackSpeed(double mrate)
{
    float newSpeed = m_playbackSpeed + mrate;
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

void MainWindow::videoSeek(int mseconds)
{
    qint64 zeroNum = 0;
    if (m_player->isAvailable()) {
        qint64 hduration = m_player->duration();
        qint64 newPosition = m_player->position() + mseconds;

        if (newPosition < hduration && newPosition > zeroNum) {
            m_player->setPosition(newPosition);

        } else if (newPosition >= hduration) {
            m_player->setPosition(hduration);

        } else if (newPosition <= zeroNum) {
            m_player->setPosition(zeroNum);
        }
    }
}

void MainWindow::videoJumpToTime(int position)
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

void MainWindow::changeVolume(double mvolume)
{
    float n_volume = m_audioOutput->volume() + mvolume;
    if (n_volume > 1.0) {
        n_volume = 1.0;
    } else if (n_volume < 0.0) {
        n_volume = 0.0;
    }
    m_audioOutput->setVolume(n_volume);
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
    if (ui->videoWidget->isFullScreen()) {
        ui->videoWidget->setFullScreen(false);
        ui->videoWidget->setCursor(QCursor(Qt::ArrowCursor));
    } else {
        ui->videoWidget->setFullScreen(true);
        ui->videoWidget->setCursor(QCursor(Qt::BlankCursor));
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
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    QList<double> marker = m_videoMarkersList.value(markerType);
    marker.append(sliderPercent);
    m_videoMarkersList.insert(markerType, marker);

    m_videoSlider->setMarkers(m_videoMarkersList);
}

void MainWindow::nextMarker()
{
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);
    m_videoSlider->jumpToNextMarker(sliderPercent);
}

void MainWindow::previousMarker()
{
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);
    m_videoSlider->jumpToPreviousMarker(sliderPercent);
}

void MainWindow::clearSelectedMarker() {}

void MainWindow::clearMarkers()
{
    m_videoMarkersList.clear();
    m_videoSlider->setMarkers(m_videoMarkersList);
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

void MainWindow::addInMarker()
{
    m_inMarker = m_player->position();
}

void MainWindow::addOutMarker()
{
    m_outMarker = m_player->position();
}

void MainWindow::createSubclip()
{
    extractSubclipFromVideo();
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
    QString fullPath;
    int index = 0;

    QString documentsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Vura/Screenshots";
    QString fileBaseName = strippedFileName(m_currentFile);
    // Extract base name without extension for numbering
    QString nameWithoutExt = QFileInfo(fileBaseName).baseName();

    QString positionString;
    const int currentPosition = static_cast<int>(m_player->position());
    QString format = "mm-ss";
    if (m_player->position() > 3600)
        format = "hh-mm-ss";

    const QTime currentTime(
        (currentPosition / 3600) % 60,
        (currentPosition / 60) % 60,
        currentPosition % 60,
        (currentPosition * 1000) % 1000);

    positionString = currentTime.toString(format);

    do {
        if (index == 0) {
            fullPath = QDir(documentsDir).absoluteFilePath(QString("%1-%2.jpg").arg(nameWithoutExt, positionString));
        } else {
            fullPath = QDir(documentsDir).absoluteFilePath(QString("%1-%2 (%3).jpg").arg(nameWithoutExt, positionString, QString::number(index)));
        }
        index++;
    } while (QFile::exists(fullPath)); // Check if file exists


    QVideoFrame frame = m_videoSink->videoFrame();
    QImage image = frame.toImage();
    image.save(fullPath, "JPEG");
}


#pragma endregion


#pragma region PLAYLIST CONTEXT MENU


void MainWindow::showPlaylistContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->playlistView->mapToGlobal(pos); // Map to global position
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
    QModelIndex index = ui->playlistView->indexAt(m_pos);
    m_playlist->setCurrentIndex(index.row());
}

void MainWindow::playlistContextMenu_StreamVideoAction() {}

void MainWindow::playlistContextMenu_SaveVideoAction() {}

void MainWindow::playlistContextMenu_InformationVideoAction() {}

void MainWindow::playlistContextMenu_ShowFolderVideoAction()
{
    QModelIndex index = ui->playlistView->indexAt(m_pos);
    QVariant data = index.data(); // Get the data
    QString text = data.toString();
    QFileInfo info(text);
    QString directoryPath = info.absoluteDir().absolutePath();
    QUrl folderUrl = QUrl::fromLocalFile(directoryPath);
    if (!QDesktopServices::openUrl(folderUrl)) {
        // Handle error if the folder couldn't be opened
        qWarning() << "Could not open folder: " << directoryPath.toStdString();
    }
}

void MainWindow::playlistContextMenu_RemoveSelectedVideoAction()
{
    QModelIndex index = ui->playlistView->indexAt(m_pos);
    if (index.row() == m_playlist->currentIndex()) {
        m_playlist->next();
    }
    m_playlist->removeMedia(index.row());
}


#pragma endregion


#pragma region VIDEO CONTROLS

void MainWindow::seek(int mseconds)
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

void MainWindow::jumpTo(int mseconds)
{
    qDebug() << "Jump To command called.";
    qint64 zeroNum = 0;
    if (m_player->isAvailable()) {
        qint64 hduration = m_player->duration();
        qint64 newPosition = m_player->position() + mseconds;

        if (newPosition < hduration && newPosition > zeroNum) {
            m_player->setPosition(newPosition);

        } else if (newPosition >= hduration) {
            m_player->setPosition(hduration);

        } else if (newPosition <= zeroNum) {
            m_player->setPosition(zeroNum);
        }
    }
}


#pragma endregion


#pragma region VIDEO EDITING FUNCTIONS


void MainWindow::showErrorMessage(const QString &message)
{
    QMessageBox::warning(this, tr("vura"), message);
}

void MainWindow::extractSubclipFromVideo()
{
    if (m_inMarker <= 0 || m_outMarker <= 0) {
        showErrorMessage("Missing In Marker or Out Marker");
        return;
    }

    QString startTimestamp = createTimestampString(m_inMarker);
    QString endTimestamp = createTimestampString(m_outMarker);

    // Get the user's documents location as a QString
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString applicationFolder = "vura";
    QFileInfo fileInfo(m_currentFile);
    QString projectFolder = fileInfo.baseName();
    QString fileName = "subclip";
    QString dirPath = QDir::cleanPath(documentsPath + QDir::separator() + applicationFolder + QDir::separator() + projectFolder);

    // Check if project folder exists.
    QDir dir;

    if (dir.mkpath(dirPath)) {
        qDebug() << "Successfully created project directory (and parent directories if needed):" << dirPath;
    } else {
        qDebug() << "Failed to create project directory:" << dirPath;
    }

    QString outputFile = generateSubclipFilenameWithIncrement(dirPath, fileName, "mp4");

    qDebug() << "Input file: " << m_currentFile;
    qDebug() << "Output file: " << outputFile;
    qDebug() << "Start timestamp: " << startTimestamp;
    qDebug() << "End timestamp: " << endTimestamp;

    QProcess *ffmpegProc = new QProcess(this);
    QStringList arguments;
    arguments << "-ss" << startTimestamp << "-to" << endTimestamp << "-i" << m_currentFile << "-c" << "copy" << outputFile;

    // Optional: Connect signals to handle output/errors
    connect(ffmpegProc, &QProcess::readyReadStandardError, [=]() {
        qWarning() << ffmpegProc->readAllStandardError();
    });

    ffmpegProc->start("ffmpeg", arguments);
    // Use ffmpegProc->waitForFinished(); if synchronous behavior is needed
}

QString MainWindow::createTimestampString(qint64 pos)
{
    // Convert to seconds
    qint64 totalSeconds = pos / 1000;

    // Format as mm:ss or hh:mm:ss
    QTime time((totalSeconds / 3600) % 24, (totalSeconds / 60) % 60, totalSeconds % 60);
    QString format = (totalSeconds >= 3600) ? "hh:mm:ss" : "mm:ss";
    QString timestamp = time.toString(format);

    // 'timestamp' is now a QString (e.g., "01:30" or "01:05:10")
    return timestamp;
}

QString MainWindow::generateSubclipFilenameWithIncrement(const QString &directoryPath, const QString &baseFileName, const QString &extension)
{
    QString fullPath;
    int index = 0;

    // Extract base name without extension for numbering
    QString nameWithoutExt = QFileInfo(baseFileName).baseName();

    do {
        if (index == 0) {
            fullPath = QDir(directoryPath).absoluteFilePath(baseFileName + "." + extension);
        } else {
            fullPath = QDir(directoryPath).absoluteFilePath(nameWithoutExt + QString::number(index) + "." + extension);
        }
        index++;
    } while (QFile::exists(fullPath)); // Check if file exists

    return fullPath;
}


#pragma endregion


#pragma region CORE APPLICATION FUNCTIONS


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

        if (m_durationLabelShowRemainingTime) {
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
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_hashFile) {
        if (!m_currentFileHash.isEmpty())
            m_videoMarkers->saveMarkers(m_currentFileHash, m_videoMarkersList);
    } else {
        if (!m_currentFile.isEmpty())
            m_videoMarkers->saveMarkers(m_currentFile, m_videoMarkersList);
    }
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
            if (m_pausePlaybackWhenMinimized)
                m_player->pause();
        }
    }
    QMainWindow::changeEvent(event);
}

bool MainWindow::event(QEvent *e)
{
    switch (e->type()) {
        case QEvent::HoverMove:
            if (m_hideCursorWhenPlaying) {
                if (ui->videoWidget->rect().contains(QCursor::pos())) {
                    if (m_playing) {
                        timer->stop();
                        ui->videoWidget->setCursor(QCursor(Qt::ArrowCursor));
                        timer->start(m_hideCursorTime);

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
            if (m_playing)
                timer->start(m_hideCursorTime);


        default:
            return QMainWindow::event(e);
    }
}


#pragma endregion


#pragma region FUNCTIONS


bool MainWindow::isPlaylist(const QUrl &url)
{
    QFileInfo fileInfo(url.toString());
    QString fileExtension = fileInfo.suffix();

    if (fileExtension == "hlist")
        return true;

    return false;
}

bool MainWindow::loadPlaylist(const QUrl &url)
{
    if (m_playlistLoaded) {
        QMessageBox::StandardButton confirmationBox;
        confirmationBox = QMessageBox::question(this, "Close Playlist", "Are you sure you want to close the current playlist and load the selected one?",
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

QString MainWindow::getMarker(const double &markerTime)
{
    QSettings settings;

    double markerMatchTolerance = settings.value("markerMatchTolerance", 0.001).toDouble();
    QString markerKey;
    double m_time;
    double markerDif;

    for (QMap<QString, QList<double>>::iterator i = m_videoMarkersList.begin(); i != m_videoMarkersList.end(); ++i)
    {
        QString m_key = i.key();
        foreach (const double marker, i.value())
        {
            if (qAbs(marker - markerTime) <= markerMatchTolerance)
            {
                if (!markerKey.isEmpty())
                {
                    if (qAbs(marker - markerTime) <= markerDif)
                    {
                        markerKey = m_key;
                        m_time = marker;
                        markerDif = qAbs(marker - markerTime);
                    }
                }
                else
                {
                    markerKey = m_key;
                    m_time = marker;
                    markerDif = qAbs(marker - markerTime);
                }
            }
        }
    }

    return markerKey;
}

void MainWindow::markersChanged(const QString &markerName, const double &markerTime, const QString &markerType)
{
    if (markerType == m_markerValue)
    {
        QList<double> markers = m_videoMarkersList.value(m_markerValue).toList();

        if (!markers.isEmpty())
        {
            markers.replace(m_markerIndex, markerTime);
            m_videoMarkersList.insert(m_markerValue, markers);
        }
    }
    else
    {
        QList<double> markers = m_videoMarkersList.value(m_markerValue).toList();

        if (!markers.isEmpty())
        {
            markers.removeAt(m_markerIndex);
            QList<double> marker = m_videoMarkersList.value(markerType).toList();
            marker.append(markerTime);
            m_videoMarkersList.insert(markerType, marker);
        }
    }
    m_videoSlider->setMarkers(m_videoMarkersList);
}

void MainWindow::markerDeleted(const double &markerTime)
{
    QSettings settings;

    double markerMatchTolerance = settings.value("markerMatchTolerance", 0.001).toDouble();
    QString markerKey;
    double m_time;
    double markerDif;

    for (QMap<QString, QList<double>>::iterator i = m_videoMarkersList.begin(); i != m_videoMarkersList.end(); ++i)
    {
        QString m_key = i.key();
        foreach (const double marker, i.value())
        {
            if (qAbs(marker - markerTime) <= markerMatchTolerance)
            {
                if (!markerKey.isEmpty())
                {
                    if (qAbs(marker - markerTime) <= markerDif)
                    {
                        markerKey = m_key;
                        m_time = marker;
                        markerDif = qAbs(marker - markerTime);
                    }
                }
                else
                {
                    markerKey = m_key;
                    m_time = marker;
                    markerDif = qAbs(marker - markerTime);
                }
            }
        }
    }

    if (!markerKey.isEmpty())
    {
        if (m_videoMarkersList.contains(markerKey))
        {
            QList<double>& m_list = m_videoMarkersList[markerKey];
            bool removed = m_list.removeAll(m_time);

            if (removed)
            {
                qDebug() << tr("Removed marker %1").arg(markerKey);
            }
            else
            {
                qDebug() << tr("Failed to removed marker %1").arg(markerKey);
            }
        }
    }

    m_videoSlider->setMarkers(m_videoMarkersList);
}

void MainWindow::saveMediaFilterList(const QStringList& filterList)
{
    QSettings settings;
    settings.setValue("mediaFileFilter", QVariant::fromValue(filterList));
    settings.sync();
}

QStringList MainWindow::loadMediaFilterList()
{
    QSettings settings;

    QStringList defaultList;
    QVariant value = settings.value("mediaFileFilter", QVariant::fromValue(defaultList));
    QStringList fileFilters = value.value<QStringList>();

    if (fileFilters.isEmpty()) {
        fileFilters << "*.mp4" << "*.wmv";
    }

    return fileFilters;
}

void MainWindow::showNotImplemented_Message()
{
    QMessageBox::information(this, "Not Implemented", "Sorry this function has not been implemented yet.");
}

void MainWindow::loadFile(const QString &fileName)
{
    const int previousMediaCount = m_playlist->mediaCount();
    m_playlist->addMedia(fileName);

    if (m_playlist->mediaCount() > previousMediaCount) {
        auto index = m_playlistModel->index(previousMediaCount, 0);
        ui->playlistView->setCurrentIndex(index);
        jump(index);
    }
}

QString MainWindow::strippedFileName(const QString &fileName)
{
    return QFileInfo(fileName).fileName();
}

QString MainWindow::timestampString(qint64 position)
{
    QString tStr;
    if (position || m_duration) {
        QTime currentTime((position / 3600) % 60, (position / 60) % 60, position % 60,
                          (position * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60, m_duration % 60,
                        (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    return tStr;
}

qint64 MainWindow::fileHash(const QString& filePath)
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

void MainWindow::setApplicationWindowTitle()
{
    QString windowTitle;

    if (!m_player->source().isEmpty()) {
        windowTitle = QString("%1 - Vura %2").arg(strippedFileName(m_currentFile), VURA_VERSION_STRING);
        m_trayIcon->setToolTip(strippedFileName(m_currentFile));
        m_sourceLoaded = false;

    } else {
        windowTitle = QString("Vura %1").arg(VURA_VERSION_STRING);
        m_trayIcon->setToolTip("Vura media player");
        m_sourceLoaded = true;

    }

    setWindowTitle(windowTitle);
}

void MainWindow::setSystemTrayIcon()
{
    if (m_systemTray) {
        m_trayIcon->show();

    } else if (!m_systemTray) {
        m_trayIcon->hide();
    }
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
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
    }
}

bool MainWindow::createUserDirs()
{
    // User Directories
    qDebug() << "Running user directory initialization...";
    QStringList directoryList;
    QString documentsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Vura";
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


#pragma endregion


void MainWindow::systemTray_Clicked(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (this->isHidden() || this->isMinimized()) {
            this->showNormal();
            this->activateWindow();
        }
    }
}

void MainWindow::hideSystemTray()
{
    if (m_systemTray_Showing) {
        this->hide();
        m_player->pause();
        systemTray_ToggleShow->setText(tr("Show Vura"));
        m_systemTray_Showing = false;
    } else {
        this->show();
        this->showNormal();
        this->raise();
        this->activateWindow();
        systemTray_ToggleShow->setText(tr("Hide Vura in taskbar"));
        m_systemTray_Showing = true;
    }
}

void MainWindow::systemTray_Stop()
{
    m_player->stop();
}

void MainWindow::systemTray_Record()
{
    showNotImplemented_Message();
}

void MainWindow::systemTray_Faster()
{
    changePlaybackSpeed(m_playbackSpeedAdjustment);
}

void MainWindow::systemTray_FasterFine()
{
    changePlaybackSpeed(m_playbackSpeedFineAdjustment);
}

void MainWindow::systemTray_Normal()
{
    setPlaybackSpeedNormal();
}

void MainWindow::systemTray_SlowerFine()
{
    changePlaybackSpeed(-m_playbackSpeedFineAdjustment);
}

void MainWindow::systemTray_Slower()
{
    changePlaybackSpeed(-m_playbackSpeedAdjustment);
}

void MainWindow::systemTray_IncreaseVolume()
{
    changeVolume(m_volumeStep);
}

void MainWindow::systemTray_DecreaseVolume()
{
    changeVolume(-m_volumeStep);
}

void MainWindow::systemTray_OpenFile()
{
    QSettings settings;

    // File filters
    QStringList fileFilters;
    fileFilters << MediaFileExtensions;
    fileFilters << VideoFileExtensions;
    fileFilters << AudioFileExtensions;
    fileFilters << PlaylistFileExtensions;
    fileFilters << "All Files (*.*)";

    // Create open file dialog.
    QFileDialog fileDialog(this);
    fileDialog.setNameFilters(fileFilters);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open File"));
    fileDialog.setDirectory(settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString());

    if (fileDialog.exec() == QDialog::Accepted) {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (!selectedFiles.isEmpty()) {
            QStringList fileList;
            for (auto &url : selectedFiles) {
                fileList.append(url);
            }

            // Set last file directory where file was opened.
            QString lastFileDirectory = selectedFiles.last();
            settings.setValue("lastFileDirectory", QFileInfo(lastFileDirectory).path());

            openFiles(fileList);
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
}
