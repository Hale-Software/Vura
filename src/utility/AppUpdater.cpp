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

#include "AppUpdater.h"
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QCryptographicHash>


AppUpdater::AppUpdater(QObject *parent) : QObject(parent) {}

AppUpdater::~AppUpdater()
{
    if (m_currentReply) m_currentReply->deleteLater();
}

void AppUpdater::startDownload(const QString &downloadUrl, const QString &expectedHash)
{
    m_expectedHash = expectedHash.toLower().trimmed();

    const QUrl url(downloadUrl);

    // 1. Determine local temp file path based on URL file target extension
    QString fileName = url.fileName();
    if (fileName.isEmpty()) fileName = "update_package.tmp";

    const QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    m_downloadedFilePath = tempDir + QDir::separator() + fileName;

    m_tempFile.setFileName(m_downloadedFilePath);
    if (!m_tempFile.open(QIODevice::WriteOnly)) {
        emit downloadFinished(false, "Failed to create local temporary file storage.");
        return;
    }

    // 2. Fire Async Network Request
    QNetworkRequest request(url);
    // Follow redirect headers automatically (important for CDN downloads)
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    m_currentReply = m_networkManager.get(request);

    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &AppUpdater::onDownloadProgress);
    connect(m_currentReply, &QNetworkReply::readyRead, this, &AppUpdater::onReadyRead);
    connect(m_currentReply, &QNetworkReply::finished, this, &AppUpdater::onFinished);
}

void AppUpdater::onDownloadProgress(const qint64 bytesReceived, const qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

void AppUpdater::onReadyRead()
{
    if (m_currentReply) {
        m_tempFile.write(m_currentReply->readAll());
    }
}

void AppUpdater::onFinished() {
    m_tempFile.close();

    if (m_currentReply->error() != QNetworkReply::NoError) {
        emit downloadFinished(false, QString("Download failed: %1").arg(m_currentReply->errorString()));
        m_tempFile.remove();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    // --- INTEGRITY SIGNATURE VALIDATION PHASE ---
    emit downloadFinished(true, "Verifying file checksum...");

    if (!verifyFileIntegrity()) {
        emit downloadFinished(false, "Security Error: SHA-256 checksum mismatch! The file may be corrupted or compromised.");
        m_tempFile.remove(); // Force delete untrusted payload bytes immediately
    } else {
        emit downloadFinished(true, "Integrity verified. Launching installer...");
        executeInstaller();
    }

    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

bool AppUpdater::verifyFileIntegrity() {
    QFile file(m_downloadedFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Process file data in streaming chunks to avoid loading large installers completely into RAM
    QCryptographicHash hasher(QCryptographicHash::Sha256);
    char buffer[4096];
    qint64 bytesRead;

    while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0) {
        hasher.addData(buffer, bytesRead);
    }
    file.close();

    QString calculatedHash = hasher.result().toHex().toLower();

    // Return direct evaluation state comparison matrix
    return (calculatedHash == m_expectedHash);
}

void AppUpdater::executeInstaller() const
{
#if defined(Q_OS_WIN)
    // --- WINDOWS (MinGW / NSIS or InnoSetup installer bundles) ---
    // If zipped, you'd extract first. If it's an .exe installer:
    // Run installer detached so this app can exit completely for replacement
    QStringList arguments;
    arguments << "/S"; // Standard flag for modern silent/silent-ish installation loops

    QProcess::startDetached(m_downloadedFilePath, arguments);
    QCoreApplication::quit();

#elif defined(Q_OS_MAC)
    // --- MACOS (Assuming package download is a standalone installer script or .pkg) ---
    // For a .pkg package installer:
    QString m_script = QString("do shell script \"installer -pkg '%1' -target /\" with administrator privileges")
                       .arg(m_downloadedFilePath);

    // Prompts macOS native system UAC gatekeeper secure authentication popups
    QProcess::startDetached("osascript", QStringList() << "-e" << m_script);
    QCoreApplication::quit();

#elif defined(Q_OS_LINUX)
    // --- LINUX (Assuming self-contained AppImage replace execution payload) ---
    // 1. Mark downloaded AppImage as fully executable program bytes chmod +x
    QProcess chmod;
    chmod.start("chmod", QStringList() << "+x" << m_downloadedFilePath);
    chmod.waitForFinished();

    // 2. Launch new updated binary version instance independently
    QProcess::startDetached(m_downloadedFilePath, QStringList());
    QCoreApplication::quit();
#endif
}
