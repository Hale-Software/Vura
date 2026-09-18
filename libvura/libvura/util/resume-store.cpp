#include "resume-store.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>
#include <QTimer>

namespace {
constexpr qreal kIgnoreBeforeFraction = 0.02;
constexpr qreal kIgnoreAfterFraction = 0.95;
constexpr int kMaxEntries = 2000;
} // namespace

ResumeStore::ResumeStore(QObject *parent, const QString &path)
    : QObject(parent)
    , m_path(path)
    , m_saveTimer(new QTimer(this))
{
    if (m_path.isEmpty()) {
        const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dir);
        m_path = dir + QStringLiteral("/resume.json");
    }

    // Positions update constantly during playback; writing on every change
    // would hammer the disk for no benefit.
    m_saveTimer->setSingleShot(true);
    m_saveTimer->setInterval(5000);
    connect(m_saveTimer, &QTimer::timeout, this, &ResumeStore::save);

    load();
}

ResumeStore::~ResumeStore()
{
    if (m_dirty)
        save();
}

media::Msec ResumeStore::positionFor(const QUrl &url) const
{
    return m_entries.value(keyFor(url)).position;
}

void ResumeStore::remember(const QUrl &url, media::Msec position, media::Msec duration)
{
    if (url.isEmpty() || duration < m_minimumDuration)
        return;

    const QString key = keyFor(url);
    const qreal fraction = qreal(position) / qreal(duration);

    if (fraction < kIgnoreBeforeFraction || fraction > kIgnoreAfterFraction) {
        // Near either end there is nothing to resume to, and a stale entry
        // would send the next play to the wrong place.
        if (m_entries.remove(key) > 0)
            m_dirty = true;
    } else {
        Entry &entry = m_entries[key];
        entry.position = position;
        entry.duration = duration;
        entry.savedAt = QDateTime::currentSecsSinceEpoch();
        m_dirty = true;
    }

    if (m_dirty && !m_saveTimer->isActive())
        m_saveTimer->start();
}

void ResumeStore::forget(const QUrl &url)
{
    if (m_entries.remove(keyFor(url)) > 0) {
        m_dirty = true;
        m_saveTimer->start();
    }
}

void ResumeStore::clear()
{
    if (m_entries.isEmpty())
        return;
    m_entries.clear();
    m_dirty = true;
    save();
}

void ResumeStore::save()
{
    if (!m_dirty)
        return;

    // Trim oldest entries so the file cannot grow without bound.
    if (m_entries.size() > kMaxEntries) {
        QList<QPair<qint64, QString>> byAge;
        byAge.reserve(m_entries.size());
        for (auto it = m_entries.cbegin(); it != m_entries.cend(); ++it)
            byAge.append({it.value().savedAt, it.key()});
        std::sort(byAge.begin(), byAge.end());
        for (int i = 0; i < byAge.size() - kMaxEntries; ++i)
            m_entries.remove(byAge.at(i).second);
    }

    QJsonObject root;
    for (auto it = m_entries.cbegin(); it != m_entries.cend(); ++it) {
        QJsonObject entry;
        entry[QStringLiteral("position")] = double(it.value().position);
        entry[QStringLiteral("duration")] = double(it.value().duration);
        entry[QStringLiteral("savedAt")] = double(it.value().savedAt);
        root.insert(it.key(), entry);
    }

    QSaveFile file(m_path);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
    if (file.commit())
        m_dirty = false;
}

void ResumeStore::load()
{
    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly))
        return;

    const QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    for (auto it = root.constBegin(); it != root.constEnd(); ++it) {
        const QJsonObject value = it.value().toObject();
        Entry entry;
        entry.position = media::Msec(value.value(QStringLiteral("position")).toDouble());
        entry.duration = media::Msec(value.value(QStringLiteral("duration")).toDouble());
        entry.savedAt = qint64(value.value(QStringLiteral("savedAt")).toDouble());
        m_entries.insert(it.key(), entry);
    }
}

QString ResumeStore::keyFor(const QUrl &url)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(url.toString(QUrl::NormalizePathSegments).toUtf8());

    // Include the size for local files so re-encoding or replacing a file
    // does not resume at a position that no longer means anything.
    if (url.isLocalFile()) {
        const QFileInfo info(url.toLocalFile());
        hash.addData(QByteArray::number(info.size()));
    }
    return QString::fromLatin1(hash.result().toHex());
}
