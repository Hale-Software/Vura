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

#include "CrashReporter.h"
#include <libvura/config.h>


CrashReporter::CrashReporter(QObject *parent) : QObject(parent) {}

void CrashReporter::checkForPreviousCrashes()
{
    qDebug() << "Checking for previous application crashes...";

    const QString crashDir = (QString(VURA_BUILD_TYPE) == "Debug") ? "debug/crashes"
        : QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/crashes";

    const QDir dir(crashDir);
    if (!dir.exists()) {
        qDebug() << "Crash file directory doesn't exist. Returning.";
        emit scanFinished(false);
        return;
    }

    // Look for minidumps or text logs
    QStringList filters;
    filters << "*.dmp" << "*.log";
    m_crashFileList = dir.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);

    if (m_crashFileList.isEmpty()) {
        qDebug() << "No crash files found in crash directory. Returning.";
        emit scanFinished(false);
        return;
    }
    qDebug() << QString::number(m_crashFileList.count()) << " crash files found in crash directory.";

    m_crashFile = m_crashFileList.first().absoluteFilePath();
    emit scanFinished(true);
}

void CrashReporter::uploadCrashFile(const bool uploadFile)
{
    if (uploadFile) {
        qDebug() << "Uploading crash file: " << m_crashFile;

        const QFileInfo fileInfo(m_crashFile);
        const QString fileName = fileInfo.fileName();

        const auto file = new QFile(m_crashFile);
        if (!file->open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to read data from crash file: " << m_crashFile;
            emit finished(false, "Failed to read data from crash file: " + m_crashFile);
            delete file;
            return;
        }

        m_multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                    QVariant(QString("form-data; name=\"crashfile\"; filename=\"%1\"").arg(fileName)));
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
        filePart.setBodyDevice(file);

        file->setParent(m_multiPart);
        m_multiPart->append(filePart);

        // Server endpoint to send crash files
        const QUrl url("https://website.com/api/crash-report");
        const QNetworkRequest request(url);

        m_manager = new QNetworkAccessManager();
        m_reply = m_manager->post(request, m_multiPart);

        m_multiPart->setParent(m_reply);

        connect(m_reply, &QNetworkReply::finished, this, &CrashReporter::replyFinished);
    } else {
        cleanupCrashFiles();
    }
}

void CrashReporter::replyFinished()
{
    qDebug() << "Crash file upload finished.";

    if (m_reply->error() == QNetworkReply::NoError) {
        qDebug() << "Crash file upload successful.";
        emit finished(true, "Crash file uploaded successfully!");
    } else {
        qDebug() << "Failed to send crash report. Error: " << m_reply->errorString();
        emit finished(false, "Failed to send crash report. Error: " + m_reply->errorString());
    }
    m_reply->deleteLater();
    m_manager->deleteLater();
    cleanupCrashFiles();
}

void CrashReporter::cleanupCrashFiles()
{
    qDebug() << "Removing all crash files...";

    for (const QFileInfo& file : m_crashFileList) {
        if (!QFile::remove(file.absoluteFilePath())) {
            qWarning() << "Failed to remove crash file located at: " << file.absoluteFilePath();
        } else {
            qDebug() << "Removed crash file located at: " << file.absoluteFilePath();
        }
    }

    qDebug() << "Successfully removed all crash files!";
}
