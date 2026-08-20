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

#include <QSettings>
#include <QStandardPaths>
#include <QDebug>

#include "logger.h"
#include <libvura/config.h>


Logger::Logger(QObject* parent) : QObject(parent)
{
    const QSettings settings;
    if (settings.value("logToFile", true).toBool())
        initLogFile();
}

Logger::~Logger()
{
    if (m_logFile.isOpen())
        m_logFile.close();
}

Logger* Logger::instance()
{
    static Logger instance;
    return &instance;
}

QString Logger::getLogFileName() const
{
    return m_logFileName;
}

void Logger::initLogFile()
{
    const QSettings settings;
    const int maxLogs = settings.value("maxLogFiles", 10).toInt();

    const QString baseDir = (QString(VURA_BUILD_TYPE) == "Debug") ? "debug" :
                        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QDir logDir(baseDir + "/logs");

    if (!logDir.exists() && !logDir.mkpath(".")) {
        qCritical() << "Failed to create log directory at " << logDir.absolutePath();
        return;
    }

    rotateLogs(logDir, maxLogs);

    const QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss") + ".log";
    m_logFileName = logDir.filePath(fileName);
    m_logFile.setFileName(m_logFileName);

    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qCritical() << "Failed to open log file at " << m_logFile.fileName() << ". Error: " << m_logFile.errorString();
        return;
    }

    writeSessionBanner();
}

void Logger::writeSessionBanner()
{
    const QString banner = QStringLiteral("=== Vura %1 (%2 build %3) - log file opened %4 ===")
        .arg(QString(VURA_VERSION_STRING),
             QString(VURA_BUILD_TYPE),
             QString(VURA_BUILD_NUMBER),
             QDateTime::currentDateTime().toString(Qt::ISODate));

    m_logFile.write((banner + "\n").toUtf8());
    m_logFile.flush();

    LogMessage bannerMessage;
    bannerMessage.timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    bannerMessage.level = LogLevel::Info;
    bannerMessage.component = QStringLiteral("Session");
    bannerMessage.message = banner;
    bannerMessage.fullText = banner;
    m_logMessages.append(bannerMessage);
}

void Logger::rotateLogs(const QDir &logDir, const int maxLogs)
{
    QFileInfoList logFiles = logDir.entryInfoList(QDir::Files, QDir::Time);

    while (logFiles.count() >= maxLogs) {
        QFile::remove(logFiles.last().absoluteFilePath());
        logFiles.removeLast();
    }
}

void Logger::clearLogFile()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }

    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qCritical() << "Failed to clear log file at " << m_logFile.fileName() << ". Error: " << m_logFile.errorString();
    }
}

QString Logger::formatMessage(const QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
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

QList<LogMessage> Logger::getLogMessages() const
{
    QMutexLocker locker(&m_mutex);
    return m_logMessages;
}


void Logger::messageHandler(const QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Logger* logger = instance();
    const QString output = formatMessage(type, context, msg);

    LogLevel typeInt;
    switch (type) {
        case QtDebugMsg:    typeInt = LogLevel::Debug; break;
        case QtInfoMsg:     typeInt = LogLevel::Info; break;
        case QtWarningMsg:  typeInt = LogLevel::Warning; break;
        case QtCriticalMsg: typeInt = LogLevel::Error; break;
        case QtFatalMsg:    typeInt = LogLevel::Fatal; break;
        default:            typeInt = LogLevel::Debug; break;
    }

    LogMessage newMessage;
    newMessage.timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    newMessage.level = typeInt;
    newMessage.component = context.category ? context.category : "unknown";
    newMessage.message = msg;
    newMessage.fullText = output;

    {
        QMutexLocker locker(&logger->m_mutex);

        if (logger->m_logFile.isOpen()) {
            logger->m_logFile.write((output + "\n").toUtf8());
            logger->m_logFile.flush();

            if (logger->m_logFile.size() > kMaxLogSizeBytes) {
                logger->m_logFile.close();
                logger->initLogFile();
            }
        }

        logger->m_logMessages.append(newMessage);
    }

    emit logger->newLogEntry(newMessage);
}
