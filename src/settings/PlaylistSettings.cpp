#include "PlaylistSettings.h"
#include "ui_PlaylistSettings.h"


PlaylistSettings::PlaylistSettings(QWidget *parent) : QWidget(parent), ui(new Ui::PlaylistSettings)
{
    ui->setupUi(this);
}

PlaylistSettings::~PlaylistSettings()
{
    delete ui;
}

bool PlaylistSettings::unsavedChanges()
{
    return m_unsavedChanges;
}

void PlaylistSettings::resetToDefaults()
{

}

void PlaylistSettings::saveSettings()
{

}
