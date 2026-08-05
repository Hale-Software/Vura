//
// Created by halea on 8/5/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_AssociationsSettings.h" resolved

#include "associationssettings.h"
#include "ui_AssociationsSettings.h"


AssociationsSettings::AssociationsSettings(QWidget *parent) : QWidget(parent), ui(new Ui::AssociationsSettings)
{
    ui->setupUi(this);
}

AssociationsSettings::~AssociationsSettings()
{
    delete ui;
}
