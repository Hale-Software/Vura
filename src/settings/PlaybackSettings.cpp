//
// Created by halea on 8/5/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PlaybackSettings.h" resolved

#include "playbacksettings.h"
#include "ui_PlaybackSettings.h"


PlaybackSettings::PlaybackSettings(QWidget *parent) : QWidget(parent), ui(new Ui::PlaybackSettings)
{
    ui->setupUi(this);
}

PlaybackSettings::~PlaybackSettings()
{
    delete ui;
}
