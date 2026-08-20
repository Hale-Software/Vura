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
#include <QFile>
#include <QDir>
#include <QString>
#include <QList>
#include <QMutex>
#include <QMessageLogContext>

#include <iostream>

#include "categories.h"
#include "../models/log-message.h"


class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger* instance();
    void clearLogFile();
    QString getLogFileName() const;
    QList<LogMessage> getLogMessages() const;
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

signals:
    void newLogEntry(LogMessage message);

private:
    explicit Logger(QObject* parent = nullptr);
    ~Logger() override;

    void initLogFile();
    void writeSessionBanner();
    static void rotateLogs(const QDir& logDir, int maxLogs);
    static QString formatMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    static constexpr qint64 kMaxLogSizeBytes = 10LL * 1024 * 1024;

    QString m_logFileName;
    QFile m_logFile;
    QList<LogMessage> m_logMessages;
    mutable QMutex m_mutex;

};
