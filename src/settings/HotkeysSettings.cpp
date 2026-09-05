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

#include "HotkeysSettings.h"
#include "ui_HotkeysSettings.h"

#include "KeySequenceDelegate.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QKeyEvent>
#include <QTimer>
#include <QSettings>
#include <QDebug>

#include <libvura/hotkeys/hotkey-manager.h>
#include <libvura/models/hotkey-model.h>


HotkeysSettings::HotkeysSettings(QWidget *parent) : QWidget(parent), ui(new Ui::HotkeysSettings)
{
    ui->setupUi(this);

    m_model = new HotkeyModel(this);
    m_model->setConflictResolver([this](const QString &requestingLabel, const QString &currentOwnerLabel,
                                        const QKeySequence &sequence) {
        return confirmReassignment(requestingLabel, currentOwnerLabel, sequence);
    });

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(-1);   // -1 searches every column

    m_delegate = new KeySequenceDelegate(this);

    ui->hotkeyView->setModel(m_proxyModel);
    ui->hotkeyView->setItemDelegateForColumn(HotkeyModel::ShortcutColumn, m_delegate);
    ui->hotkeyView->sortByColumn(HotkeyModel::ActionColumn, Qt::AscendingOrder);
    ui->hotkeyView->installEventFilter(this);

    QHeaderView *header = ui->hotkeyView->header();
    header->setSectionResizeMode(HotkeyModel::ActionColumn, QHeaderView::Stretch);
    header->setSectionResizeMode(HotkeyModel::CategoryColumn, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(HotkeyModel::ShortcutColumn, QHeaderView::Interactive);
    header->setSectionResizeMode(HotkeyModel::GlobalColumn, QHeaderView::ResizeToContents);
    ui->hotkeyView->setColumnWidth(HotkeyModel::ShortcutColumn, 160);
    ui->hotkeyView->setColumnHidden(HotkeyModel::GlobalColumn, !HotkeyManager::supportsGlobalHotkeys());

    connect(ui->searchEdit, &QLineEdit::textChanged, this, &HotkeysSettings::search_Changed);
    connect(ui->searchScope, &QComboBox::currentIndexChanged, this, &HotkeysSettings::searchScope_Changed);
    connect(ui->clearButton, &QPushButton::clicked, this, &HotkeysSettings::clearHotkey_Clicked);
    connect(ui->restoreButton, &QPushButton::clicked, this, &HotkeysSettings::restoreDefault_Clicked);
    connect(ui->hotkeyView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &HotkeysSettings::selection_Changed);

    connect(m_model, &HotkeyModel::modified, this, &HotkeysSettings::model_Modified);
    connect(m_model, &HotkeyModel::errorMessage, this, &HotkeysSettings::model_ErrorMessage);

    if (m_model->rowCount() == 0) {
        ui->statusLabel->setText(tr("No commands registered. HotkeyManager::registerWindow() "
                                    "must run before the settings dialog is opened."));
    }
}

HotkeysSettings::~HotkeysSettings()
{
    delete ui;
}

bool HotkeysSettings::unsavedChanges()
{
    return m_model && m_model->isModified();
}

void HotkeysSettings::resetToDefaults()
{
    m_model->restoreDefaults();
}

void HotkeysSettings::saveSettings()
{
    m_model->commit();
    ui->statusLabel->clear();
}

bool HotkeysSettings::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->hotkeyView && event->type() == QEvent::KeyPress) {
        const auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
            clearHotkey_Clicked();
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void HotkeysSettings::search_Changed(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);
}

void HotkeysSettings::searchScope_Changed(int index)
{
    switch (index) {
    case 1:  m_proxyModel->setFilterKeyColumn(HotkeyModel::ActionColumn);   break;
    case 2:  m_proxyModel->setFilterKeyColumn(HotkeyModel::CategoryColumn); break;
    case 3:  m_proxyModel->setFilterKeyColumn(HotkeyModel::ShortcutColumn); break;
    default: m_proxyModel->setFilterKeyColumn(-1);                          break;
    }
}

void HotkeysSettings::selection_Changed()
{
    const bool hasSelection = currentSourceRow() >= 0;
    ui->clearButton->setEnabled(hasSelection);
    ui->restoreButton->setEnabled(hasSelection);
}

void HotkeysSettings::clearHotkey_Clicked()
{
    const int row = currentSourceRow();
    if (row >= 0) {
        m_model->clearShortcut(row);
    }
}

void HotkeysSettings::restoreDefault_Clicked()
{
    const int row = currentSourceRow();
    if (row < 0) {
        return;
    }

    // Routed through setData so a default that collides with another user
    // binding still goes through conflict resolution.
    m_model->setData(m_model->index(row, HotkeyModel::ShortcutColumn),
                     QVariant::fromValue(m_model->defaultSequenceFor(row)),
                     Qt::EditRole);
}

void HotkeysSettings::model_Modified()
{
    ui->statusLabel->clear();
    emit settingsChanged();
}

void HotkeysSettings::model_ErrorMessage(const QString &message)
{
    ui->statusLabel->setText(message);

    QTimer::singleShot(8000, this, [this]() {
        ui->statusLabel->clear();
    });
}

int HotkeysSettings::currentSourceRow() const
{
    const QModelIndex proxyIndex = ui->hotkeyView->selectionModel()->currentIndex();
    if (!proxyIndex.isValid()) {
        return -1;
    }
    return m_proxyModel->mapToSource(proxyIndex).row();
}

bool HotkeysSettings::confirmReassignment(const QString &requestingLabel, const QString &currentOwnerLabel,
                                          const QKeySequence &sequence)
{
    const QMessageBox::StandardButton answer = QMessageBox::question(this,
        tr("Hotkey Already Assigned"),
        tr("\"%1\" is already assigned to \"%2\".\n\n"
           "Reassign it to \"%3\"? \"%2\" will be left without a hotkey.")
            .arg(sequence.toString(QKeySequence::NativeText), currentOwnerLabel, requestingLabel),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    return answer == QMessageBox::Yes;
}
