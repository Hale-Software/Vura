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

#include "InterfaceSettings.h"
#include "ui_InterfaceSettings.h"


InterfaceSettings::InterfaceSettings(QWidget *parent) : QWidget(parent), ui(new Ui::InterfaceSettings)
{
    ui->setupUi(this);

    const QSettings settings;
    ui->currentTheme->setCurrentIndex(settings.value("theme", 0).toInt());
    ui->rememberWindowSize->setChecked(settings.value("rememberWindowSize", false).toBool());
    ui->showPlaylistOnStart->setChecked(settings.value("showPlaylistOnStart", true).toBool());
    ui->showVideoControlsOnStart->setChecked(settings.value("showVideoControlsOnStart", false).toBool());
    ui->showStatusBarOnStart->setChecked(settings.value("showStatusBarOnStart", false).toBool());
    ui->autohideSlider->setChecked(settings.value("autohideSlider", true).toBool());
    ui->unhideSliderOnHotkey->setChecked(settings.value("unhideSliderOnHotkey", true).toBool());
    ui->sliderAutohideTimer->setValue(settings.value("sliderAutohideTime", 5).toInt());
    ui->sliderHeight->setValue(settings.value("sliderHeight", 6).toInt());
    ui->videoMarkerHeight->setValue(settings.value("videoMarkerHeight", 10).toInt());
    ui->videoMarkerWidth->setValue(settings.value("videoMarkerWidth", 2).toInt());
    ui->videoMarkerSideLength->setValue(settings.value("videoMarkerSideLength", 14).toInt());
    ui->videoMarkerVerticalMargin->setValue(settings.value("videoMarkerVerticalMargin", 0).toInt());
    ui->sliderLeftRightMargin->setValue(settings.value("sliderLeftRightMargin", 1).toInt());
    ui->sliderHandleRadius->setValue(settings.value("sliderHandleRadius", 8).toInt());
    ui->sliderPadding->setValue(settings.value("sliderPadding", 2).toInt());
    ui->emptySliderColor->setText(settings.value("emptySliderColor", "#000000").toString());
    ui->fullSliderColor->setText(settings.value("fullSliderColor", "#00d9ff").toString());
    ui->caretColor->setText(settings.value("caretColor", "#ffffff").toString());
    ui->backgroundTrackColor->setText(settings.value("backgroundTrackColor", "#323232").toString());
    ui->progressFillColor->setText(settings.value("progressFillColor", "#0076d7").toString());
    ui->handlePlayheadColor->setText(settings.value("handlePlayheadColor", "#ffffff").toString());
    ui->markerColor->setText(settings.value("markerColor", "#03c200").toString());
    ui->sceneMarkerColor->setText(settings.value("sceneMarkerColor", "#000eab").toString());
    ui->cumshotMarkerColor->setText(settings.value("cumshotMarkerColor", "#ffffff").toString());
    ui->stripMarkerColor->setText(settings.value("stripMarkerColor", "#cf0202").toString());
    ui->dialogMarkerColor->setText(settings.value("dialogMarkerColor", "#e0f500").toString());
    ui->cyanMarkerColor->setText(settings.value("cyanMarkerColor", "#00edf5").toString());
    ui->magentaMarkerColor->setText(settings.value("magentaMarkerColor", "#f5007e").toString());
    ui->orangeMarkerColor->setText(settings.value("orangeMarkerColor", "#f56a00").toString());

    connect(ui->emptySliderColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::emptySliderColorBrowse_Clicked);
    connect(ui->stripMarkerColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::stripMarkerColorBrowse_Clicked);
    connect(ui->dialogMarkerColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::dialogMarkerColorBrowse_Clicked);
    connect(ui->cyanMarkerColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::cyanMarkerColorBrowse_Clicked);
    connect(ui->magentaMarkerColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::magentaMarkerColorBrowse_Clicked);
    connect(ui->orangeMarkerColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::orangeMarkerColorBrowse_Clicked);
    connect(ui->fullSliderColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::fullSliderColorBrowse_Clicked);
    connect(ui->caretColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::caretColorBrowse_Clicked);
    connect(ui->backgroundTrackColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::backgroundTrackColorBrowse_Clicked);
    connect(ui->progressFillColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::progressFillColorBrowse_Clicked);
    connect(ui->handlePlayheadColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::handlePlayheadColorBrowse_Clicked);
    connect(ui->markerColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::markerColorBrowse_Clicked);
    connect(ui->sceneMarkerColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::sceneMarkerColorBrowse_Clicked);
    connect(ui->cumshotMarkerColorBrowse, &QPushButton::clicked, this, &InterfaceSettings::cumshotMarkerColorBrowse_Clicked);
}

InterfaceSettings::~InterfaceSettings()
{
    delete ui;
}

bool InterfaceSettings::unsavedChanges()
{
    m_unsavedChanges = false;
    const QSettings settings;
    if (settings.value("theme", 0).toInt() != ui->currentTheme->currentIndex()) {
        m_unsavedChanges = true;
    } else if (settings.value("rememberWindowSize", false).toBool() != ui->rememberWindowSize->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("showPlaylistOnStart", true).toBool() != ui->showPlaylistOnStart->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("showVideoControlsOnStart", false).toBool() != ui->showVideoControlsOnStart->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("showStatusBarOnStart", false).toBool() != ui->showStatusBarOnStart->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("autohideSlider", true).toBool() != ui->autohideSlider->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("unhideSliderOnHotkey", true).toBool() != ui->unhideSliderOnHotkey->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("sliderAutohideTime", 5).toInt() != ui->sliderAutohideTimer->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("sliderHeight", 6).toInt() != ui->sliderHeight->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("videoMarkerHeight", 10).toInt() != ui->videoMarkerHeight->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("videoMarkerWidth", 2).toInt() != ui->videoMarkerWidth->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("videoMarkerSideLength", 14).toInt() != ui->videoMarkerSideLength->value()) {
        m_unsavedChanges = true;
    }

    return m_unsavedChanges;
}

void InterfaceSettings::resetToDefaults()
{
    const QSettings settings;
    ui->currentTheme->setCurrentIndex(0);
    ui->rememberWindowSize->setChecked(false);
    ui->showPlaylistOnStart->setChecked(true);
    ui->showVideoControlsOnStart->setChecked(false);
    ui->showStatusBarOnStart->setChecked(false);
    ui->autohideSlider->setChecked(true);
    ui->unhideSliderOnHotkey->setChecked(true);
    ui->sliderAutohideTimer->setValue(5);
    ui->sliderHeight->setValue(6);
    ui->videoMarkerHeight->setValue(10);
    ui->videoMarkerWidth->setValue(2);
    ui->videoMarkerSideLength->setValue(14);
    ui->sliderHandleRadius->setValue(8);
    ui->sliderLeftRightMargin->setValue(1);
    ui->sliderPadding->setValue(2);
    ui->videoMarkerVerticalMargin->setValue(0);
    ui->emptySliderColor->setText("#000000");
    ui->fullSliderColor->setText("#00d9ff");
    ui->caretColor->setText("#ffffff");
    ui->backgroundTrackColor->setText("#323232");
    ui->progressFillColor->setText("#0076d7");
    ui->handlePlayheadColor->setText("#ffffff");
    ui->markerColor->setText("#03c200");
    ui->sceneMarkerColor->setText("#000eab");
    ui->cumshotMarkerColor->setText("#ffffff");
    ui->stripMarkerColor->setText("#cf0202");
    ui->dialogMarkerColor->setText("#e0f500");
    ui->cyanMarkerColor->setText("#00edf5");
    ui->magentaMarkerColor->setText("#f5007e");
    ui->orangeMarkerColor->setText("#f56a00");
    m_unsavedChanges = true;
}

void InterfaceSettings::saveSettings()
{
    QSettings settings;
    settings.setValue("theme", ui->currentTheme->currentIndex());
    settings.setValue("rememberWindowSize", ui->rememberWindowSize->isChecked());
    settings.setValue("showPlaylistOnStart", ui->showPlaylistOnStart->isChecked());
    settings.setValue("showVideoControlsOnStart", ui->showVideoControlsOnStart->isChecked());
    settings.setValue("showStatusBarOnStart", ui->showStatusBarOnStart->isChecked());
    settings.setValue("autohideSlider", ui->autohideSlider->isChecked());
    settings.setValue("unhideSliderOnHotkey", ui->unhideSliderOnHotkey->isChecked());
    settings.setValue("sliderAutohideTime", ui->sliderAutohideTimer->value());
    settings.setValue("sliderHeight", ui->sliderHeight->value());
    settings.setValue("videoMarkerHeight", ui->videoMarkerHeight->value());
    settings.setValue("videoMarkerWidth", ui->videoMarkerWidth->value());
    settings.setValue("videoMarkerSideLength", ui->videoMarkerSideLength->value());
    settings.setValue("sliderHandleRadius", ui->sliderHandleRadius->value());
    settings.setValue("sliderLeftRightMargin", ui->sliderLeftRightMargin->value());
    settings.setValue("sliderPadding", ui->sliderPadding->value());
    settings.setValue("videoMarkerVerticalMargin", ui->videoMarkerVerticalMargin->value());
    settings.setValue("emptySliderColor", ui->emptySliderColor->text());
    settings.setValue("stripMarkerColor", ui->stripMarkerColor->text());
    settings.setValue("dialogMarkerColor", ui->dialogMarkerColor->text());
    settings.setValue("cyanMarkerColor", ui->cyanMarkerColor->text());
    settings.setValue("magentaMarkerColor", ui->magentaMarkerColor->text());
    settings.setValue("orangeMarkerColor", ui->orangeMarkerColor->text());
    settings.setValue("fullSliderColor", ui->fullSliderColor->text());
    settings.setValue("caretColor", ui->caretColor->text());
    settings.setValue("backgroundTrackColor", ui->backgroundTrackColor->text());
    settings.setValue("progressFillColor", ui->progressFillColor->text());
    settings.setValue("handlePlayheadColor", ui->handlePlayheadColor->text());
    settings.setValue("markerColor", ui->markerColor->text());
    settings.setValue("sceneMarkerColor", ui->sceneMarkerColor->text());
    settings.setValue("cumshotMarkerColor", ui->cumshotMarkerColor->text());
    m_unsavedChanges = false;
}

void InterfaceSettings::emptySliderColor_TextChanged(const QString &text) {}

void InterfaceSettings::emptySliderColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->emptySliderColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("emptySliderColor", color.name());
    }
}

void InterfaceSettings::stripMarkerColor_TextChanged(const QString &text) {}

void InterfaceSettings::stripMarkerColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->stripMarkerColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("stripMarkerColor", color.name());
    }
}

void InterfaceSettings::dialogMarkerColor_TextChanged(const QString &text) {}

void InterfaceSettings::dialogMarkerColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->dialogMarkerColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("dialogMarkerColor", color.name());
    }
}

void InterfaceSettings::cyanMarkerColor_TextChanged(const QString &text) {}

void InterfaceSettings::cyanMarkerColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->cyanMarkerColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("cyanMarkerColor", color.name());
    }
}

void InterfaceSettings::magentaMarkerColor_TextChanged(const QString &text) {}

void InterfaceSettings::magentaMarkerColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->magentaMarkerColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("magentaMarkerColor", color.name());
    }
}

void InterfaceSettings::orangeMarkerColor_TextChanged(const QString &text) {}

void InterfaceSettings::orangeMarkerColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->orangeMarkerColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("orangeMarkerColor", color.name());
    }
}

void InterfaceSettings::fullSliderColor_TextChanged(const QString &text) {}

void InterfaceSettings::fullSliderColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->fullSliderColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("fullSliderColor", color.name());
    }
}

void InterfaceSettings::caretColor_TextChanged(const QString &text) {}

void InterfaceSettings::caretColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->caretColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("caretColor", color.name());
    }
}

void InterfaceSettings::backgroundTrackColor_TextChanged(const QString &text) {}

void InterfaceSettings::backgroundTrackColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->backgroundTrackColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("backgroundTrackColor", color.name());
    }
}

void InterfaceSettings::progressFillColor_TextChanged(const QString &text) {}

void InterfaceSettings::progressFillColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->progressFillColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("progressFillColor", color.name());
    }
}

void InterfaceSettings::handlePlayheadColor_TextChanged(const QString &text) {}

void InterfaceSettings::handlePlayheadColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->handlePlayheadColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("handlePlayheadColor", color.name());
    }
}

void InterfaceSettings::markerColor_TextChanged(const QString &text) {}

void InterfaceSettings::markerColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->markerColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("markerColor", color.name());
    }
}

void InterfaceSettings::sceneMarkerColor_TextChanged(const QString &text) {}

void InterfaceSettings::sceneMarkerColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->sceneMarkerColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("sceneMarkerColor", color.name());
    }
}

void InterfaceSettings::cumshotMarkerColor_TextChanged(const QString &text) {}

void InterfaceSettings::cumshotMarkerColorBrowse_Clicked()
{
    QSettings settings;
    const QColor color = QColorDialog::getColor(QColor(ui->cumshotMarkerColor->text()), this, tr("Choose a Color"));

    if (color.isValid()) {
        settings.setValue("cumshotMarkerColor", color.name());
    }
}
