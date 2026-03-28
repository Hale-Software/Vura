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

#include "../constants.h"


class Blog : public QObject
{
    Q_OBJECT
public:
    static Blog* instance(); // Singleton instance
    void clearLogFile();
    QString getLogFileName();
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    signals:
        void message(QString message);

private:
    Blog(QObject* parent = nullptr);
    ~Blog();
    QString m_logFileName;
    QFile m_logFile;

    void InitLogFile();
    bool deleteOldestLogFile(QDir logDir, QStringList logFiles);
    QString m_message(QtMsgType type, const QMessageLogContext& context, const QString& msg);

};
