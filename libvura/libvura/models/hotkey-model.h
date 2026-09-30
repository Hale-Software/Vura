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

#pragma once

#include <QAbstractTableModel>
#include <QKeySequence>
#include <QList>

#include <functional>

#include "../hotkeys/hotkey-manager.h"


/**
 * Editable working copy of HotkeyManager's binding table.
 *
 * Nothing here touches QSettings or the live QActions until commit() is called,
 * which keeps the settings dialog's Apply / Cancel semantics honest.
 */
class HotkeyModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ActionColumn = 0,
        CategoryColumn,
        ShortcutColumn,
        GlobalColumn,
        ColumnCount
    };

    /**
     * Asked to arbitrate before a sequence is stolen from another command.
     * Return true to reassign, false to abandon the edit.
     */
    using ConflictResolver = std::function<bool(const QString &requestingLabel,
                                                const QString &currentOwnerLabel,
                                                const QKeySequence &sequence)>;

    explicit HotkeyModel(QObject *parent = nullptr);

    void setConflictResolver(ConflictResolver resolver) { m_conflictResolver = std::move(resolver); }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /** Discards local edits and re-reads the committed table. */
    void reload();

    /** Sets every row back to its Designer default, in the working copy only. */
    void restoreDefaults();

    /** Clears one row's binding (bound to the Delete key in the view). */
    void clearShortcut(int row);

    /** The factory default for a row, so the page can restore it through setData(). */
    QKeySequence defaultSequenceFor(int row) const;

    bool isModified() const;
    void commit();

signals:
    void modified();
    void errorMessage(const QString &message);

private:
    int rowForSequence(const QKeySequence &sequence, int exceptRow) const;

    QList<HotkeyBinding> m_rows;
    QList<HotkeyBinding> m_baseline;
    ConflictResolver m_conflictResolver;
};
