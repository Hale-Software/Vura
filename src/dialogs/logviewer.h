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

#include <QDialog>
#include <QPushButton>
#include <QTableView>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QString>

#include <constants.h>
#include <models/log-viewer-model.h>
#include <util/blogger.h>
#include <util/messagebox.h>


QT_BEGIN_NAMESPACE

namespace Ui {
    class LogViewer;
}

QT_END_NAMESPACE


class LogViewer : public QDialog {
    Q_OBJECT

public:
    explicit LogViewer(QWidget *parent = nullptr);
    ~LogViewer() override;

    void openFile(const QString &fileName);

public slots:
    void message(const QString &message) const;
    void newLogMessage(const QString &timestamp, const QString &verbosity, const QString &msg);

private slots:
    void openButton_Clicked();
    void clearButton_Clicked();
    void closeButton_Clicked();
    void verbosityIndexChanged(int index);

private:
    Ui::LogViewer *ui;
    int m_verbosity = 0;
    QString m_currentLogFile;
    QString m_openedLogFile;
    LogViewerModel *m_logViewerModel;

    void refreshMessages();
    void messageFormatter(const QString &message) const;

};
