/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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

#include "dumpsender.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPair>
#include <QProcess>
#include <QStringList>
#include <QTemporaryFile>
#include <QUrl>

static QString getApplicationArguments(int idx)
{
	if (QCoreApplication::arguments().size() > idx)
		return QCoreApplication::arguments().at(idx);

	return "";
}

DumpSender::DumpSender(QUrl submitUrl, QObject *parent) :
    m_submitUrl(submitUrl), QObject(parent),
    m_httpMultiPart(QHttpMultiPart::FormDataType)
{
    const QString dumpPath = getApplicationArguments(1);
    const QByteArray startupTime = getApplicationArguments(2).toLocal8Bit();
    const QByteArray applicationName = getApplicationArguments(3).toLocal8Bit();
    QByteArray applicationVersion = getApplicationArguments(4).toLocal8Bit();
    const QByteArray plugins = getApplicationArguments(5).toLocal8Bit();
    // QByteArray ideRevision = getApplicationArguments(6).toLocal8Bit();
    m_applicationFilePath = getApplicationArguments(7);

    if (applicationVersion.isEmpty())
        applicationVersion = "1.0.0";

    QFile dumpFile(dumpPath, this);
    const bool isOpen = dumpFile.open(QIODevice::ReadOnly);
    //Q_ASSERT(isOpen);
    Q_UNUSED(isOpen);

    const QList<QPair<QByteArray, QByteArray> > pairList = {
        { "StartupTime", startupTime },
        { "Vendor", "Qt Project" },
        { "InstallTime", "0" },
        { "Add-ons", plugins },
        { "BuildID", "" },
        { "SecondsSinceLastCrash", "0" },
        { "ProductName", applicationName },
        { "URL", "" },
        { "Theme", "" },
        { "Version", applicationVersion },
        { "CrashTime", QByteArray::number(QDateTime::currentDateTime().toTime_t()) },
        { "Throttleable", "0" }
    };

    const QByteArray boundary = m_httpMultiPart.boundary();
    m_formData.append("--" + boundary + "\r\n");
    for (const auto &pair : pairList) {
        m_formData.append("Content-Disposition: form-data; name=\"" + pair.first + "\"\r\n\r\n");
        m_formData.append(pair.second + "\r\n");
        m_formData.append("--" + boundary + "\r\n");
    }


    QByteArray dumpArray = dumpFile.readAll();
    m_formData.append("Content-Type: application/octet-stream\r\n");
    m_formData.append("Content-Disposition: form-data; name=\"upload_file_minidump\"; filename=\""
                      + QFileInfo(dumpPath).baseName().toUtf8() + "\r\n");
    m_formData.append("Content-Transfer-Encoding: binary\r\n\r\n");
    m_formData.append(dumpArray);

    m_formData.append("--" + boundary + "--\r\n");

    for (const auto &pair : pairList) {
        QHttpPart httpPart;
        httpPart.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"" + pair.first + "\"");
        httpPart.setBody(pair.second);
        m_httpMultiPart.append(httpPart);
    }

    QHttpPart dumpPart;
    dumpPart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    dumpPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       "form-data; name=\"upload_file_minidump\"; filename=\""
                       + QFileInfo(dumpPath).baseName().toUtf8() + "\"");
    dumpPart.setRawHeader("Content-Transfer-Encoding:", "binary");
    dumpPart.setBody(dumpArray);
    m_httpMultiPart.append(dumpPart);
}

void DumpSender::sendDumpAndQuit()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest request(m_submitUrl);

    const QByteArray boundary = m_httpMultiPart.boundary();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundary);

    QList<QPair<QByteArray, QByteArray>> pairList;

    if (!m_emailAddress.isEmpty())
        pairList.append({ "Email", m_emailAddress.toLocal8Bit() });

    if (!m_commentText.isEmpty())
        pairList.append({ "Comments", m_commentText.toLocal8Bit() });

    for (const auto &pair : pairList) {
        m_formData.append("Content-Disposition: form-data; name=\"" + pair.first + "\"\r\n\r\n");
        m_formData.append(pair.second + "\r\n");
        m_formData.append("--" + boundary + "\r\n");
    }

    for (const auto &pair : pairList) {
        QHttpPart httpPart;
        httpPart.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"" + pair.first + "\"");
        httpPart.setBody(pair.second);
        m_httpMultiPart.append(httpPart);
    }

    QNetworkReply *reply = manager->post(request, &m_httpMultiPart);

    m_httpMultiPart.setParent(reply);

    connect(reply, &QNetworkReply::uploadProgress, this, &DumpSender::uploadProgress);
    connect(reply, &QNetworkReply::finished, QCoreApplication::instance(), &QCoreApplication::quit);
    connect(reply,
            static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            QCoreApplication::instance(), &QCoreApplication::quit);
}

void DumpSender::restartCrashedApplicationAndSendDump()
{
    QProcess::startDetached(m_applicationFilePath);
    sendDumpAndQuit();
}

void DumpSender::restartCrashedApplication()
{
    QProcess::startDetached(m_applicationFilePath);
    QCoreApplication::quit();
}

void DumpSender::setEmailAddress(const QString &email)
{
    m_emailAddress = email;
}

void DumpSender::setCommentText(const QString &comment)
{
    m_commentText = comment;
}

int DumpSender::dumperSize() const
{
    return m_formData.size();
}
