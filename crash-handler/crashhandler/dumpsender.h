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

#pragma once

#include <QHttpMultiPart>
#include <QObject>

class DumpSender : public QObject
{
    Q_OBJECT

public:
    explicit DumpSender(QUrl submitUrl, QObject *parent = nullptr);

    int dumperSize() const;

    void sendDumpAndQuit();
    void restartCrashedApplication();
    void restartCrashedApplicationAndSendDump();
    void setEmailAddress(const QString &email);
    void setCommentText(const QString &comment);

    signals:
        void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    QUrl m_submitUrl;
    QHttpMultiPart m_httpMultiPart;
    QByteArray m_formData;
    QString m_applicationFilePath;
    QString m_emailAddress;
    QString m_commentText;
};
