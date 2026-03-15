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


class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger* instance(); // Singleton instance
    void clearLogFile();
    QString getLogFileName();
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

signals:
    void message(QString message);

private:
    Logger(QObject* parent = nullptr);
    ~Logger();
    QString m_logFileName;
    QFile m_logFile;

    void InitLogFile();
    bool deleteOldestLogFile(QDir logDir, QStringList logFiles);
    QString m_message(QtMsgType type, const QMessageLogContext& context, const QString& msg);

};
