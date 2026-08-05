//
// Created by halea on 8/5/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_InterfaceSettings.h" resolved

#include "interfacesettings.h"
#include "ui_InterfaceSettings.h"


InterfaceSettings::InterfaceSettings(QWidget *parent) : QWidget(parent), ui(new Ui::InterfaceSettings)
{
    ui->setupUi(this);
}

InterfaceSettings::~InterfaceSettings()
{
    delete ui;
}
