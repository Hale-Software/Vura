#include "playlistmodel.h"
#include "../utility/playlist.h"

#include <QFileInfo>
#include <qmimedata.h>
#include <QUrl>

PlaylistModel::PlaylistModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_playlist.reset(new Playlist);
    connect(m_playlist.get(), &Playlist::mediaAboutToBeInserted, this, &PlaylistModel::beginInsertItems);
    connect(m_playlist.get(), &Playlist::mediaInserted, this, &PlaylistModel::endInsertItems);
    connect(m_playlist.get(), &Playlist::mediaAboutToBeRemoved, this, &PlaylistModel::beginRemoveItems);
    connect(m_playlist.get(), &Playlist::mediaRemoved, this, &PlaylistModel::endRemoveItems);
    connect(m_playlist.get(), &Playlist::mediaChanged, this, &PlaylistModel::changeItems);
}

PlaylistModel::~PlaylistModel() = default;

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid() ? m_playlist->mediaCount() : 0;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? ColumnCount : 0;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid() && row >= 0 && row < m_playlist->mediaCount()
                    && column >= 0 && column < ColumnCount
            ? createIndex(row, column)
            : QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::DisplayRole) {
        QVariant value = m_data[index];
        if (!value.isValid() && index.column() == Title) {
            QUrl location = m_playlist->media(index.row());
            return QFileInfo(location.path()).fileName();
        }

        return value;
    }
    return QVariant();
}

Playlist *PlaylistModel::playlist() const
{
    return m_playlist.get();
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    m_data[index] = value;
    emit dataChanged(index, index);
    return true;
}

void PlaylistModel::beginInsertItems(int start, int end)
{
    m_data.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::endInsertItems()
{
    endInsertRows();
}

void PlaylistModel::beginRemoveItems(int start, int end)
{
    m_data.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::endRemoveItems()
{
    endInsertRows();
}

void PlaylistModel::changeItems(int start, int end)
{
    m_data.clear();
    emit dataChanged(index(start, 0), index(end, ColumnCount));
}

#include "moc_playlistmodel.cpp"
