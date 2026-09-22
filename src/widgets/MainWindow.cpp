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

#include "MainWindow.h"

#include "SeekSlider.h"

#include <libvura/media/media-controller.h>
#include <libvura/media/playlist.h>
#include <libvura/media/video-stage.h>
#include <libvura/media/sleep-inhibitor.h>

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

#ifdef VURA_HAVE_QTMULTIMEDIA
#include <QAudio>
#endif

namespace {

/// A volume slider must be perceptual or the top quarter of its travel does
/// nothing audible. Qt ships the conversion; without Qt Multimedia we use
/// the same cubic curve by hand.
qreal sliderToLinear(int sliderValue)
{
    const qreal fraction = qreal(sliderValue) / 100.0;
#ifdef VURA_HAVE_QTMULTIMEDIA
    return QAudio::convertVolume(fraction, QAudio::LogarithmicVolumeScale,
                                 QAudio::LinearVolumeScale);
#else
    return fraction * fraction * fraction;
#endif
}

int linearToSlider(qreal linear)
{
#ifdef VURA_HAVE_QTMULTIMEDIA
    const qreal fraction = QAudio::convertVolume(linear, QAudio::LinearVolumeScale,
                                                 QAudio::LogarithmicVolumeScale);
#else
    const qreal fraction = std::cbrt(linear);
#endif
    return int(qRound(fraction * 100.0));
}

const QStringList kMediaFilters = {
    QStringLiteral("Media files (*.mp4 *.mkv *.avi *.mov *.webm *.mp3 *.flac *.m4a *.ogg *.opus "
                   "*.wav *.aac *.wmv *.mpg *.mpeg *.ts)"),
    QStringLiteral("All files (*)"),
};

} // namespace

MainWindow::MainWindow(MediaController *controller, QWidget *parent) : QMainWindow(parent), m_controller(controller), m_sleepInhibitor(new SleepInhibitor(this))
{
    setWindowTitle(tr("Player"));
    setAcceptDrops(true);
    resize(960, 600);

    m_stage = new VideoStage(this);
    setCentralWidget(m_stage);

    // The stage decides which output the current engine can drive, and is
    // re-consulted on every backend swap.
    m_controller->setVideoOutputProvider([this](media::Engine *engine) { return m_stage->outputFor(engine); });

    buildActions();
    buildTransportBar();
    buildPlaylistDock();
    buildMenus();
    connectController();

    applyCapabilities(m_controller->capabilities());
    m_volumeSlider->setValue(linearToSlider(m_controller->volume()));
    updateTimeLabels();

    QSettings settings;
    restoreGeometry(settings.value(QStringLiteral("window/geometry")).toByteArray());
    restoreState(settings.value(QStringLiteral("window/state")).toByteArray());
}

MainWindow::~MainWindow() = default;

void MainWindow::buildActions()
{
    const auto icon = [this](QStyle::StandardPixmap pixmap) { return style()->standardIcon(pixmap); };

    m_playPauseAction = new QAction(icon(QStyle::SP_MediaPlay), tr("Play"), this);
    m_playPauseAction->setShortcut(Qt::Key_Space);
    connect(m_playPauseAction, &QAction::triggered, m_controller, &MediaController::togglePlayPause);

    m_stopAction = new QAction(icon(QStyle::SP_MediaStop), tr("Stop"), this);
    connect(m_stopAction, &QAction::triggered, m_controller, &MediaController::stop);

    m_previousAction = new QAction(icon(QStyle::SP_MediaSkipBackward), tr("Previous"), this);
    connect(m_previousAction, &QAction::triggered, m_controller, &MediaController::previous);

    m_nextAction = new QAction(icon(QStyle::SP_MediaSkipForward), tr("Next"), this);
    connect(m_nextAction, &QAction::triggered, m_controller, &MediaController::next);

    m_muteAction = new QAction(icon(QStyle::SP_MediaVolume), tr("Mute"), this);
    m_muteAction->setCheckable(true);
    m_muteAction->setShortcut(Qt::Key_M);
    connect(m_muteAction, &QAction::toggled, m_controller, &MediaController::setMuted);

    m_fullScreenAction = new QAction(tr("Full screen"), this);
    m_fullScreenAction->setCheckable(true);
    m_fullScreenAction->setShortcut(Qt::Key_F);
    connect(m_fullScreenAction, &QAction::triggered, this, &MainWindow::toggleFullScreen);

    m_shuffleAction = new QAction(tr("Shuffle"), this);
    m_shuffleAction->setCheckable(true);
    connect(m_shuffleAction, &QAction::toggled, m_controller->playlist(), &Playlist::setShuffled);

    m_loadSubtitleAction = new QAction(tr("Load subtitle file…"), this);
    connect(m_loadSubtitleAction, &QAction::triggered, this, [this] {
        const QString path = QFileDialog::getOpenFileName(
                this, tr("Load subtitle file"), {},
                tr("Subtitles (*.srt *.ass *.ssa *.vtt *.sub);;All files (*)"));
        if (path.isEmpty())
            return;
        if (!m_controller->loadExternalSubtitle(QUrl::fromLocalFile(path)))
            showStatus(tr("This backend cannot load separate subtitle files."));
    });
}

void MainWindow::buildTransportBar()
{
    auto *bar = new QToolBar(tr("Transport"), this);
    bar->setObjectName(QStringLiteral("transportBar"));
    bar->setMovable(false);
    bar->setFloatable(false);

    bar->addAction(m_previousAction);
    bar->addAction(m_playPauseAction);
    bar->addAction(m_stopAction);
    bar->addAction(m_nextAction);
    bar->addSeparator();

    m_positionLabel = new QLabel(QStringLiteral("0:00"), bar);
    m_positionLabel->setMinimumWidth(56);
    m_positionLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    bar->addWidget(m_positionLabel);

    m_seekSlider = new SeekSlider(bar);
    bar->addWidget(m_seekSlider);

    m_durationLabel = new QLabel(QStringLiteral("0:00"), bar);
    m_durationLabel->setMinimumWidth(56);
    bar->addWidget(m_durationLabel);

    bar->addSeparator();
    m_rateBox = new QComboBox(bar);
    for (qreal rate : {0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0})
        m_rateBox->addItem(tr("%1×").arg(rate), rate);
    m_rateBox->setCurrentIndex(2);
    bar->addWidget(m_rateBox);

    bar->addAction(m_muteAction);
    m_volumeSlider = new QSlider(Qt::Horizontal, bar);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setFixedWidth(110);
    bar->addWidget(m_volumeSlider);

    addToolBar(Qt::BottomToolBarArea, bar);

    connect(m_seekSlider, &SeekSlider::scrubbed, m_controller, [this](qint64 ms) {
        // Preview seeks while dragging; the controller coalesces them.
        m_controller->seek(ms);
    });
    connect(m_seekSlider, &SeekSlider::scrubFinished, m_controller,
            [this](qint64 ms) { m_controller->seek(ms); });

    connect(m_volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        m_controller->setVolume(sliderToLinear(value));
    });

    connect(m_rateBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_controller->setRate(m_rateBox->itemData(index).toReal());
    });
}

void MainWindow::buildPlaylistDock()
{
    m_playlistDock = new QDockWidget(tr("Queue"), this);
    m_playlistDock->setObjectName(QStringLiteral("playlistDock"));

    m_playlistView = new QListView(m_playlistDock);
    m_playlistView->setModel(m_controller->playlist());
    m_playlistView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_playlistView->setDragDropMode(QAbstractItemView::InternalMove);
    m_playlistView->setAlternatingRowColors(true);

    connect(m_playlistView, &QListView::doubleClicked, this, [this](const QModelIndex &index) {
        m_controller->playIndex(index.row());
    });

    m_playlistDock->setWidget(m_playlistView);
    addDockWidget(Qt::RightDockWidgetArea, m_playlistDock);
}

void MainWindow::buildMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("Open files…"), QKeySequence::Open, this, &MainWindow::openFiles);
    fileMenu->addAction(tr("Open URL…"), this, &MainWindow::openUrl);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Clear queue"), m_controller->playlist(), &Playlist::clear);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Quit"), QKeySequence::Quit, qApp, &QApplication::quit);

    QMenu *playbackMenu = menuBar()->addMenu(tr("&Playback"));
    playbackMenu->addAction(m_playPauseAction);
    playbackMenu->addAction(m_stopAction);
    playbackMenu->addAction(m_previousAction);
    playbackMenu->addAction(m_nextAction);
    playbackMenu->addSeparator();

    m_audioTrackMenu = playbackMenu->addMenu(tr("Audio track"));
    m_subtitleTrackMenu = playbackMenu->addMenu(tr("Subtitles"));
    playbackMenu->addAction(m_loadSubtitleAction);
    playbackMenu->addSeparator();

    QMenu *repeatMenu = playbackMenu->addMenu(tr("Repeat"));
    m_repeatGroup = new QActionGroup(this);
    const auto addRepeat = [this, repeatMenu](const QString &text, Playlist::RepeatMode mode) {
        QAction *action = repeatMenu->addAction(text);
        action->setCheckable(true);
        action->setData(int(mode));
        m_repeatGroup->addAction(action);
        connect(action, &QAction::triggered, this, [this, mode] {
            m_controller->playlist()->setRepeatMode(mode);
        });
        return action;
    };
    addRepeat(tr("Off"), Playlist::RepeatNone)->setChecked(true);
    addRepeat(tr("Repeat one"), Playlist::RepeatOne);
    addRepeat(tr("Repeat all"), Playlist::RepeatAll);
    playbackMenu->addAction(m_shuffleAction);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(m_fullScreenAction);
    viewMenu->addAction(m_playlistDock->toggleViewAction());

    // Switching backends at runtime is the test that proves the abstraction
    // holds. Anything that leaks through the Engine interface shows up the
    // first time this menu is used mid-playback.
    m_backendMenu = menuBar()->addMenu(tr("&Backend"));
    auto *backendGroup = new QActionGroup(this);
    for (const media::BackendInfo &info : media::availableBackends()) {
        QAction *action = m_backendMenu->addAction(info.displayName);
        action->setCheckable(true);
        //action->setEnabled(info.available);
        action->setEnabled(true);
        action->setChecked(info.backend == m_controller->backend());
        backendGroup->addAction(action);

        connect(action, &QAction::triggered, this, [this, info] {
            QString error;
            if (!m_controller->setBackend(info.backend, &error))
                showStatus(tr("Could not switch backend: %1").arg(error));
            else if (!error.isEmpty())
                showStatus(error);
            else
                showStatus(tr("Now using the %1 backend.").arg(info.displayName));
        });
    }

    statusBar();
}

void MainWindow::connectController()
{
    connect(m_controller, &MediaController::positionChanged, this, [this](media::Msec ms) {
        m_seekSlider->setPositionFromEngine(ms);
        updateTimeLabels();
    });

    connect(m_controller, &MediaController::durationChanged, this, [this](media::Msec ms) {
        m_seekSlider->setRange(0, int(ms));
        updateTimeLabels();
    });

    connect(m_controller, &MediaController::playbackStateChanged, this,
            [this](media::PlaybackState state) {
                const bool playing = state == media::PlaybackState::Playing;
                m_playPauseAction->setIcon(style()->standardIcon(
                        playing ? QStyle::SP_MediaPause : QStyle::SP_MediaPlay));
                m_playPauseAction->setText(playing ? tr("Pause") : tr("Play"));

                // Only inhibit sleep for video; audio playback has no reason
                // to keep the display awake.
                const bool hasVideo =
                        !m_controller->engine()
                        || !m_controller->engine()->tracks(media::TrackType::Video).isEmpty();
                m_sleepInhibitor->setInhibited(playing && hasVideo, tr("Playing video"));
            });

    connect(m_controller, &MediaController::mediaStatusChanged, this,
            [this](media::MediaStatus status) {
                switch (status) {
                case media::MediaStatus::Loading:
                    showStatus(tr("Loading…"), 0);
                    break;
                case media::MediaStatus::Buffering:
                    showStatus(tr("Buffering…"), 0);
                    break;
                case media::MediaStatus::Buffered:
                case media::MediaStatus::Loaded:
                    statusBar()->clearMessage();
                    break;
                case media::MediaStatus::Invalid:
                    showStatus(tr("This file could not be played."));
                    break;
                default:
                    break;
                }
            });

    connect(m_controller, &MediaController::seekableChanged, this,
            [this](bool seekable) { m_seekSlider->setEnabled(seekable); });

    connect(m_controller, &MediaController::volumeChanged, this, [this](qreal linear) {
        QSignalBlocker blocker(m_volumeSlider);
        m_volumeSlider->setValue(linearToSlider(linear));
    });

    connect(m_controller, &MediaController::mutedChanged, this, [this](bool muted) {
        QSignalBlocker blocker(m_muteAction);
        m_muteAction->setChecked(muted);
        m_muteAction->setIcon(style()->standardIcon(muted ? QStyle::SP_MediaVolumeMuted
                                                          : QStyle::SP_MediaVolume));
    });

    connect(m_controller, &MediaController::rateChanged, this, [this](qreal rate) {
        const int index = m_rateBox->findData(rate);
        if (index >= 0) {
            QSignalBlocker blocker(m_rateBox);
            m_rateBox->setCurrentIndex(index);
        }
    });

    connect(m_controller, &MediaController::tracksChanged, this, &MainWindow::rebuildTrackMenus);
    connect(m_controller, &MediaController::capabilitiesChanged, this,
            &MainWindow::applyCapabilities);

    connect(m_controller, &MediaController::metaDataChanged, this,
            [this](const QVariantMap &data) {
                m_stage->setCoverArt(data.value(QLatin1String(media::meta::CoverArt))
                                             .value<QImage>());
                updateWindowTitle();
            });

    connect(m_controller, &MediaController::currentItemChanged, this,
            [this](const PlaylistItem &item) {
                m_stage->setCaption(item.displayTitle());
                updateWindowTitle();
            });

    connect(m_controller, &MediaController::errorOccurred, this,
            [this](media::ErrorKind kind, const QString &detail) {
                showStatus(detail.isEmpty() ? media::describe(kind) : detail, 8000);
            });

    connect(m_controller->playlist(), &Playlist::shuffleChanged, this, [this](bool shuffled) {
        QSignalBlocker blocker(m_shuffleAction);
        m_shuffleAction->setChecked(shuffled);
    });
}

void MainWindow::applyCapabilities(const media::Capabilities &capabilities)
{
    // Grey out what the current engine cannot do, rather than letting it
    // accept the call and silently do nothing. Silent no-ops are the hardest
    // class of bug to trace back to a backend swap.
    m_rateBox->setEnabled(capabilities.variableRate);
    m_rateBox->setToolTip(capabilities.pitchCorrection
                                  ? tr("Playback speed")
                                  : tr("This backend changes speed without pitch correction."));
    m_loadSubtitleAction->setEnabled(capabilities.externalSubtitles);
    m_subtitleTrackMenu->setEnabled(capabilities.subtitleTrackSelection);
    m_audioTrackMenu->setEnabled(capabilities.audioTrackSelection);

    // Rates the backend cannot honour are greyed out rather than silently
    // clamped, so the UI never offers a control that does nothing.
    if (auto *model = qobject_cast<QStandardItemModel *>(m_rateBox->model())) {
        for (int i = 0; i < m_rateBox->count(); ++i) {
            const qreal rate = m_rateBox->itemData(i).toReal();
            const bool inRange = rate >= capabilities.rateRange.first
                    && rate <= capabilities.rateRange.second;
            if (QStandardItem *item = model->item(i))
                item->setEnabled(inRange);
        }
    }
}

void MainWindow::rebuildTrackMenus()
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

    populate(m_audioTrackMenu, media::TrackType::Audio, false);
    populate(m_subtitleTrackMenu, media::TrackType::Subtitle, true);
}

void MainWindow::updateTimeLabels()
{
    m_positionLabel->setText(media::formatTime(m_controller->position()));
    m_durationLabel->setText(media::formatTime(m_controller->duration()));
}

void MainWindow::updateWindowTitle()
{
    const QString title = m_controller->currentTitle();
    setWindowTitle(title.isEmpty() ? tr("Player") : tr("%1 — Player").arg(title));
}

void MainWindow::showStatus(const QString &message, int timeoutMs)
{
    statusBar()->showMessage(message, timeoutMs);
}

void MainWindow::openFiles()
{
    QFileDialog dialog(this, tr("Open media"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilters(kMediaFilters);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QList<QUrl> urls;
    for (const QString &path : dialog.selectedFiles())
        urls.append(QUrl::fromLocalFile(path));
    openPaths(urls);
}

void MainWindow::openUrl()
{
    bool accepted = false;
    const QString text = QInputDialog::getText(this, tr("Open URL"), tr("Stream address:"),
                                               QLineEdit::Normal, {}, &accepted);
    if (!accepted || text.trimmed().isEmpty())
        return;

    const QUrl url = QUrl::fromUserInput(text.trimmed());
    if (!url.isValid()) {
        showStatus(tr("That address could not be parsed."));
        return;
    }
    openPaths({url});
}

void MainWindow::openPaths(const QList<QUrl> &urls)
{
    if (urls.isEmpty())
        return;
    m_controller->openUrls(urls);
}

void MainWindow::toggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
        if (m_wasMaximized)
            showMaximized();
        menuBar()->show();
        m_fullScreenAction->setChecked(false);
        return;
    }

    m_wasMaximized = isMaximized();
    menuBar()->hide();
    showFullScreen();
    m_fullScreenAction->setChecked(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(QStringLiteral("window/geometry"), saveGeometry());
    settings.setValue(QStringLiteral("window/state"), saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    // Dropping replaces the queue; holding Shift adds to it.
    if (event->modifiers() & Qt::ShiftModifier)
        m_controller->enqueue(urls);
    else
        openPaths(urls);

    event->acceptProposedAction();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        m_controller->seekRelative(-5000);
        return;
    case Qt::Key_Right:
        m_controller->seekRelative(5000);
        return;
    case Qt::Key_Up:
        m_volumeSlider->setValue(m_volumeSlider->value() + 5);
        return;
    case Qt::Key_Down:
        m_volumeSlider->setValue(m_volumeSlider->value() - 5);
        return;
    case Qt::Key_Escape:
        if (isFullScreen()) {
            toggleFullScreen();
            return;
        }
        break;
    default:
        break;
    }
    QMainWindow::keyPressEvent(event);
}
