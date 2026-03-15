//
// Created by halea on 1/30/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MediaInformation.h" resolved

#include "mediainformation.h"
#include "ui_MediaInformation.h"


MediaInformation::MediaInformation(QWidget *parent) : QWidget(parent), ui(new Ui::MediaInformation)
{
    ui->setupUi(this);
}

MediaInformation::~MediaInformation()
{
    delete ui;
}