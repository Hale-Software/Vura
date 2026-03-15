#ifndef PLAYLISTFILEPARSER_P_H
#define PLAYLISTFILEPARSER_P_H

#include "playlist.h"
#include "qtmultimediaglobal.h"

#include <QObject>

#include <memory>

QT_BEGIN_NAMESPACE

class QIODevice;
class QUrl;
class QNetworkRequest;

class PlaylistFileParserPrivate;

class PlaylistFileParser : public QObject
{
    Q_OBJECT
public:
    PlaylistFileParser(QObject *parent = nullptr);
    ~PlaylistFileParser();

    enum FileType {
        UNKNOWN,
        M3U,
        M3U8, // UTF-8 version of M3U
        PLS,
        HLIST
    };

    void start(const QUrl &media, QIODevice *stream = nullptr, const QString &mimeType = QString());
    void start(const QUrl &request, const QString &mimeType = QString());
    void start(QIODevice *stream, const QString &mimeType = QString());
    void abort();

    QList<QUrl> playlist;

signals:
    void newItem(const QVariant &content);
    void finished();
    void error(Playlist::Error err, const QString &errorMsg);

private slots:
    void handleData();
    void handleError();

private:
    static FileType findByMimeType(const QString &mime);
    static FileType findBySuffixType(const QString &suffix);
    static FileType findByDataHeader(const char *data, quint32 size);
    static FileType findPlaylistType(QIODevice *device, const QString &mime);
    static FileType findPlaylistType(const QString &suffix, const QString &mime, const char *data = nullptr, quint32 size = 0);

    Q_DISABLE_COPY(PlaylistFileParser)
    Q_DECLARE_PRIVATE(PlaylistFileParser)
    std::unique_ptr<PlaylistFileParserPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // PLAYLISTFILEPARSER_P_H
