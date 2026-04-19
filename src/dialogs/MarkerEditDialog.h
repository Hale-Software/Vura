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
#include <QString>
#include <QStringList>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QTime>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

#include <data/video-markers.h>


QT_BEGIN_NAMESPACE
namespace Ui {
class MarkerEditDialog;
}
QT_END_NAMESPACE

class MarkerEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MarkerEditDialog(const VuraVideoMarker &videoMarker, int videoDuration, QWidget *parent = nullptr);
    ~MarkerEditDialog() override;
    void saveMarker();

signals:
    void markerEdited(const VuraVideoMarker &videoMarker);
    void markerDeleted(const VuraVideoMarker &videoMarker);
    void getNextMarker(const VuraVideoMarker &videoMarker);
    void getPrevMarker(const VuraVideoMarker &videoMarker);

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void forceClose();
    void loadVideoMarker(const VuraVideoMarker &videoMarker);
    void name_TextChanged(const QString &text);
    void timestamp_TextChanged(const QString &text);
    void type_IndexChanged(int index);
    void comments_TextChanged(const QString &text);
    void deleteButton_Clicked();
    void prevButton_Clicked();
    void nextButton_Clicked();
    void cancelButton_Clicked();
    void okButton_Clicked();

private:
    Ui::MarkerEditDialog *ui;
    VuraVideoMarker m_videoMarker;
    int m_videoDuration;
    bool m_unsavedChanges = false;
    bool m_forceClose = false;

    QString durationToTimestampString() const;
    void populateUI() const;

};
