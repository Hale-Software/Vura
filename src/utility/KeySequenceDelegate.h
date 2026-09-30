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

#include <QKeySequenceEdit>
#include <QStyledItemDelegate>


/**
 * Single-shot key grabber.
 *
 * QKeySequenceEdit on its own records up to four chords and waits on a timeout,
 * which is wrong for a media player where every binding is one key or one chord.
 * This captures the first real key press and finishes immediately.
 *
 * Escape cancels the edit, so Escape itself cannot be bound from this widget.
 */
class KeySequenceEditor : public QKeySequenceEdit
{
    Q_OBJECT

public:
    explicit KeySequenceEditor(QWidget *parent = nullptr);

signals:
    void keyCaptured();
    void editingCancelled();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};


class KeySequenceDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit KeySequenceDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
};
