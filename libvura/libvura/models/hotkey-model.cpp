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

#include "hotkey-model.h"

#include <QFont>


HotkeyModel::HotkeyModel(QObject *parent) : QAbstractTableModel(parent)
{
    reload();
}

int HotkeyModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_rows.size());
}

int HotkeyModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant HotkeyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rows.size()) {
        return {};
    }

    const HotkeyBinding &binding = m_rows.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()) {
        case ActionColumn:
            return binding.label;
        case CategoryColumn:
            return binding.category;
        case ShortcutColumn:
            if (role == Qt::EditRole) {
                return binding.sequence;
            }
            return binding.sequence.isEmpty() ? QString()
                                              : binding.sequence.toString(QKeySequence::NativeText);
        default:
            return {};
        }

    case Qt::CheckStateRole:
        if (index.column() == GlobalColumn) {
            return binding.global ? Qt::Checked : Qt::Unchecked;
        }
        return {};

    case Qt::FontRole: {
        // Rebound rows read as changed at a glance, the way Qt Creator marks them.
        if (binding.isDefault()) {
            return {};
        }
        QFont font;
        font.setBold(true);
        return font;
    }

    case Qt::ToolTipRole: {
        const QString defaultText = binding.defaultSequence.isEmpty()
            ? tr("none")
            : binding.defaultSequence.toString(QKeySequence::NativeText);
        return tr("%1\nCommand: %2\nDefault: %3").arg(binding.label, binding.id, defaultText);
    }

    case Qt::TextAlignmentRole:
        if (index.column() == GlobalColumn) {
            return QVariant(Qt::AlignCenter);
        }
        return {};

    default:
        return {};
    }
}

bool HotkeyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rows.size()) {
        return false;
    }

    HotkeyBinding &binding = m_rows[index.row()];

    if (index.column() == ShortcutColumn && role == Qt::EditRole) {
        // Explicit type check: QVariant will happily coerce a QString into a
        // QKeySequence using NativeText, which is not the stored format.
        const QKeySequence sequence = value.userType() == QMetaType::QKeySequence
            ? value.value<QKeySequence>()
            : QKeySequence::fromString(value.toString(), QKeySequence::PortableText);

        if (sequence == binding.sequence) {
            return false;
        }

        if (!sequence.isEmpty()) {
            const int conflictRow = rowForSequence(sequence, index.row());
            if (conflictRow >= 0) {
                if (!m_conflictResolver
                    || !m_conflictResolver(binding.label, m_rows.at(conflictRow).label, sequence)) {
                    return false;
                }

                m_rows[conflictRow].sequence = QKeySequence();
                m_rows[conflictRow].global = false;
                emit dataChanged(this->index(conflictRow, ShortcutColumn),
                                 this->index(conflictRow, GlobalColumn));
            }
        }

        binding.sequence = sequence;

        if (binding.global && !HotkeyManager::isValidGlobalSequence(sequence)) {
            binding.global = false;
        }

        emit dataChanged(this->index(index.row(), ShortcutColumn),
                         this->index(index.row(), GlobalColumn));
        emit modified();
        return true;
    }

    if (index.column() == GlobalColumn && role == Qt::CheckStateRole) {
        const bool requested = value.toInt() == Qt::Checked;

        if (requested && !HotkeyManager::isValidGlobalSequence(binding.sequence)) {
            emit errorMessage(tr("System-wide hotkeys must include Ctrl, Alt, Shift or Win, "
                                 "or be a dedicated media key. Otherwise the key would be "
                                 "captured from every other application."));
            return false;
        }

        binding.global = requested;
        emit dataChanged(index, index);
        emit modified();
        return true;
    }

    return false;
}

QVariant HotkeyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    switch (section) {
    case ActionColumn:   return tr("Action");
    case CategoryColumn: return tr("Category");
    case ShortcutColumn: return tr("Hotkey");
    case GlobalColumn:   return tr("Global");
    default:             return {};
    }
}

Qt::ItemFlags HotkeyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags itemFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == ShortcutColumn) {
        itemFlags |= Qt::ItemIsEditable;
    } else if (index.column() == GlobalColumn) {
        itemFlags |= Qt::ItemIsUserCheckable;
    }

    return itemFlags;
}

void HotkeyModel::reload()
{
    beginResetModel();
    m_rows = HotkeyManager::instance()->bindings();
    m_baseline = m_rows;
    endResetModel();
}

void HotkeyModel::restoreDefaults()
{
    if (m_rows.isEmpty()) {
        return;
    }

    for (HotkeyBinding &binding : m_rows) {
        binding.sequence = binding.defaultSequence;
        binding.global = binding.defaultGlobal;
    }

    emit dataChanged(index(0, 0), index(static_cast<int>(m_rows.size()) - 1, ColumnCount - 1));
    emit modified();
}

void HotkeyModel::clearShortcut(int row)
{
    if (row < 0 || row >= m_rows.size() || m_rows.at(row).sequence.isEmpty()) {
        return;
    }

    m_rows[row].sequence = QKeySequence();
    m_rows[row].global = false;

    emit dataChanged(index(row, ShortcutColumn), index(row, GlobalColumn));
    emit modified();
}

QKeySequence HotkeyModel::defaultSequenceFor(int row) const
{
    if (row < 0 || row >= m_rows.size()) {
        return {};
    }
    return m_rows.at(row).defaultSequence;
}

bool HotkeyModel::isModified() const
{
    if (m_rows.size() != m_baseline.size()) {
        return true;
    }

    for (int i = 0; i < m_rows.size(); ++i) {
        if (m_rows.at(i).sequence != m_baseline.at(i).sequence
            || m_rows.at(i).global != m_baseline.at(i).global) {
            return true;
        }
    }

    return false;
}

void HotkeyModel::commit()
{
    if (!isModified()) {
        return;
    }

    HotkeyManager::instance()->commit(m_rows);
    reload();
}

int HotkeyModel::rowForSequence(const QKeySequence &sequence, int exceptRow) const
{
    for (int i = 0; i < m_rows.size(); ++i) {
        if (i != exceptRow && m_rows.at(i).sequence == sequence) {
            return i;
        }
    }
    return -1;
}
