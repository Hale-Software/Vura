//
// Created by halea on 8/5/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_AdvancedSettings.h" resolved

#include "advancedsettings.h"
#include "ui_AdvancedSettings.h"


AdvancedSettings::AdvancedSettings(QWidget *parent) : QWidget(parent), ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);
}

AdvancedSettings::~AdvancedSettings()
{
    delete ui;
}
