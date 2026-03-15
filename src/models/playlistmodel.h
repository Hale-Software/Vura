#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractItemModel>

#include <memory>

QT_BEGIN_NAMESPACE
class Playlist;
QT_END_NAMESPACE

class PlaylistModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Column { Title = 0, ColumnCount };

    explicit PlaylistModel(QObject *parent = nullptr);
    ~PlaylistModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Playlist *playlist() const;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::DisplayRole) override;

private slots:
    void beginInsertItems(int start, int end);
    void endInsertItems();
    void beginRemoveItems(int start, int end);
    void endRemoveItems();
    void changeItems(int start, int end);

private:
    std::unique_ptr<Playlist> m_playlist;
    QMap<QModelIndex, QVariant> m_data;
    const QString MimeType = "test.mp4";
};

#endif // PLAYLISTMODEL_H
