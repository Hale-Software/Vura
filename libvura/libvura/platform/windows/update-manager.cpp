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

#include "update-manager.h"

#include <QSettings>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QVector>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>

#include <libvura/config.h>


UpdateManager::UpdateManager(QObject *parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this))
{

}

UpdateManager::~UpdateManager() {}

void UpdateManager::checkForUpdates()
{
    QSettings settings;

    QString manifestFile = "stable.json";
    int updateBranch = settings.value("updateBranch", 0).toInt();
    if (updateBranch == 1) {
        manifestFile = "beta.json";
        m_isBeta = true;
    }

    connect(m_networkManager, &QNetworkAccessManager::finished, this, &UpdateManager::updateCheckReplyFinished);

    QUrl url(QString("https://vura.hale-software.com/%1").arg(manifestFile));
    QNetworkRequest request(url);

    qDebug() << "Checking for updates using URL: " << url << "...";
    m_networkManager->get(request);
}

void UpdateManager::downloadUpdate()
{
    //QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString tempDir = "C:\\Users\\halea\\vura-update";

    for (FileEntry entry : m_files) {
        QString expectedHash = entry.hash.toLower().trimmed();
        QString downloadUrlString = QString("%1/%2").arg(m_rootUrl).arg(entry.name);
        QUrl downloadUrl(downloadUrlString);

        QString fileName = downloadUrl.fileName();
        if (fileName.isEmpty())
            fileName = entry.name;

        entry.downloadedFilePath = tempDir + QDir::separator() + fileName;

        QFile tempFile;
        tempFile.setFileName(entry.downloadedFilePath);
        if (!tempFile.open(QIODevice::WriteOnly)) {
            qWarning() << "Failed to open local temporary file for downloading.";
            continue;
        }

        QNetworkRequest request(downloadUrl);
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply *reply = m_networkManager->get(request);

        connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesReceived, qint64 bytesTotal) {
            emit downloadProgress(bytesReceived, bytesTotal);
        });
        connect(reply, &QNetworkReply::readyRead, this, [this, &tempFile, reply]() {
            if (reply)
                tempFile.write(reply->readAll());
        });
        connect(reply, &QNetworkReply::finished, this, [this, &tempFile, reply, entry, expectedHash]() {
            tempFile.close();
            if (reply->error() != QNetworkReply::NoError) {
                emit errorOccurred(reply->errorString());
                tempFile.remove();
                reply->deleteLater();
                return;
            }

            if (!verifyFileIntegrity(entry.downloadedFilePath, expectedHash)) {
                tempFile.remove();
            }
            reply->deleteLater();
        });
    }

    emit downloadFinished(true, "");
}

void UpdateManager::updateCheckReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        return;
    }

    QString error;
    QByteArray response = reply->readAll();
    if (isPlatformUpdateAvailable(response, &error)) {
        if (error.isEmpty()) {
            emit updateAvailable(true);
        } else {
            emit errorOccurred(error);
        }
    } else {
        if (!error.isEmpty())
            emit errorOccurred(error);
    }
}

bool UpdateManager::isPlatformUpdateAvailable(QByteArray &replyData, QString *error)
{
    QJsonParseError pe{};

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyData, &pe);
    if (pe.error != QJsonParseError::NoError) {
        if (error)
            *error = pe.errorString();
        return false;
    }

    QJsonObject root = jsonDoc.object();

    bool isRemoteBeta = root["is_beta"].toBool();
    QString remoteVersion = root["version"].toString();
    QString releaseDate = root["release_date"].toString();
    QString changelogUrl = root["changelog_url"].toString();

    QSettings settings;
    QString lastCheckedVersion = settings.value("lastCheckedVersion", "").toString();
    if (remoteVersion == lastCheckedVersion)
        return false;

    if (remoteVersion != VURA_VERSION_STRING) {
        QJsonValue platforms = root.value("platforms");
        if (!platforms.isObject()) {
            if (error)
                *error = "missing 'platforms' in response";
            return false;
        }

        QJsonValue windows = platforms.toObject().value("windows");
        if (!windows.isArray()) {
            if (error)
                *error = "missing 'windows' in platforms";
            return false;
        }

        QString releaseType = m_isBeta ? "beta" : "stable";
        m_rootUrl = QString("https://vura.hale-software.com/releases/%1/%2").arg(releaseType).arg(remoteVersion);

        for (const QJsonValue &v : windows.toArray()) {
            const QJsonObject o = v.toObject();
            FileEntry e;
            e.name = o.value("name").toString();
            e.hash = o.value("hash").toString();
            e.size = static_cast<qint64>(o.value("size").toDouble());
            m_files.append(e);
        }

        if (m_files.count() > 0)
            return true;
    }

    return false;
}

bool UpdateManager::verifyFileIntegrity(const QString &filePath, const QString &expectedHash)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QCryptographicHash hasher(QCryptographicHash::Sha256);
    char buffer[4096];
    qint64 bytesRead;

    while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0)
        hasher.addData(buffer, bytesRead);

    file.close();

    QString calculatedHash = hasher.result().toHex().toLower();

    return (calculatedHash == expectedHash);
}
