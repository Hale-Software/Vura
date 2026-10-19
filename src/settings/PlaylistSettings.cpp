/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "PlaylistSettings.h"
#include "ui_PlaylistSettings.h"

#include <QSettings>
#include <QDebug>


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
