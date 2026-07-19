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

