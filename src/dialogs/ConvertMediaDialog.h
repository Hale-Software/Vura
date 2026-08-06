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
#include <QProgressBar>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include <libvura/io/remuxer-thread.h>
#include <libvura/io/video-converter.h>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ConvertMediaDialog;
}
QT_END_NAMESPACE

class ConvertMediaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvertMediaDialog(QWidget *parent = nullptr, const QString &initialOption = "Convert/Save");
    ~ConvertMediaDialog() override;

public slots:
    void progressUpdated(int percentage);
    void conversionFinished(bool success, const QString &message);

private slots:
    void fileBrowse_Clicked();
    void convert_Clicked();

private:
    Ui::ConvertMediaDialog *ui;
    QString m_initialOption = "Convert/Save";
    RemuxerThread *m_remuxerThread = nullptr;
    VideoConverter *m_converter = nullptr;

    void convertFile(QString outFile);
    void convertNetwork(QString outFile);

};
