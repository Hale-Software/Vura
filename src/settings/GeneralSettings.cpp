//
// Created by halea on 8/5/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_GeneralSettings.h" resolved

#include "generalsettings.h"
#include "ui_GeneralSettings.h"


GeneralSettings::GeneralSettings(QWidget *parent) : QWidget(parent), ui(new Ui::GeneralSettings)
{
    ui->setupUi(this);
}

GeneralSettings::~GeneralSettings()
{
    delete ui;
}
