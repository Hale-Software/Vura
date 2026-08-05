//
// Created by halea on 8/5/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PlayerSettings.h" resolved

#include "playersettings.h"
#include "ui_PlayerSettings.h"


PlayerSettings::PlayerSettings(QWidget *parent) : QWidget(parent), ui(new Ui::PlayerSettings)
{
    ui->setupUi(this);
}

PlayerSettings::~PlayerSettings()
{
    delete ui;
}
