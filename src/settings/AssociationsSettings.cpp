#include "AssociationsSettings.h"
#include "ui_AssociationsSettings.h"


AssociationsSettings::AssociationsSettings(QWidget *parent) : QWidget(parent), ui(new Ui::AssociationsSettings)
{
    ui->setupUi(this);
}

AssociationsSettings::~AssociationsSettings()
{
    delete ui;
}

bool AssociationsSettings::unsavedChanges()
{
    return m_unsavedChanges;
}

void AssociationsSettings::resetToDefaults()
{

}

void AssociationsSettings::saveSettings()
{

}
