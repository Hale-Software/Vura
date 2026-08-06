#include "HotkeysSettings.h"
#include "ui_HotkeysSettings.h"


HotkeysSettings::HotkeysSettings(QWidget *parent) : QWidget(parent), ui(new Ui::HotkeysSettings)
{
    ui->setupUi(this);
}

HotkeysSettings::~HotkeysSettings()
{
    delete ui;
}

bool HotkeysSettings::unsavedChanges()
{
    return m_unsavedChanges;
}

void HotkeysSettings::resetToDefaults()
{

}

void HotkeysSettings::saveSettings()
{

}
