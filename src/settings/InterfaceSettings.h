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
class QComboBox;
class QCheckBox;
class QSpinBox;
class QColorDialog;
class QPushButton;
class QDebug;


QT_BEGIN_NAMESPACE
namespace Ui {
class InterfaceSettings;
}
QT_END_NAMESPACE

class InterfaceSettings : public QWidget
{
    Q_OBJECT

public:
    explicit InterfaceSettings(QWidget *parent = nullptr);
    ~InterfaceSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

private slots:
    void currentTheme_Changed(int index);
    void showMaximizedOnStart_Changed(int index);
    void rememberWindowSize_Checked(int state);
    void showPlaylistOnStart_Checked(int state);
    void showVideoControlsOnStart_Checked(int state);
    void showStatusBarOnStart_Checked(int state);
    void autohideSlider_Checked(int state);
    void unhideSliderOnHotkey_Checked(int state);
    void sliderAutohideTimer_ValueChanged(int value);
    void sliderHeight_ValueChanged(int value);
    void videoMarkerHeight_ValueChanged(int value);
    void videoMarkerWidth_ValueChanged(int value);
    void videoMarkerSideLength_ValueChanged(int value);
    void videoMarkerVerticalMargin_ValueChanged(int value);
    void sliderLeftRightMargin_ValueChanged(int value);
    void sliderHandleRadius_ValueChanged(int value);
    void sliderPadding_ValueChanged(int value);
    void emptySliderColor_TextChanged(const QString &text);
    void emptySliderColorBrowse_Clicked();
    void stripMarkerColor_TextChanged(const QString &text);
    void stripMarkerColorBrowse_Clicked();
    void dialogMarkerColor_TextChanged(const QString &text);
    void dialogMarkerColorBrowse_Clicked();
    void cyanMarkerColor_TextChanged(const QString &text);
    void cyanMarkerColorBrowse_Clicked();
    void magentaMarkerColor_TextChanged(const QString &text);
    void magentaMarkerColorBrowse_Clicked();
    void orangeMarkerColor_TextChanged(const QString &text);
    void orangeMarkerColorBrowse_Clicked();
    void fullSliderColor_TextChanged(const QString &text);
    void fullSliderColorBrowse_Clicked();
    void caretColor_TextChanged(const QString &text);
    void caretColorBrowse_Clicked();
    void backgroundTrackColor_TextChanged(const QString &text);
    void backgroundTrackColorBrowse_Clicked();
    void progressFillColor_TextChanged(const QString &text);
    void progressFillColorBrowse_Clicked();
    void handlePlayheadColor_TextChanged(const QString &text);
    void handlePlayheadColorBrowse_Clicked();
    void markerColor_TextChanged(const QString &text);
    void markerColorBrowse_Clicked();
    void sceneMarkerColor_TextChanged(const QString &text);
    void sceneMarkerColorBrowse_Clicked();
    void cumshotMarkerColor_TextChanged(const QString &text);
    void cumshotMarkerColorBrowse_Clicked();

private:
    Ui::InterfaceSettings *ui;
    bool m_unsavedChanges = false;

};
