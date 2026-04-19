/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

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
#include <QTextBrowser>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageLogContext>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>
#include <QMessageBox>
#include <QList>

#include "../constants.h"

struct BloggerMessage
{
    QString timestamp;
    QString verbosity;
    QString message;
};


class Blogger : public QObject
{
    Q_OBJECT
public:
    static Blogger* instance(); // Singleton instance
    void clearLogFile();
    QString getLogFileName();
    QList<BloggerMessage> getMessages();
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

signals:
    void message(QString message);
    void newLogMessage(QString timestamp, QString verbosity, QString message);

private:
    Blogger(QObject* parent = nullptr);
    ~Blogger();
    QString m_logFileName;
    QFile m_logFile;
    QList<BloggerMessage> m_messages;

    void InitLogFile();
    bool deleteOldestLogFile(QDir logDir, QStringList logFiles);
    QString m_message(QtMsgType type, const QMessageLogContext& context, const QString& msg);

};
