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

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

#include <libvura/media-engine/media-engine.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class TestWindow;
}
QT_END_NAMESPACE


class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestWindow(QWidget *parent = nullptr);
    ~TestWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void errorOccurred(const QString& errorMessage);

private slots:
    void actionOpenFile();
    void previousButtonClicked();
    void jumpBackwardsButtonClicked();
    void jumpForwardButtonClicked();
    void nextButtonClicked();

private:
    Ui::TestWindow *ui;
    VuraMediaEngine *m_mediaPlayer = nullptr;
    int m_position = 0;
    int m_duration = 0;

    static QString timestampString(qint64 timestampMs);
    static int qint64ToInt(qint64 value);

};
