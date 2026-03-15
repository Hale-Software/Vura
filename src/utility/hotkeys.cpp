#include "hotkeys.h"


Hotkeys::Hotkeys(QObject *parent) : QObject(parent)
{
    QSettings settings;
    QString hotkeyFile = settings.value("hotkeyFile", "default.hkey").toString();
    if (!QFile(hotkeyFile).exists()) {
        qDebug() << "Hotkeys file does not exist.";
        setDefaultHotkeys(hotkeyFile);
    }

    FileManager fileManager;
    hotkeys = fileManager.readHotkeyFile(hotkeyFile.toStdString().c_str());

    //settings.beginGroup("Hotkeys");
    //const QStringList childKeys = settings.childKeys();
    //foreach (const QString &childKey, childKeys)
    //{
    //    hotkeys[childKey] = settings.value(childKey).toString();
    //}
    //settings.endGroup();

    //if (hotkeys.isEmpty()) {
    //    qDebug() << "Hotkeys are empty.";
    //    setDefaultHotkeys();
    //}
}

void Hotkeys::setMenuItemHotkey(QAction &action)
{
    if (!action.text().isEmpty()) {
        if (action.text() == "Play/Pause") {
            qDebug() << "Setting hotkey Space for action " << action.text();
            action.setShortcut(tr("Space"));
            action.setShortcutContext(Qt::ApplicationShortcut);
        } else if (hotkeys.contains(action.text())) {
            qDebug() << "Setting hotkey " << hotkeys[action.text()] << " for action " << action.text();
            action.setShortcut(hotkeys.value(action.text()));
            action.setShortcutContext(Qt::ApplicationShortcut);
        }
    }
}

void Hotkeys::setDefaultHotkeys(QString fileName)
{
    FileManager fileManager;
    qDebug() << "Saving default hotkeys to file: " << fileName;
    const QMap<QString, QString> hotkeyMap;

    // File
    hotkeyMap["Emergency Collapse"] = "C";
    hotkeyMap["Exit"] = "Ctrl+Q";
    hotkeyMap["Open File"] = "O";
    //hotkeyMap[HOTKEYS::OPEN_FOLDER] = "Ctrl+F";
    //hotkeyMap[HOTKEYS::OPEN_MULTIPLE_FILES] = "Alt+O";
    //hotkeyMap[HOTKEYS::OPEN_NETWORK_STREAM] = "Ctrl+N";
    //hotkeyMap[HOTKEYS::SAVE] = "Ctrl+S";
    //hotkeyMap[HOTKEYS::SAVE_AS] = "Ctrl+Shift+S";
    //hotkeyMap[HOTKEYS::SAVE_PLAYLIST] = "Alt+S";
    //hotkeyMap[HOTKEYS::SAVE_A_COPY] = "Ctrl+Alt+S";
    //hotkeyMap[HOTKEYS::CLOSE] = "Ctrl+A";
    //hotkeyMap[HOTKEYS::CLOSE_ALL] = "Ctrl+Shift+A";
    //hotkeyMap[HOTKEYS::PREFERENCES] = "Ctrl+Shift+P";
/*
    // View
    hotkeyMap[HOTKEYS::TOGGLE_PLAYLIST] = "Ctrl+P";
    hotkeyMap[HOTKEYS::TOGGLE_STATUS_BAR] = "Ctrl+B";
    hotkeyMap[HOTKEYS::SHOW_LOG_VIEWER] = "Ctrl+L";
    hotkeyMap[HOTKEYS::TOGGLE_VIDEO_CONTROLS] = "Ctrl+V";

    // Playback
    hotkeyMap[HOTKEYS::NEXT_VIDEO] = "N";
    hotkeyMap[HOTKEYS::NEXT_FRAME] = "]";
    hotkeyMap[HOTKEYS::PREVIOUS_VIDEO] = "P";
    hotkeyMap[HOTKEYS::PREVIOUS_FRAME] = "[";
    hotkeyMap[HOTKEYS::RESTART_VIDEO] = "R";
    hotkeyMap[HOTKEYS::JUMP_TO_SPECIFIC_TIME] = "Ctrl+T";
    hotkeyMap[HOTKEYS::JUMP_BACK_EXTRA_LARGE] = "";
    hotkeyMap[HOTKEYS::JUMP_BACK_LARGE] = "Q";
    hotkeyMap[HOTKEYS::JUMP_BACK_MEDIUM] = "A";
    hotkeyMap[HOTKEYS::JUMP_BACK_SMALL] = "Z";
    hotkeyMap[HOTKEYS::JUMP_FORWARD_EXTRA_LARGE] = "";
    hotkeyMap[HOTKEYS::JUMP_FORWARD_LARGE] = "W";
    hotkeyMap[HOTKEYS::JUMP_FORWARD_MEDIUM] = "S";
    hotkeyMap[HOTKEYS::JUMP_FORWARD_SMALL] = "X";
    hotkeyMap[HOTKEYS::FASTER] = "Num++";
    hotkeyMap[HOTKEYS::FASTER_FINE] = "Ctrl+Num++";
    hotkeyMap[HOTKEYS::NORMAL_SPEED] = "=";
    hotkeyMap[HOTKEYS::SLOWER] = "Num+-";
    hotkeyMap[HOTKEYS::SLOWER_FINE] = "Ctrl+Num+-";

    // Markers
    hotkeyMap[HOTKEYS::ADD_CUMSHOT_MARKER] = "Ctrl+3";
    hotkeyMap[HOTKEYS::ADD_CYAN_MARKER] = "Ctrl+5";
    hotkeyMap[HOTKEYS::ADD_DIALOG_MARKER] = "Ctrl+4";
    hotkeyMap[HOTKEYS::ADD_MAGENTA_MARKER] = "Ctrl+6";
    hotkeyMap[HOTKEYS::ADD_MARKER] = "M";
    hotkeyMap[HOTKEYS::ADD_ORANGE_MARKER] = "Ctrl+7";
    hotkeyMap[HOTKEYS::ADD_SCENE_MARKER] = "Ctrl+1";
    hotkeyMap[HOTKEYS::ADD_STRIP_MARKER] = "Ctrl+2";
    hotkeyMap[HOTKEYS::CLEAR_IN] = "Ctrl+Shift+,";
    hotkeyMap[HOTKEYS::CLEAR_IN_AND_OUT] = "Ctrl+Shift+X";
    hotkeyMap[HOTKEYS::CLEAR_MARKERS] = "Ctrl+Shift+M";
    hotkeyMap[HOTKEYS::CLEAR_OUT] = "Ctrl+Shift+.";
    hotkeyMap[HOTKEYS::CLEAR_SELECTED_MARKER] = "Ctrl+C";
    hotkeyMap[HOTKEYS::EDIT_SELECTED_MARKER] = "Alt+M";
    hotkeyMap[HOTKEYS::GO_TO_IN] = "Shift+,";
    hotkeyMap[HOTKEYS::GO_TO_NEXT_MARKER] = "B";
    hotkeyMap[HOTKEYS::GO_TO_OUT] = "Shift+.";
    hotkeyMap[HOTKEYS::GO_TO_PREVIOUS_MARKER] = "V";
    hotkeyMap[HOTKEYS::MARK_IN] = ",";
    hotkeyMap[HOTKEYS::MARK_OUT] = ".";

    // Audio
    hotkeyMap[HOTKEYS::DECREASE_VOLUME] = "Down";
    hotkeyMap[HOTKEYS::INCREASE_VOLUME] = "Up";
    hotkeyMap[HOTKEYS::MUTE] = "Ctrl+M";

    // Video
    hotkeyMap[HOTKEYS::FULLSCREEN] = "F";

    // Tools
    hotkeyMap[HOTKEYS::MAKE_SUBCLIP] = "Ctrl+U";
    hotkeyMap[HOTKEYS::STREAM_STASH_VIDEO] = "Ctrl+X";

    // Help
    hotkeyMap[HOTKEYS::HELP] = "F1";
*/
    if (!fileManager.writeHotkeyFile(fileName.toStdString().c_str(), hotkeyMap)) {
        qWarning() << "Could not write hotkey file";
        return;
    }

    qDebug() << "Saved default hotkeys to file: " << fileName;
}
