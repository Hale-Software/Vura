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
#include <QCloseEvent>

#include <libvura/models/video-marker-record.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MarkerEditDialog; }
QT_END_NAMESPACE


class MarkerEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MarkerEditDialog(const VideoMarkerRecord &videoMarker, int videoDuration, QWidget *parent = nullptr);
    ~MarkerEditDialog() override;

    void saveMarker();

signals:
    void markerEdited(const VideoMarkerRecord &videoMarker);
    void markerDeleted(const VideoMarkerRecord &videoMarker);

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void forceClose();
    void loadVideoMarker(const VideoMarkerRecord &videoMarker);
    void name_TextChanged(const QString &text);
    void timestamp_TextChanged(const QString &text);
    void type_IndexChanged(int index);
    void comments_TextChanged();
    void deleteButton_Clicked();
    void cancelButton_Clicked();
    void okButton_Clicked();

private:
    QString durationToTimestampString() const;
    void populateUI();

    Ui::MarkerEditDialog *ui;
    VideoMarkerRecord m_videoMarker;
    int m_videoDuration;
    bool m_unsavedChanges = false;
    bool m_forceClose = false;

};
