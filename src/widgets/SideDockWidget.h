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

#include "MarkerPanel.h"
#include "PlaylistWidget.h"

#include <libvura/models/playlist.h>


QT_BEGIN_NAMESPACE
namespace Ui { class SideDockWidget; }
QT_END_NAMESPACE


class SideDockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SideDockWidget(QWidget *parent = nullptr);
    ~SideDockWidget() override;

    PlaylistWidget *playlistWidget() const { return m_playlistWidget; }
    MarkerPanel *markerPanel() const { return m_markerPanel; }

signals:
    void hideSideDockWidget();

public slots:
    void setPlaylistWidgetVisibility(bool visible);
    void playlistWidgetShow();
    void playlistWidgetHide();

    void setMarkerPanelVisibility(bool visible);
    void markerPanelShow();
    void markerPanelHide();

private slots:
    void playlistButton_Clicked();
    void markersButton_Clicked();

private:
    void visibilityHandler();
    void initPlaylistWidget();
    void initMarkerPanel();

    Ui::SideDockWidget *ui;
    PlaylistWidget *m_playlistWidget = nullptr;
    MarkerPanel *m_markerPanel = nullptr;

    int m_playlistWidgetIndex = 0;
    int m_markerPanelIndex = 0;

};
