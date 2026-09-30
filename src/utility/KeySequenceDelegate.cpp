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

#include "KeySequenceDelegate.h"

#include <libvura/models/hotkey-model.h>

#include <QKeyEvent>
#include <QPointer>
#include <QTimer>


KeySequenceEditor::KeySequenceEditor(QWidget *parent) : QKeySequenceEdit(parent)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    setMaximumSequenceLength(1);
#endif
    setClearButtonEnabled(false);
}

void KeySequenceEditor::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();

    if (key == Qt::Key_Escape) {
        event->accept();
        emit editingCancelled();
        return;
    }

    // Wait for a real key - a bare modifier is not a binding.
    switch (key) {
    case Qt::Key_Control:
    case Qt::Key_Shift:
    case Qt::Key_Alt:
    case Qt::Key_Meta:
    case Qt::Key_AltGr:
    case Qt::Key_unknown:
        event->accept();
        return;
    default:
        break;
    }

    const Qt::KeyboardModifiers modifiers = event->modifiers()
        & (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier | Qt::MetaModifier);

    setKeySequence(QKeySequence(QKeyCombination(modifiers, static_cast<Qt::Key>(key))));
    event->accept();

    emit keyCaptured();
}

void KeySequenceEditor::keyReleaseEvent(QKeyEvent *event)
{
    // The base class uses release + a timeout to finalise multi-chord sequences,
    // which would overwrite what was already captured.
    event->accept();
}


KeySequenceDelegate::KeySequenceDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QWidget *KeySequenceDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    auto *editor = new KeySequenceEditor(parent);
    auto *self = const_cast<KeySequenceDelegate *>(this);
    const QPointer<KeySequenceEditor> guard(editor);

    connect(editor, &KeySequenceEditor::keyCaptured, self, [self, guard]() {
        // Queued so the editor is not torn down while its key event is unwinding.
        // The guard covers the case where the view closed the editor first.
        QTimer::singleShot(0, self, [self, guard]() {
            if (!guard) {
                return;
            }
            emit self->commitData(guard);
            emit self->closeEditor(guard, QAbstractItemDelegate::SubmitModelCache);
        });
    });

    connect(editor, &KeySequenceEditor::editingCancelled, self, [self, guard]() {
        QTimer::singleShot(0, self, [self, guard]() {
            if (!guard) {
                return;
            }
            emit self->closeEditor(guard, QAbstractItemDelegate::RevertModelCache);
        });
    });

    return editor;
}

void KeySequenceDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (auto *keyEditor = qobject_cast<KeySequenceEditor *>(editor)) {
        keyEditor->setKeySequence(index.data(Qt::EditRole).value<QKeySequence>());
    }
}

void KeySequenceDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                       const QModelIndex &index) const
{
    auto *keyEditor = qobject_cast<KeySequenceEditor *>(editor);
    if (!keyEditor) {
        return;
    }

    // The model owns conflict resolution, so a rejected edit simply leaves the row alone.
    model->setData(index, QVariant::fromValue(keyEditor->keySequence()), Qt::EditRole);
}

void KeySequenceDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}
