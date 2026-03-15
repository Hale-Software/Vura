#include "logger.h"

#include <config.h>


Logger::Logger(QObject* parent) : QObject(parent) {
    QSettings settings;
    if (settings.value("logToFile", true).toBool()) {
        InitLogFile();
    }
}

Logger::~Logger() {
    m_logFile.close();
}

void Logger::InitLogFile()
{
    QSettings settings;

    // Get max log count set by user.
    int maxLogs = settings.value("maxLogFiles", 10).toInt();

    // Set the log directory.
    QString logDirString = "logs";
    QString appDataDirString = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (VURA_BUILD_TYPE == "Debug") {
        appDataDirString = "C:/Users/halea/vura-debug";
    }
    QDir appDataDir(appDataDirString);
    if (appDataDir.exists()) {
        QDir appLogDir(appDataDirString + "/logs");
        if (appLogDir.exists()) {
            logDirString = appDataDirString + "/logs";
        } else {
            if (appLogDir.mkdir(".")) {
                logDirString = appDataDirString + "/logs";
            }
        }
    } else {
        if (appDataDir.mkdir(".")) {
            QDir appLogDir(appDataDirString + "/logs");
            if (appLogDir.exists()) {
                logDirString = appDataDirString + "/logs";
            } else {
                if (appLogDir.mkdir(".")) {
                    logDirString = appDataDirString + "/logs";
                }
            }
        }
    }
    QDir logDir(logDirString);

    // Scan file directory and create filename list.
    QStringList logFiles = logDir.entryList(QDir::Files);

    // If log files found is over max log count delete oldest files until limit reached.
    while (logFiles.count() >= maxLogs) {
        deleteOldestLogFile(logDir, logFiles);
        logFiles = logDir.entryList(QDir::Files);
    }

    // Create new log file.
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    QString logfileName = logDir.filePath(fileName+".log");
    m_logFileName = logfileName;
    m_logFile.setFileName(logfileName);
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

bool Logger::deleteOldestLogFile(QDir logDir, QStringList logFiles)
{
    // Oldest filename found.
    QString oldestFilename;
    for (const QString& logFile : logFiles) {
        // Convert to QDateTime for filename comparison.
        QString fileDateTime = logFile.chopped(4);
        QDateTime fileDate = QDateTime::fromString(fileDateTime, "yyyy-MM-dd HH-mm-ss");
        if (fileDate.isValid()) {
            // Set oldest filename to current filename if none have been set yet.
            if (oldestFilename.isEmpty()) {
                oldestFilename = logFile;
            } else {
                // Convert to QDateTime for oldest filename comparison.
                QString oldestFileDateTime = oldestFilename.chopped(4);
                QDateTime oldestFileDate = QDateTime::fromString(oldestFileDateTime, "yyyy-MM-dd HH-mm-ss");
                if (oldestFileDate.isValid()) {
                    // If current filename is older than current oldest replace.
                    if (oldestFileDate > fileDate) {
                        oldestFilename = logFile;
                    }
                }
            }
        }
    }

    // If there is a valid oldest filename found.
    if (!oldestFilename.isEmpty()) {
        // Create full filepath for file to delete.
        QString fileToDelete = logDir.filePath(oldestFilename);

        // If file is successfully deleted return true.
        if (QFile::remove(fileToDelete)) {
            return true;
        }
    }

    // If all else fails return false.
    return false;
}

Logger* Logger::instance() {
    static Logger instance;
    return &instance;
}

QString Logger::getLogFileName()
{
    return m_logFileName;
}

void Logger::clearLogFile()
{
    m_logFile.close();
    QFile f(m_logFileName);
    if (f.open(QFile::WriteOnly | QFile::Truncate))
    {
        f.close();
    }
    m_logFile.setFileName(m_logFileName);
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

QString Logger::m_message(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    QByteArray localMsg = msg.toLocal8Bit();
    QString output;
    QTextStream stream(&output);

    stream << QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
    stream << "[" << context.file << "] ";
    stream << "[" << context.line << "] ";
    stream << "[" << context.function << "] ";

    // Add message type prefix
    switch (type) {
        case QtDebugMsg:
            stream << "[DEBUG] - ";
            break;
        case QtInfoMsg:
            stream << "[INFO] - ";
            break;
        case QtWarningMsg:
            stream << "[WARN] - ";
            break;
        case QtCriticalMsg:
            stream << "[CRITICAL] - ";
            break;
        case QtFatalMsg:
            stream << "[FATAL] - ";
            break;
    }
    stream << msg;  // Add the actual message content

    return output;
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QSettings settings;

    QString output = Logger::instance()->m_message(type, context, msg);

    // If log to file setting is on, write log to file.
    if (settings.value("logToFile", true).toBool()) {
        if (Logger::instance()->m_logFile.isOpen()) {
            QString formattedOutput = output + "\n";
             Logger::instance()->m_logFile.write(formattedOutput.toUtf8());
             Logger::instance()->m_logFile.flush(); // Ensure immediate writing
        }
    }

    // Emit new log message
    emit Logger::instance()->message(output);

    // Also output to the standard console for development visibility
    fprintf(stderr, "%s", output.toLocal8Bit().constData());
}
