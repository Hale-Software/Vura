#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QKeySequence>

#include <libvura/hotkeys.h>

#include "HotkeyDelegate.h"

class HotkeySettingsDialog : public QDialog {
    Q_OBJECT
public:
    QTableWidget *table;
    QMap<QString, QAction*> &actionsRef;

    explicit HotkeySettingsDialog(QMap<QString, QAction*> &actions, QWidget *parent = nullptr);

private slots:
    void applyChanges();

};
