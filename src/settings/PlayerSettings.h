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

#include <QWidget>
#include <QSettings>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui {
class PlayerSettings;
}
QT_END_NAMESPACE

class PlayerSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerSettings(QWidget *parent = nullptr);
    ~PlayerSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

private slots:
    void allowOnlyOneInstance_Checked(int state);
    void showMediaChangeNotification_Changed(int state);
    void continueVideoPlayback_Changed(int state);
    void pausePlaybackWhenMinimized_Checked(int state);
    void pauseOnLastFrameOfVideo_Checked(int state);
    void useHardwareAcceleration_Checked(int state);
    void defaultVideoContrast_ValueChanged(int value);
    void defaultAspectRatio_Changed(int index);

private:
    Ui::PlayerSettings *ui;
    bool m_unsavedChanges = false;

};
