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

class QSettings;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QDebug;


QT_BEGIN_NAMESPACE
namespace Ui {
class AdvancedSettings;
}
QT_END_NAMESPACE

class AdvancedSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedSettings(QWidget *parent = nullptr);
    ~AdvancedSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

private slots:
    void logToFile_StateChanged(int state);
    void maxLogFiles_ValueChanged(int value);
    void markerProximityThreshold_ValueChanged(double value);
    void playbackSpeedMax_ValueChanged(double value);
    void defaultWindowHeight_ValueChanged(int value);
    void defaultWindowWidth_ValueChanged(int value);
    void continuePlaybackBannerTime_ValueChanged(int value);

private:
    Ui::AdvancedSettings *ui;

    bool m_unsavedChanges = false;

};
