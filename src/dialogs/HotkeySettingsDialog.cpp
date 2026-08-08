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

#include "HotkeySettingsDialog.h"


HotkeySettingsDialog::HotkeySettingsDialog(QMap<QString, QAction*> &actions, QWidget *parent)
        : QDialog(parent), actionsRef(actions) {

    setWindowTitle("Vura - Keyboard Shortcuts");
    resize(500, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Setup Table
    table = new QTableWidget(actions.size(), 2, this);
    table->setHorizontalHeaderLabels({"Action", "Shortcut"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Apply our custom delegate to the "Shortcut" column (Column 1)
    table->setItemDelegateForColumn(1, new HotkeyDelegate(this));

    // Populate Table
    int row = 0;
    for (auto it = actions.constBegin(); it != actions.constEnd(); ++it) {
        // Column 0: Action Name (Read-only)
        QTableWidgetItem *nameItem = new QTableWidgetItem(it.key());
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable); // Make un-editable
        table->setItem(row, 0, nameItem);

        // Column 1: Shortcut (Editable via Delegate)
        QString currentShortcut = it.value()->shortcut().toString();
        QTableWidgetItem *shortcutItem = new QTableWidgetItem(currentShortcut);
        table->setItem(row, 1, shortcutItem);

        row++;
    }

    layout->addWidget(table);

    // Save Button
    QPushButton *applyBtn = new QPushButton("Apply", this);
    connect(applyBtn, &QPushButton::clicked, this, &HotkeySettingsDialog::applyChanges);
    layout->addWidget(applyBtn);
}

void HotkeySettingsDialog::applyChanges() {
    // We will keep track of whether any changes were actually made
    bool changesMade = false;

    for (int row = 0; row < table->rowCount(); ++row) {
        QString actionName = table->item(row, 0)->text();
        QString newShortcutStr = table->item(row, 1)->text();
        QKeySequence newShortcut(newShortcutStr);

        if (!actionsRef.contains(actionName)) continue;

        QAction *currentAction = actionsRef[actionName];

        // If the shortcut hasn't changed, skip to the next row
        if (currentAction->shortcut() == newShortcut) continue;

        // --- CONFLICT CHECK START ---
        // If the new shortcut is not empty, check if it's already used by another action
        if (!newShortcut.isEmpty()) {
            QString conflictingActionName = "";

            // Scan our reference map for conflicts
            for (auto it = actionsRef.constBegin(); it != actionsRef.constEnd(); ++it) {
                // Ignore the action we are currently trying to modify
                if (it.key() == actionName) continue;

                if (it.value()->shortcut() == newShortcut) {
                    conflictingActionName = it.key();
                    break; // Found a conflict
                }
            }

            // If we found a conflict, prompt the user
            if (!conflictingActionName.isEmpty()) {
                QString warningMsg = QString("The shortcut '%1' is already assigned to '%2'.\n\n"
                                             "Do you want to reassign it to '%3'?")
                                             .arg(newShortcutStr, conflictingActionName, actionName);

                QMessageBox::StandardButton reply;
                reply = QMessageBox::warning(this, "Shortcut Conflict", warningMsg,
                                             QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    // User chose to overwrite.
                    // Clear the shortcut from the conflicting action.
                    actionsRef[conflictingActionName]->setShortcut(QKeySequence());

                    // Optional: Update the table UI so it visually reflects the cleared shortcut
                    for (int r = 0; r < table->rowCount(); ++r) {
                        if (table->item(r, 0)->text() == conflictingActionName) {
                            table->item(r, 1)->setText("");
                            break;
                        }
                    }
                } else {
                    // User chose NOT to overwrite.
                    // Revert the UI table cell back to what the action originally had.
                    table->item(row, 1)->setText(currentAction->shortcut().toString());
                    continue; // Skip saving this particular row
                }
            }
        }
        // --- CONFLICT CHECK END ---

        // If we made it here, there is no conflict (or the user chose to overwrite it)
        // Apply the new shortcut to the action
        currentAction->setShortcut(newShortcut);
        changesMade = true;
    }

    // Only serialize to the Vura .vhk file if actual changes occurred
    if (changesMade) {
        // saveToVuraRegistry();
        VuraHotkeys hotkeys;
        hotkeys.updateMenuItemHotkeys(actionsRef);
    }

    accept(); // Close the dialog
}
