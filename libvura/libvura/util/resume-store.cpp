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

#include "resume-store.h"

#include <libvura/config.h>

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
#include <QSettings>


ResumeStore::ResumeStore(QObject *parent, const QString &path)
    : QObject(parent),
      m_path(path),
      m_saveTimer(new QTimer(this))
{
    if (m_path.isEmpty()) {
        bool isDebugging = false;
        if (QString(VURA_BUILD_TYPE) == "Debug")
            isDebugging = true;
        const QString dir = isDebugging ? "debug" : QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (!QDir().mkpath(dir)) {
            qCritical() << "Failed to create resume store directory at " << dir;
            emit errorOccured(QString("Failed to create resume store directory at %1").arg(dir));
            m_initialized = false;
            return;
        }
        m_path = dir + QStringLiteral("/resume.json");
    }

    // Positions update constantly during playback; writing on every change
    // would hammer the disk for no benefit.
    m_saveTimer->setSingleShot(true);
    m_saveTimer->setInterval(5000);
    connect(m_saveTimer, &QTimer::timeout, this, &ResumeStore::save);

    m_initialized = true;
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

void ResumeStore::remember(const QUrl &url, const media::Msec position, const media::Msec duration)
{
    if (url.isEmpty() || duration < m_minimumDuration)
        return;

    const QString key = keyFor(url);
    const qreal fraction = static_cast<qreal>(position) / static_cast<qreal>(duration);

    if (fraction < QSettings().value("resumeStoreIgnoreBeforeFraction", 0.02).toDouble() || fraction > QSettings().value("resumeStoreIgnoreAfterFraction", 0.95).toDouble()) {
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
    if (m_entries.size() > QSettings().value("resumeStoreMaxEntries", 2000).toInt()) {
        QList<QPair<qint64, QString>> byAge;
        byAge.reserve(m_entries.size());
        for (auto it = m_entries.cbegin(); it != m_entries.cend(); ++it)
            byAge.append({it.value().savedAt, it.key()});
        std::sort(byAge.begin(), byAge.end());
        for (int i = 0; i < byAge.size() - QSettings().value("resumeStoreMaxEntries", 2000).toInt(); ++i)
            m_entries.remove(byAge.at(i).second);
    }

    QJsonObject root;
    for (auto it = m_entries.cbegin(); it != m_entries.cend(); ++it) {
        QJsonObject entry;
        entry[QStringLiteral("position")] = static_cast<double>(it.value().position);
        entry[QStringLiteral("duration")] = static_cast<double>(it.value().duration);
        entry[QStringLiteral("savedAt")] = static_cast<double>(it.value().savedAt);
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
        entry.position = static_cast<media::Msec>(value.value(QStringLiteral("position")).toDouble());
        entry.duration = static_cast<media::Msec>(value.value(QStringLiteral("duration")).toDouble());
        entry.savedAt = static_cast<qint64>(value.value(QStringLiteral("savedAt")).toDouble());
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
