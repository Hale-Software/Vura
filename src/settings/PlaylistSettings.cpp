//
// Created by halea on 8/5/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PlaylistSettings.h" resolved

#include "playlistsettings.h"
#include "ui_PlaylistSettings.h"


PlaylistSettings::PlaylistSettings(QWidget *parent) : QWidget(parent), ui(new Ui::PlaylistSettings)
{
    ui->setupUi(this);
}

PlaylistSettings::~PlaylistSettings()
{
    delete ui;
}
