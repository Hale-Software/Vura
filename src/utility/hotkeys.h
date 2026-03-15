#pragma once

#include <QObject>
#include <QAction>
#include <QSettings>
#include <QMap>
#include <QShortcut>
#include <QFile>
#include <QDebug>

#include "filemanager.h"


class Hotkeys : QObject {
    Q_OBJECT

public:
    explicit Hotkeys(QObject *parent = nullptr);
    void setMenuItemHotkey(QAction &action);

private:
    QMap<QString,QString> hotkeys;
    void setDefaultHotkeys(QString fileName);

};

