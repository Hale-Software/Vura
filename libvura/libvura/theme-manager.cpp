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

#include "theme-manager.h"


ThemeManager::ThemeManager(const QString &themePath, QObject *parent)
    : QObject(parent), m_themePath(themePath)
{
    m_watcher.addPath(m_themePath);
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &ThemeManager::handleFileChanged);
}

void ThemeManager::applyTheme(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonObject theme = QJsonDocument::fromJson(file.readAll()).object();
    QJsonObject colors = theme["colors"].toObject();

    QString qss = QString(
            "QMainWindow { background-color: %1; }"
            "QPushButton { background-color: %2; color: %3; }"
            "QLabel { font-family: '%4'; font-size: %5; color: %6; }"
        )
    .arg(colors["background"].toString())
    .arg(colors["accent"].toString())
    .arg(colors["text"].toString())
    .arg(theme["fonts"].toObject()["main"].toString())
    .arg(theme["fonts"].toObject()["size"].toString())
    .arg(colors["text"].toString());

    qApp->setStyleSheet(qss);
}

void ThemeManager::handleFileChanged(const QString &filePath)
{
    applyTheme(filePath);
}
