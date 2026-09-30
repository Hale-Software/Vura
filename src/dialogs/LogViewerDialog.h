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

#include <QDialog>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSettings>
#include <QCloseEvent>
#include <QScrollBar>
#include <QString>
#include <QList>

#include <libvura/logging/logger.h>


QT_BEGIN_NAMESPACE

namespace Ui {
class LogViewerDialog;
}

QT_END_NAMESPACE


struct LogEntry {
    QString timestamp;
    int level;
    QString component;
    QString message;
    QString fullText;
};

class LogViewerDialog : public QDialog {
    Q_OBJECT

public:
    explicit LogViewerDialog(QWidget *parent = nullptr);
    ~LogViewerDialog() override;

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void appendLogMessage(LogMessage message);

private slots:
    void onSearchTextChanged(const QString &text);
    void onFilterToggled();
    void onAlwaysOnTopToggled(bool checked);
    void onAutoScrollToggled(bool checked);
    void onStyleMessagesToggled(bool checked);
    void clearButton_Clicked();
    void exportButton_Clicked();

private:
    Ui::LogViewerDialog *ui;

    QList<LogEntry> m_logBuffer;
    bool m_autoScroll = true;
    bool m_styleMessages = true;

    void refreshLogView();
    void appendToView(const LogEntry &entry) const;
    void readSettings();
    void writeSettings() const;

};
