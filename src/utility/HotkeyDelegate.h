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

#include <QStyledItemDelegate>
#include <QKeySequenceEdit>


class HotkeyDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit HotkeyDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    // 1. Provide the QKeySequenceEdit when editing starts
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QKeySequenceEdit *editor = new QKeySequenceEdit(parent);
        return editor;
    }

    // 2. Set the current shortcut into the editor
    void setEditorData(QWidget *editor, const QModelIndex &index) const override {
        QString currentShortcut = index.model()->data(index, Qt::EditRole).toString();
        QKeySequenceEdit *keyEditor = qobject_cast<QKeySequenceEdit *>(editor);
        if (keyEditor) {
            keyEditor->setKeySequence(QKeySequence(currentShortcut));
        }
    }

    // 3. Save the new shortcut back to the table when the user finishes
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override {
        QKeySequenceEdit *keyEditor = qobject_cast<QKeySequenceEdit *>(editor);
        if (keyEditor) {
            model->setData(index, keyEditor->keySequence().toString(), Qt::EditRole);
        }
    }
};

