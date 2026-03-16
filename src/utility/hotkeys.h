#pragma once

#include <QObject>
#include <QAction>
#include <QSettings>
#include <QMap>
#include <QShortcut>
#include <QDebug>


class Hotkeys : QObject {
    Q_OBJECT

public:
    explicit Hotkeys(QObject *parent = nullptr);
    void setMenuItemHotkey(QAction &action);
    void removeMenuItemHotkey(QAction &action);

private:
    QMap<QString,QString> hotkeys;
    void setDefaultHotkeys();

};

