/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QKeySequenceEdit>
#include <QKeySequence>
#include <QToolButton>


QT_BEGIN_NAMESPACE

namespace Ui {
    class HotkeyWidget;
}

QT_END_NAMESPACE

class HotkeyWidget : public QWidget {
    Q_OBJECT

public:
    explicit HotkeyWidget(int id, QString action, QString defaultHotkey, QString hotkey, QWidget *parent = nullptr);
    ~HotkeyWidget() override;

    QString getAction();
    QString getHotkey();
    void setHotkey(QString hotkey);
    void clearHotkey();

signals:
    void hotkeyChanged(int id, QString action, QString oldHotkey, QString newHotkey);

private slots:
    void keySequence_Changed(const QKeySequence &keySequence);
    void revertButton_Clicked();
    void clearButton_Clicked();

private:
    Ui::HotkeyWidget *ui;
    int m_id;
    QString m_name;
    QString m_action;
    QString m_defaultHotkey;
    QString m_hotkey;

};
