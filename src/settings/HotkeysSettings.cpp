//
// Created by halea on 8/5/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_HotkeysSettings.h" resolved

#include "hotkeyssettings.h"
#include "ui_HotkeysSettings.h"


HotkeysSettings::HotkeysSettings(QWidget *parent) : QWidget(parent), ui(new Ui::HotkeysSettings)
{
    ui->setupUi(this);
}

HotkeysSettings::~HotkeysSettings()
{
    delete ui;
}
