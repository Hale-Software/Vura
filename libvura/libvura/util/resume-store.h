#pragma once

#include "../models/types.h"

#include <QHash>
#include <QObject>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

/// Remembers where each file was left off.
///
/// Small feature, disproportionate effect: its absence is one of the first
/// things people notice about a player, and it costs a JSON file.
class ResumeStore : public QObject
{
    Q_OBJECT

public:
    /// An empty path uses AppDataLocation/resume.json.
    explicit ResumeStore(QObject *parent = nullptr, const QString &path = {});
    ~ResumeStore() override;

    /// Returns 0 when there is nothing worth resuming.
    media::Msec positionFor(const QUrl &url) const;

    /// Ignores positions that are not worth restoring: very short media,
    /// the first few percent, and anything near the end, where resuming
    /// means "replay the credits".
    void remember(const QUrl &url, media::Msec position, media::Msec duration);

    void forget(const QUrl &url);
    void clear();

    void save();

    void setMinimumDuration(media::Msec ms) { m_minimumDuration = ms; }

private:
    void load();
    static QString keyFor(const QUrl &url);

    struct Entry {
        media::Msec position = 0;
        media::Msec duration = 0;
        qint64 savedAt = 0;
    };

    QString m_path;
    QHash<QString, Entry> m_entries;
    QTimer *m_saveTimer = nullptr;
    media::Msec m_minimumDuration = 90'000;
    bool m_dirty = false;
};
