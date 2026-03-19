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

#include "hotkeywidget.h"
#include "ui_hotkeywidget.h"
#include "settingswindow.h"


HotkeyWidget::HotkeyWidget(int id, QString action, QString defaultHotkey, QString hotkey, QWidget *parent) : QWidget(parent), ui(new Ui::HotkeyWidget)
{
    ui->setupUi(this);

    m_id = id;
    m_name = action.remove("&");
    m_action = action;
    m_defaultHotkey = defaultHotkey;
    m_hotkey = hotkey;

    QString hotkeyBox_ToolTip = "Hotkey combination for " + action + ".";
    ui->actionLabel->setText(action);
    ui->hotkeyBox->setKeySequence(hotkey);

    connect(ui->hotkeyBox, &QKeySequenceEdit::keySequenceChanged, this, &HotkeyWidget::keySequence_Changed);
    connect(ui->resetButton, &QToolButton::clicked, this, &HotkeyWidget::revertButton_Clicked);
    connect(ui->clearButton, &QToolButton::clicked, this, &HotkeyWidget::clearButton_Clicked);
}

HotkeyWidget::~HotkeyWidget()
{
    delete ui;
}

QString HotkeyWidget::getAction()
{
    return m_action;
}

QString HotkeyWidget::getHotkey()
{
    return m_hotkey;
}

void HotkeyWidget::setHotkey(QString hotkey)
{
    m_hotkey = hotkey;
    ui->hotkeyBox->setKeySequence(hotkey);
}

void HotkeyWidget::clearHotkey()
{
    m_hotkey.clear();
    ui->hotkeyBox->clear();
}

void HotkeyWidget::keySequence_Changed(const QKeySequence &keySequence)
{
    QString newHotkey = keySequence.toString();
    if (newHotkey != m_hotkey)
        emit hotkeyChanged(m_id, m_action, m_hotkey, newHotkey);
}

void HotkeyWidget::revertButton_Clicked()
{
    if (ui->hotkeyBox->keySequence() != m_defaultHotkey)
        ui->hotkeyBox->setKeySequence(m_defaultHotkey);
}

void HotkeyWidget::clearButton_Clicked()
{
    m_hotkey.clear();
    ui->hotkeyBox->clear();
}
