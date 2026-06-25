#pragma once

#include <QObject>
#include <QListView>
#include <QStackedWidget>
#include <QStringList>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QAction>
#include <QDebug>

class PlaylistModel;
class PlaylistEmptyStateWidget;



class PlaylistController : public QObject {
    Q_OBJECT

public:
    explicit PlaylistController(QListView* view,
                                PlaylistEmptyStateWidget* emptyPlaylistWidget,
                                QStackedWidget* container,
                                QObject* parent = nullptr);

    ~PlaylistController() override;

    PlaylistModel* getModel() const { return m_model; }

public slots:
    void showContextMenu(const QPoint& pos);
    void requestFileImport() const;
    void addFolder() const;
    void filesDropped(const QStringList &filePaths) const;
    void clearPlaylist() const;
    void hidePlaylist() const;
    void showPlaylist() const;
    void togglePlaylist() const;
    void nextTrack();
    void previousTrack();

signals:
    void playTrackRequested(const QUrl &mediaUrl);
    void playlistUpdated(int mediaCount);

private slots:
    void updateEmptyState();
    void handleItemDoubleClicked(const QModelIndex &index);
    void itemClicked(const QModelIndex &index);

private:
    void processFilePaths(const QStringList &paths) const;

    PlaylistModel* m_model;
    QListView* m_view;
    PlaylistEmptyStateWidget* m_emptyPlaylistWidget;
    QStackedWidget* m_container;

    QMenu* m_contextMenu;
    QAction* m_addFileAction;
    QAction* m_addFolderAction;
    QAction* m_clearPlaylistAction;

};
