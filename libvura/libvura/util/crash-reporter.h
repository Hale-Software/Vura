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

#pragma once

#include <QObject>
#include <QDir>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QDebug>


class CrashReporter : public QObject
{
    Q_OBJECT

public:
    explicit CrashReporter(QObject *parent = nullptr);

    void checkForPreviousCrashes();

    signals:
        void scanFinished(bool crashFileExists);
    void uploadStarted();
    void finished(bool success, const QString& message);

public slots:
    void uploadCrashFile(bool uploadFile);

private slots:
    void replyFinished();

private:
    void cleanupCrashFiles();
    QString m_crashFile = "";
    QFileInfoList m_crashFileList;
    QHttpMultiPart *m_multiPart = nullptr;
    QNetworkAccessManager *m_manager = nullptr;
    QNetworkReply *m_reply = nullptr;

};
