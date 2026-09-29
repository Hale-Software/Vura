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
