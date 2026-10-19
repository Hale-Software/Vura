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

#include "UpdateChecker.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>


UpdateChecker::UpdateChecker(QObject* parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{
    connect(m_nam, &QNetworkAccessManager::finished,
            this,  &UpdateChecker::onManifestReply);
}

void UpdateChecker::check()
{
    if (m_busy) return;
    m_busy = true;

    qDebug() << "UpdateChecker: checking" << k_manifestUrl;
    m_nam->get(QNetworkRequest(QUrl(k_manifestUrl)));
}

void UpdateChecker::onManifestReply(QNetworkReply* reply)
{
    disconnect(m_nam, &QNetworkAccessManager::finished,
               this,  &UpdateChecker::onManifestReply);

    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        m_busy = false;
        emit error(tr("Update check failed: %1").arg(reply->errorString()));
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        m_busy = false;
        emit error(tr("Invalid update manifest: %1").arg(parseError.errorString()));
        return;
    }

    const QJsonObject obj       = doc.object();
    const QString     version   = obj.value("version").toString();
    const QString     updaterUrl = obj.value("updater_url").toString();
    m_downloadUrl               = obj.value("download_url").toString();

    if (version.isEmpty() || updaterUrl.isEmpty()) {
        m_busy = false;
        emit error(tr("Malformed manifest: missing version or updater_url"));
        return;
    }

    if (!isNewerVersion(version)) {
        m_busy = false;
        emit noUpdateAvailable();
        return;
    }

    qDebug() << "UpdateChecker: update available" << version;
    emit updateAvailable(version);

    downloadUpdater(updaterUrl);
}

void UpdateChecker::downloadUpdater(const QString& updaterUrl)
{
    const QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    m_updaterTempPath = tempDir + "/updater.exe";

    m_tempFile = new QFile(m_updaterTempPath, this);
    if (!m_tempFile->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        m_busy = false;
        emit error(tr("Cannot write to temp directory: %1").arg(m_tempFile->errorString()));
        return;
    }

    QNetworkRequest req{QUrl(updaterUrl)};
    // Follow redirects — GitHub releases, CDNs etc. all redirect
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);

    m_downloadReply = m_nam->get(req);

    connect(m_downloadReply, &QNetworkReply::downloadProgress,
            this,            &UpdateChecker::onUpdaterDownloadProgress);
    connect(m_downloadReply, &QNetworkReply::readyRead, this, [this]() {
        if (m_tempFile)
            m_tempFile->write(m_downloadReply->readAll());
    });
    connect(m_downloadReply, &QNetworkReply::finished,
            this,            &UpdateChecker::onUpdaterDownloadFinished);
}

void UpdateChecker::onUpdaterDownloadProgress(const qint64 received, const qint64 total)
{
    emit downloadProgress(received, total);
}

void UpdateChecker::onUpdaterDownloadFinished()
{
    m_tempFile->flush();
    m_tempFile->close();

    if (m_downloadReply->error() != QNetworkReply::NoError) {
        m_busy = false;
        emit error(tr("Updater download failed: %1")
                       .arg(m_downloadReply->errorString()));
        m_downloadReply->deleteLater();
        m_downloadReply = nullptr;
        return;
    }

    m_downloadReply->deleteLater();
    m_downloadReply = nullptr;

    qDebug() << "UpdateChecker: updater saved to" << m_updaterTempPath;
    emit updateReadyToInstall();

    launchUpdaterAndQuit();
}

void UpdateChecker::launchUpdaterAndQuit()
{
    const QString installDir = QCoreApplication::applicationDirPath();
    const QString appExe     = QCoreApplication::applicationFilePath();

    const QStringList args = {
        "--install-dir",  installDir,
        "--download-url", m_downloadUrl,
        "--app-exe",      appExe
    };

    qDebug() << "UpdateChecker: launching" << m_updaterTempPath << args;

    if (!QProcess::startDetached(m_updaterTempPath, args)) {
        m_busy = false;
        emit error(tr("Failed to launch updater: %1").arg(m_updaterTempPath));
        return;
    }

    QCoreApplication::quit();
}

bool UpdateChecker::isNewerVersion(const QString& remote)
{
    const QVersionNumber current = QVersionNumber::fromString(k_currentVersion);
    const QVersionNumber latest  = QVersionNumber::fromString(remote);
    return latest > current;
}
