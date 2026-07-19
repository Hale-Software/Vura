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

#include "HotkeyEditWidget.h"
#include "ui_HotkeyEditWidget.h"
#include "SettingsDialog.h"


HotkeyEditWidget::HotkeyEditWidget(int id, QString action, QString defaultHotkey, QString hotkey, QWidget *parent) : QWidget(parent), ui(new Ui::HotkeyEditWidget)
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

    connect(ui->hotkeyBox, &QKeySequenceEdit::keySequenceChanged, this, &HotkeyEditWidget::keySequence_Changed);
    connect(ui->resetButton, &QToolButton::clicked, this, &HotkeyEditWidget::revertButton_Clicked);
    connect(ui->clearButton, &QToolButton::clicked, this, &HotkeyEditWidget::clearButton_Clicked);
}

HotkeyEditWidget::~HotkeyEditWidget()
{
    delete ui;
}

QString HotkeyEditWidget::getAction()
{
    return m_action;
}

QString HotkeyEditWidget::getHotkey()
{
    return m_hotkey;
}

void HotkeyEditWidget::setHotkey(QString hotkey)
{
    m_hotkey = hotkey;
    ui->hotkeyBox->setKeySequence(hotkey);
}

void HotkeyEditWidget::clearHotkey()
{
    m_hotkey.clear();
    ui->hotkeyBox->clear();
}

void HotkeyEditWidget::keySequence_Changed(const QKeySequence &keySequence)
{
    QString newHotkey = keySequence.toString();
    if (newHotkey != m_hotkey)
        emit hotkeyChanged(m_id, m_action, m_hotkey, newHotkey);
}

void HotkeyEditWidget::revertButton_Clicked()
{
    if (ui->hotkeyBox->keySequence() != m_defaultHotkey)
        ui->hotkeyBox->setKeySequence(m_defaultHotkey);
}

void HotkeyEditWidget::clearButton_Clicked()
{
    m_hotkey.clear();
    ui->hotkeyBox->clear();
}
