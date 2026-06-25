#include "PlaylistController.h"
#include "PlaylistEmptyStateWidget.h"
#include <libvura/playlist/PlaylistModel.h>
#include <libvura/playlist/PlaylistDelegate.h>


PlaylistController::PlaylistController(QListView* view, PlaylistEmptyStateWidget* emptyPlaylistWidget, QStackedWidget* container, QObject* parent) :
        QObject(parent),
        m_model(new PlaylistModel()),
        m_view(view),
        m_emptyPlaylistWidget(emptyPlaylistWidget),
        m_container(container)
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(new PlaylistDelegate());

    //connect(m_view, &QListView::clicked, this, &PlaylistController::itemClicked);
    connect(m_view, &QListView::doubleClicked, this, &PlaylistController::handleItemDoubleClicked);
    connect(m_view, &QListView::customContextMenuRequested, this, &PlaylistController::showContextMenu);

    connect(m_model, &PlaylistModel::rowsInserted, this, &PlaylistController::updateEmptyState);
    connect(m_model, &PlaylistModel::rowsRemoved, this, &PlaylistController::updateEmptyState);
    connect(m_model, &PlaylistModel::modelReset, this, &PlaylistController::updateEmptyState);
    updateEmptyState();
}

PlaylistController::~PlaylistController() {}

void PlaylistController::itemClicked(const QModelIndex &index)
{
    //if (index.isValid())
    //    emit playTrackRequested(index.row());
}

void PlaylistController::handleItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = m_model->currentURL(index.row());

    if (!filePath.isEmpty())
        emit playTrackRequested(QUrl::fromLocalFile(filePath));
}

void PlaylistController::nextTrack()
{
    int nextIndex = m_model->getNextIndex();
    QString filePath = m_model->currentURL(nextIndex);

    if (!filePath.isEmpty())
        emit playTrackRequested(QUrl::fromLocalFile(filePath));
}

void PlaylistController::previousTrack()
{
    int previousIndex = m_model->getPreviousIndex();
    QString filePath = m_model->currentURL(previousIndex);

    if (!filePath.isEmpty())
        emit playTrackRequested(QUrl::fromLocalFile(filePath));
}

void PlaylistController::showContextMenu(const QPoint &pos)
{
    QModelIndex index = m_view->indexAt(pos);
    m_contextMenu = new QMenu(tr("Context Menu"), m_view);

    m_addFileAction = new QAction(tr("Add File"), m_contextMenu);
    connect(m_addFileAction, &QAction::triggered, this, &PlaylistController::requestFileImport);

    m_addFolderAction = new QAction(tr("Add Folder"), m_contextMenu);
    connect(m_addFolderAction, &QAction::triggered, this, &PlaylistController::addFolder);

    m_clearPlaylistAction = new QAction(tr("Clear Playlist"), m_contextMenu);
    connect(m_clearPlaylistAction, &QAction::triggered, this, &PlaylistController::clearPlaylist);

    m_contextMenu->exec(m_view->mapToGlobal(pos));
}

void PlaylistController::updateEmptyState()
{
    const int trackCount = m_model->rowCount();
    if (trackCount == 0) {
        m_container->setCurrentIndex(1);
    } else {
        m_container->setCurrentIndex(0);
    }

    emit playlistUpdated(trackCount);
}

void PlaylistController::requestFileImport() const
{
    const QStringList files = QFileDialog::getOpenFileNames(
            m_view,
            tr("Open Media Files"),
            QDir::homePath(),
            tr("Media Files (*.mp4 *.mkv *.avi *.mp3 *.wav *.flac);;All Files (*.*)")
        );

    if (!files.isEmpty())
        processFilePaths(files);
}

void PlaylistController::addFolder() const
{
    const QString dir = QFileDialog::getExistingDirectory(
            m_view,
            tr("Open Directory"),
            QDir::homePath(),
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

        processFilePaths(filePaths);
    }
}

void PlaylistController::filesDropped(const QStringList &filePaths) const
{
    processFilePaths(filePaths);
}

void PlaylistController::processFilePaths(const QStringList &paths) const
{
    for (const QString& path : paths) {
        m_model->addItem({path, path, 0, 0, false});
    }
}

void PlaylistController::clearPlaylist() const
{
    m_model->clear();
}

void PlaylistController::hidePlaylist() const
{
    m_container->hide();
}

void PlaylistController::showPlaylist() const
{
    m_container->show();
}

void PlaylistController::togglePlaylist() const
{
    if (m_container->isVisible()) {
        hidePlaylist();
    } else {
        showPlaylist();
    }
}
