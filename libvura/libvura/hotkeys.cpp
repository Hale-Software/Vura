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

#include "hotkeys.h"


VuraHotkeys::VuraHotkeys(QObject *parent) : QObject(parent)
{
    QSettings settings;
    settings.beginGroup("Hotkeys");
    const QStringList childKeys = settings.childKeys();
    foreach (const QString &childKey, childKeys)
    {
        hotkeys[childKey] = settings.value(childKey).toString();
    }
    settings.endGroup();

    if (hotkeys.isEmpty()) {
        setDefaultHotkeys();
    }
}

void VuraHotkeys::resetHotkeys()
{
    QSettings settings;
    settings.beginGroup("Hotkeys");
    settings.remove("");
    settings.endGroup();
    settings.sync();
    hotkeys.clear();
    setDefaultHotkeys();
}

void VuraHotkeys::setMenuItemHotkeys(const QMap<QString, QAction *> &actions) const
{
    for (auto it = actions.constBegin(); it != actions.constEnd(); ++it) {
        // Column 0: Action Name (Read-only)
        const QString& currentActionName = it.key();
        if (currentActionName == "Play/Pause") {
            it.value()->setShortcut(tr("Space"));
            it.value()->setShortcutContext(Qt::ApplicationShortcut);

        } else if (hotkeys.contains(currentActionName)) {
            if (!hotkeys.value(currentActionName).isEmpty()) {
                it.value()->setShortcut(hotkeys.value(currentActionName));
                it.value()->setShortcutContext(Qt::ApplicationShortcut);
            }
        }
    }
}

void VuraHotkeys::setDefaultHotkeys()
{
    QSettings settings;
    QMap<QString, QString> hotkeyMap;

    // File
    hotkeyMap["Emergency Collapse"] = "C";
    hotkeyMap["Exit"] = "Ctrl+Q";
    hotkeyMap["Open File"] = "O";
    hotkeyMap["Open Folder"] = "Ctrl+F";
    hotkeyMap["Open Multiple Files"] = "Alt+O";
    hotkeyMap["Open Network Stream"] = "Ctrl+N";
    hotkeyMap["Save"] = "Ctrl+S";
    hotkeyMap["Save As"] = "Ctrl+Shift+S";
    hotkeyMap["Save Playlist"] = "Alt+S";
    hotkeyMap["Save A Copy"] = "Ctrl+Alt+S";
    hotkeyMap["Close"] = "Ctrl+A";
    hotkeyMap["Close All"] = "Ctrl+Shift+A";
    hotkeyMap["Preferences"] = "Ctrl+Shift+P";
    hotkeyMap["Clear Recent Files"] = "";
    hotkeyMap["Rename Current File"] = "Ctrl+Alt+R";


    // Playback
    hotkeyMap["Next Video"] = "N";
    hotkeyMap["Next Frame"] = "]";
    hotkeyMap["Previous Video"] = "P";
    hotkeyMap["Previous Frame"] = "[";
    hotkeyMap["Restart Video"] = "R";
    hotkeyMap["Jump to Specific Time"] = "Ctrl+T";
    hotkeyMap["Jump Back Extra Large"] = "1";
    hotkeyMap["Jump Back Large"] = "Q";
    hotkeyMap["Jump Back Medium"] = "A";
    hotkeyMap["Jump Back Small"] = "Z";
    hotkeyMap["Jump Fwd Extra Large"] = "2";
    hotkeyMap["Jump Fwd Large"] = "W";
    hotkeyMap["Jump Fwd Medium"] = "S";
    hotkeyMap["Jump Fwd Small"] = "X";
    hotkeyMap["Faster"] = "+";
    hotkeyMap["Faster (fine)"] = "Ctrl++";
    hotkeyMap["Normal"] = "=";
    hotkeyMap["Slower"] = "-";
    hotkeyMap["Slower (fine)"] = "Ctrl+-";

    // Audio
    hotkeyMap["Decrease Volume"] = "Down";
    hotkeyMap["Increase Volume"] = "Up";
    hotkeyMap["Mute"] = "Ctrl+M";

    // Video
    hotkeyMap["Fullscreen"] = "F";
    hotkeyMap["Take Snapshot"] = "";

    // View
    hotkeyMap["Toggle Playlist"] = "Ctrl+P";
    hotkeyMap["Toggle Status Bar"] = "Ctrl+B";
    hotkeyMap["Toggle Video Controls"] = "Ctrl+V";
    hotkeyMap["Media Information"] = "";
    hotkeyMap["Toggle Tags"] = "";

    // Markers
    hotkeyMap["Add Cumshot Marker"] = "Ctrl+3";
    hotkeyMap["Add Cyan Marker"] = "Ctrl+5";
    hotkeyMap["Add Dialog Marker"] = "Ctrl+4";
    hotkeyMap["Add Magenta Marker"] = "Ctrl+6";
    hotkeyMap["Add Marker"] = "M";
    hotkeyMap["Add Orange Marker"] = "Ctrl+7";
    hotkeyMap["Add Scene Transition Marker"] = "Ctrl+1";
    hotkeyMap["Add Strip Marker"] = "Ctrl+2";
    hotkeyMap["Clear In"] = "Ctrl+Shift+,";
    hotkeyMap["Clear In and Out"] = "Ctrl+Shift+X";
    hotkeyMap["Clear Markers"] = "Ctrl+Shift+M";
    hotkeyMap["Clear Out"] = "Ctrl+Shift+.";
    hotkeyMap["Clear Selected Marker"] = "Ctrl+C";
    hotkeyMap["Edit Selected Marker"] = "Alt+M";
    hotkeyMap["Go to In"] = "Shift+,";
    hotkeyMap["Go to Next Marker"] = "B";
    hotkeyMap["Go to Out"] = "Shift+.";
    hotkeyMap["Go to Previous Marker"] = "V";
    hotkeyMap["Mark In"] = ",";
    hotkeyMap["Mark Out"] = ".";

    // Tools
    hotkeyMap["Make Subclip"] = "Ctrl+U";
    hotkeyMap["Stream Video from Stash"] = "Ctrl+X";
    hotkeyMap["Test Function"] = "";

    // Subtitle
    hotkeyMap["Toggle Subtitles"] = "";

    // Help
    hotkeyMap["Help"] = "F1";
    hotkeyMap["View Current Log"] = "Ctrl+L";
    hotkeyMap["About"] = "";


    settings.beginGroup("Hotkeys");
    QMapIterator<QString, QString> i(hotkeyMap);
    while (i.hasNext()) {
        i.next();
        settings.setValue(i.key(), i.value());
    }

    settings.endGroup();
    settings.sync();

    hotkeys = hotkeyMap;
}

void VuraHotkeys::updateMenuItemHotkeys(const QMap<QString, QAction *> &actions)
{
    for (auto it = actions.constBegin(); it != actions.constEnd(); ++it) {
        hotkeys[it.key()] = it.value()->shortcut().toString();
    }

    QSettings settings;
    settings.beginGroup("Hotkeys");
    QMapIterator<QString, QString> i(hotkeys);
    while (i.hasNext()) {
        i.next();
        settings.setValue(i.key(), i.value());
    }

    settings.endGroup();
    settings.sync();
}
