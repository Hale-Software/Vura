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

#include "SideDockWidget.h"
#include "ui_SideDockWidget.h"

#include <QPushButton>


SideDockWidget::SideDockWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::SideDockWidget),
      m_playlistWidget(new PlaylistWidget(this)),
      m_markerPanel(new MarkerPanel(this))
{
    ui->setupUi(this);

    m_playlistWidgetIndex = ui->stackedWidget->addWidget(m_playlistWidget);
    m_markerPanelIndex = ui->stackedWidget->addWidget(m_markerPanel);

    connect(ui->playlistButton, &QPushButton::clicked, this, &SideDockWidget::playlistButton_Clicked);
    connect(ui->markersButton, &QPushButton::clicked, this, &SideDockWidget::markersButton_Clicked);
}

SideDockWidget::~SideDockWidget()
{
    delete ui;
}

void SideDockWidget::setPlaylistWidgetVisibility(bool visible)
{
    //if (ui->playlistButton->isVisible() == visible)
    //    return;

    ui->playlistButton->setVisible(visible);

    visibilityHandler();
}

void SideDockWidget::playlistWidgetShow()
{
    ui->playlistButton->setVisible(true);
    visibilityHandler();
}

void SideDockWidget::playlistWidgetHide()
{
    ui->playlistButton->setVisible(false);
    visibilityHandler();
}

void SideDockWidget::setMarkerPanelVisibility(bool visible)
{
    //if (ui->markersButton->isVisible() == visible)
    //    return;

    ui->markersButton->setVisible(visible);

    visibilityHandler();
}

void SideDockWidget::markerPanelShow()
{
    ui->markersButton->setVisible(true);
    visibilityHandler();
}

void SideDockWidget::markerPanelHide()
{
    ui->markersButton->setVisible(false);
    visibilityHandler();
}

void SideDockWidget::playlistButton_Clicked()
{
    if (ui->stackedWidget->currentIndex() == m_playlistWidgetIndex)
        return;
    ui->stackedWidget->setCurrentIndex(m_playlistWidgetIndex);
}

void SideDockWidget::markersButton_Clicked()
{
    if (ui->stackedWidget->currentIndex() == m_markerPanelIndex)
        return;
    ui->stackedWidget->setCurrentIndex(m_markerPanelIndex);
}

void SideDockWidget::visibilityHandler()
{
    if (!ui->playlistButton->isVisible() && !ui->markersButton->isVisible()) {
        emit hideSideDockWidget();
    } else {
        if (!ui->playlistButton->isVisible() && ui->stackedWidget->currentIndex() == m_playlistWidgetIndex) {
            ui->stackedWidget->setCurrentIndex(m_markerPanelIndex);
        } else if (!ui->markersButton->isVisible() && ui->stackedWidget->currentIndex() == m_markerPanelIndex) {
            ui->stackedWidget->setCurrentIndex(m_playlistWidgetIndex);
        }
    }
}

void SideDockWidget::initPlaylistWidget()
{

}

void SideDockWidget::initMarkerPanel()
{

}
