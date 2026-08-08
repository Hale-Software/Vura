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
class QSpinBox;
class QDoubleSpinBox;
class QDebug;


QT_BEGIN_NAMESPACE
namespace Ui {
class PlaybackSettings;
}
QT_END_NAMESPACE

class PlaybackSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PlaybackSettings(QWidget *parent = nullptr);
    ~PlaybackSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

private slots:
    void playbackSpeedAdjustment_ValueChanged(double value);
    void playbackSpeedAdjustmentFine_ValueChanged(double value);
    void volumeStep_ValueChanged(double value);
    void extraLargeJump_ValueChanged(int value);
    void largeJump_ValueChanged(int value);
    void mediumJump_ValueChanged(int value);
    void smallJump_ValueChanged(int value);
    void extraSmallJump_ValueChanged(int value);
    void jumpToEndPercentage_ValueChanged(int value);

private:
    Ui::PlaybackSettings *ui;
    bool m_unsavedChanges = false;

};
