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

#pragma once

#include <QList>
#include <QObject>
#include <QUrl>

class QAction;
class QMenu;


/// Manages the "Open Recent" submenu: stores the list in QSettings, keeps the
/// menu's entries in sync, and reports which entry the user picked.
///
/// Attaches to an existing QMenu (e.g. one from the .ui file) rather than
/// subclassing it, because Qt Designer can't promote menus.
class RecentFilesMenu : public QObject
{
    Q_OBJECT
public:
    RecentFilesMenu(QMenu *menu, QAction *clearAction, QObject *parent = nullptr);

public slots:
    /// Moves the URL to the top of the list, adding it if it isn't there.
    void add(const QUrl &url);
    void clear();

    signals:
        void fileSelected(const QUrl &url);

private:
    void rebuild();

    QMenu *m_menu = nullptr;
    QAction *m_separator = nullptr;
    QList<QAction *> m_fileActions;
};
