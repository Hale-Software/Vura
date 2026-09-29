#pragma once

#include <QHash>
#include <QList>
#include <QUrl>
#include <QWidget>

#include <libvura/models/video-marker-record.h>

class QLineEdit;
class QListView;
class QModelIndex;
class QToolButton;
class MarkerListModel;
class MarkerFilterProxyModel;
class MarkerItemDelegate;
class MarkerThumbnailer;

/// Premiere-style "Markers" panel: search box, colour filter chips and a list
/// of markers with thumbnail, name, in/out timecode and comments.
class MarkerPanel : public QWidget
{
    Q_OBJECT
public:
    explicit MarkerPanel(QWidget *parent = nullptr);

    void setSource(const QUrl &source);
    void setMarkers(const QList<VideoMarkerRecord> &markers);
    void setDurationMs(qint64 ms);
    void setFrameRate(double fps);

public slots:
    /// Keeps the chip in sync when visibility is changed elsewhere (menu, slider).
    void setTypeVisible(const QString &type, bool visible);

    signals:
        void seekRequested(qint64 ms);
    void editRequested(const VideoMarkerRecord &marker);
    void deleteRequested(const VideoMarkerRecord &marker);
    void typeVisibilityChanged(const QString &type, bool visible);

private:
    void buildUi();
    void requestMissingThumbnails();
    bool recordAt(const QModelIndex &proxyIndex, VideoMarkerRecord *out) const;
    void showContextMenu(const QPoint &pos);

    QLineEdit *m_search = nullptr;
    QListView *m_view = nullptr;
    MarkerListModel *m_model = nullptr;
    MarkerFilterProxyModel *m_proxy = nullptr;
    MarkerItemDelegate *m_delegate = nullptr;
    MarkerThumbnailer *m_thumbnailer = nullptr;
    QHash<QString, QToolButton *> m_chips;
};
