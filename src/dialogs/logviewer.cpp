#include "logviewer.h"
#include "../forms/ui_LogViewer.h"

#include <config.h>


QString getVerbosityString(int verbosity)
{
    switch (verbosity) {
        case 0:
            return "[DEBUG]";
        case 1:
            return "[INFO]";
        case 2:
            return "[WARNING]";
        case 3:
            return "[ERROR]";
        case 4:
            return "[CRITICAL]";
        case 5:
            return "[FATAL]";
        default:
            return "";
    }
}

LogViewer::LogViewer(QWidget *parent) : QDialog(parent), ui(new Ui::LogViewer)
{
    ui->setupUi(this);

    QSettings settings;
    m_verbosity = settings.value("logViewerVerbosity", 1).toInt();
    ui->verbosityComboBox->setCurrentIndex(m_verbosity);

    connect(ui->openButton, &QPushButton::clicked, this, &LogViewer::openButton_Clicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &LogViewer::clearButton_Clicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &LogViewer::closeButton_Clicked);
    connect(ui->verbosityComboBox, &QComboBox::currentIndexChanged, this, &LogViewer::verbosityIndexChanged);
    connect(ui->simplify, &QCheckBox::checkStateChanged, this, &LogViewer::simplify_Clicked);

    Logger *logger = Logger::instance();
    connect(logger, &Logger::message, this, &LogViewer::message);
    m_currentLogFile = logger->getLogFileName();
    m_openedLogFile = m_currentLogFile;
    refreshMessages();
}

LogViewer::~LogViewer()
{
    delete ui;
}

void LogViewer::messageFormatter(QString message)
{
    if (m_simplify) {
        int timestampIn_pos = message.indexOf("[");
        int fileIn_pos = message.indexOf("[", timestampIn_pos + 1);
        int lineIn_pos = message.indexOf("[", fileIn_pos + 1);
        int functionIn_pos = message.indexOf("[", lineIn_pos + 1);
        int fileOut_pos = message.indexOf("]", fileIn_pos);
        int lineOut_pos = message.indexOf("]", lineIn_pos);
        int functionOut_pos = message.indexOf("]", functionIn_pos);

        message.remove(functionIn_pos - 1, functionOut_pos - functionIn_pos + 2);
        message.remove(lineIn_pos - 1, lineOut_pos - lineIn_pos + 2);
        message.remove(fileIn_pos - 1, fileOut_pos - fileIn_pos + 2);
    }

    // Message Verbosity Handler
    if (m_verbosity == 0) {
        ui->textArea->append(message);
    } else if (m_verbosity == 1) {
        if (!message.contains(getVerbosityString(0))) {
            ui->textArea->append(message);
        }
    } else if (m_verbosity == 2) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))) {
            ui->textArea->append(message);
        }
    } else if (m_verbosity == 3) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))
            && !message.contains(getVerbosityString(2))) {
            ui->textArea->append(message);
            }
    } else if (m_verbosity == 4) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))
            && !message.contains(getVerbosityString(2)) && !message.contains(getVerbosityString(3))) {
            ui->textArea->append(message);
            }
    } else if (m_verbosity == 5) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))
            && !message.contains(getVerbosityString(2)) && !message.contains(getVerbosityString(3))
            && !message.contains(getVerbosityString(4))) {
            ui->textArea->append(message);
            }
    } else {
        ui->textArea->append(message);
    }
}

void LogViewer::refreshMessages()
{
    ui->textArea->clear();
    QFile logFile(m_openedLogFile, this);
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        VuraErrorBox(this, "Error opening log file");
    } else {
        QTextStream t(&logFile);
        while (!t.atEnd()) {
            QString line = t.readLine();
            messageFormatter(line);
        }
    }
}

void LogViewer::message(QString message)
{
    if (m_openedLogFile == m_currentLogFile)
        messageFormatter(message);
}

void LogViewer::openButton_Clicked()
{
    // Get the log directory.
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

    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Log File"),
        logDirString,
        tr("Log Files (*.log);;All Files (*.*)"));

    if (!fileName.isEmpty()) {
        m_openedLogFile = fileName;
        refreshMessages();
    }
}

void LogViewer::clearButton_Clicked()
{
    QMessageBox::StandardButton confirmationBox;
    confirmationBox = QMessageBox::question(this, "Clear Logs", "Are you sure you want to clear the logs?\n This action cannot be undone.",
                                  QMessageBox::Yes | QMessageBox::No);

    if (confirmationBox == QMessageBox::Yes) {
        if (m_openedLogFile == m_currentLogFile) {
            Logger *logger = Logger::instance();
            logger->clearLogFile();
        } else {
            QFile f(m_openedLogFile);
            if (f.open(QFile::WriteOnly | QFile::Truncate)) {
                f.close();
            } else {
                VuraErrorBox(this, "Failed to clear log file.");
            }
        }
    }
}

void LogViewer::closeButton_Clicked()
{
    this->close();
}

void LogViewer::verbosityIndexChanged(int index)
{
    QSettings settings;
    settings.setValue("logViewerVerbosity", index);
    m_verbosity = index;
    refreshMessages();
}

void LogViewer::simplify_Clicked()
{
    m_simplify = ui->simplify->isChecked();
    refreshMessages();
}
