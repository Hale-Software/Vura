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

#include "RecentFilesMenu.h"

#include <QAction>
#include <QFileInfo>
#include <QMenu>
#include <QSettings>


namespace {

constexpr int kMaxEntries = 10;
constexpr auto kSettingsKey = "recentFileList";

// Entries are stored as local paths for files and URL strings for streams,
// the same format the old code wrote, so existing lists keep working.
QString toEntry(const QUrl &url)
{
    return url.isLocalFile() ? url.toLocalFile() : url.toString();
}

QUrl toUrl(const QString &entry)
{
    return QUrl::fromUserInput(entry, QString(), QUrl::AssumeLocalFile);
}

QString labelFor(const QString &entry, int index)
{
    const QUrl url = toUrl(entry);
    QString name = url.isLocalFile() ? QFileInfo(url.toLocalFile()).fileName()
                                     : url.toDisplayString();

    // A lone '&' would be read as a mnemonic marker ("Tom & Jerry.mkv").
    name.replace(QLatin1Char('&'), QLatin1String("&&"));

    // Keyboard mnemonics 1-9; the tenth entry just gets its number.
    const QString format = index < 9 ? QStringLiteral("&%1 %2") : QStringLiteral("%1 %2");
    return format.arg(QString::number(index + 1), name);
}

QStringList loadEntries()
{
    return QSettings().value(kSettingsKey).toStringList().mid(0, kMaxEntries);
}

}


RecentFilesMenu::RecentFilesMenu(QMenu *menu, QAction *clearAction, QObject *parent)
    : QObject(parent),
      m_menu(menu)
{
    m_menu->setToolTipsVisible(true);
    m_separator = m_menu->addSeparator();

    connect(clearAction, &QAction::triggered, this, &RecentFilesMenu::clear);

    rebuild();
}

void RecentFilesMenu::add(const QUrl &url)
{
    if (url.isEmpty())
        return;

    const QString entry = toEntry(url);
    QStringList entries = loadEntries();
    entries.removeAll(entry);
    entries.prepend(entry);

    QSettings().setValue(kSettingsKey, entries.mid(0, kMaxEntries));
    rebuild();
}

void RecentFilesMenu::clear()
{
    QSettings().remove(kSettingsKey);
    rebuild();
}

void RecentFilesMenu::rebuild()
{
    // deleteLater, not delete: picking an entry opens the file, which can call
    // add() -> rebuild() while that entry's triggered() signal is still running.
    for (QAction *action : std::as_const(m_fileActions)) {
        m_menu->removeAction(action);
        action->deleteLater();
    }
    m_fileActions.clear();

    const QStringList entries = loadEntries();
    for (int i = 0; i < entries.size(); ++i) {
        const QString &entry = entries.at(i);

        QAction *action = m_menu->addAction(labelFor(entry, i));
        action->setToolTip(entry);
        connect(action, &QAction::triggered, this,
                [this, url = toUrl(entry)] { emit fileSelected(url); });

        m_fileActions.append(action);
    }

    // Rebuilt eagerly on every change: a disabled submenu never emits
    // aboutToShow, so refreshing lazily there can't re-enable it.
    const bool hasEntries = !entries.isEmpty();
    m_separator->setVisible(hasEntries);
    m_menu->setEnabled(hasEntries);
}
