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

#include "settings-manager.h"


SettingsManager::SettingsManager(const QString &filePath, QObject *parent)
    : QObject(parent), m_filePath(filePath)
{

}

bool SettingsManager::saveSettings(const QJsonObject &settings)
{
    QString tempPath = m_filePath + ".tmp";
    QFile file(tempPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(settings);
    file.write(doc.toJson());
    file.close();

    QFile originalFile(m_filePath);
    if (originalFile.exists() && !originalFile.remove()) return false;

    return file.rename(m_filePath);
}

QJsonObject SettingsManager::loadSettings()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonObject();
    }

    QByteArray data = file.readAll();
    file.close();

    return QJsonDocument::fromJson(data).object();
}
