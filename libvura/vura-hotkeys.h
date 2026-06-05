#pragma once

#include <QObject>
#include <QAction>
#include <QSettings>
#include <QMap>
#include <QShortcut>
#include <QDebug>


class VuraHotkeys : QObject {
    Q_OBJECT

public:
    explicit VuraHotkeys(QObject *parent = nullptr);
    void setMenuItemHotkey(QAction &action);
    void removeMenuItemHotkey(QAction &action);

private:
    QMap<QString,QString> hotkeys;
    void setDefaultHotkeys();

};
