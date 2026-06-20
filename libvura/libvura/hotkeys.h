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

    void setMenuItemHotkeys(const QMap<QString, QAction*> &actions) const;
    void updateMenuItemHotkeys(const QMap<QString, QAction*> &actions);
    void resetHotkeys();

private:
    QMap<QString,QString> hotkeys;
    void setDefaultHotkeys();

};
