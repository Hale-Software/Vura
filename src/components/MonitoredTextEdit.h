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

#pragma once

#include <QTextEdit>


class MonitoredTextEdit : public QTextEdit {
    Q_OBJECT

public:
    MonitoredTextEdit(QWidget* parent = nullptr) : QTextEdit(parent) {}

signals:
    void startEditing();
    void editingFinished();

protected:
    void focusOutEvent(QFocusEvent *event) override {
        QTextEdit::focusOutEvent(event);
        emit editingFinished();
    }

    void focusInEvent(QFocusEvent *event) override
    {
        QTextEdit::focusInEvent(event);
        emit startEditing();
    }

};
