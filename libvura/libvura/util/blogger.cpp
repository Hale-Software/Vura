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

#include "blogger.h"
#include <libvura/config.h>

#include <QSettings>
#include <QStandardPaths>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <iostream>


Blogger::Blogger(QObject* parent) : QObject(parent)
{
    QSettings settings;
    if (settings.value("logToFile", true).toBool()) {
        initLogFile();
    }
}

Blogger::~Blogger()
{
    if (m_logFile.isOpen())
        m_logFile.close();
}

Blogger* Blogger::instance()
{
    static Blogger instance;
    return &instance;
}

QString Blogger::getLogFileName() const
{
    return m_logFileName;
}

void Blogger::initLogFile()
{
    QSettings settings;
    int maxLogs = settings.value("maxLogFiles", 10).toInt();

    QString baseDir = (QString(VURA_BUILD_TYPE) == "Debug") ? "debug" :
                        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir logDir(baseDir + "/logs");

    if (!logDir.exists() && !logDir.mkpath(".")) {
        qCritical() << "Failed to create log directory at " << logDir.absolutePath();
        return;
    }

    rotateLogs(logDir, maxLogs);

    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss") + ".log";
    m_logFileName = logDir.filePath(fileName);
    m_logFile.setFileName(m_logFileName);

    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qCritical() << "Failed to open log file at " << m_logFile.fileName() << ". Error: " << m_logFile.errorString();
    }
}

void Blogger::rotateLogs(const QDir &logDir, int maxLogs)
{
    QFileInfoList logFiles = logDir.entryInfoList(QDir::Files, QDir::Time);

    while (logFiles.count() >= maxLogs) {
        QFile::remove(logFiles.last().absoluteFilePath());
        logFiles.removeLast();
    }
}

void Blogger::clearLogFile()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }

    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qCritical() << "Failed to clear log file at " << m_logFile.fileName() << ". Error: " << m_logFile.errorString();
    }
}

QString Blogger::formatMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QString typeStr;
    switch (type) {
        case QtDebugMsg:    typeStr = "[DEBUG]"; break;
        case QtInfoMsg:     typeStr = "[INFO]"; break;
        case QtWarningMsg:  typeStr = "[WARN]"; break;
        case QtCriticalMsg: typeStr = "[CRITICAL]"; break;
        case QtFatalMsg:    typeStr = "[FATAL]"; break;
    }

    return QString("[%1] [%2] [%3] [%4] %5 - %6")
        .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"))
        .arg(context.file ? context.file : "unknown")
        .arg(context.line)
        .arg(context.function ? context.function : "unknown")
        .arg(typeStr, msg);
}


void Blogger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    Blogger* logger = Blogger::instance();
    QString output = logger->formatMessage(type, context, msg);

    if (logger->m_logFile.isOpen()) {
        logger->m_logFile.write((output + "\n").toUtf8());
        logger->m_logFile.flush();
    }

    emit logger->message(output);

    std::cerr << output.toStdString() << std::endl;
}
