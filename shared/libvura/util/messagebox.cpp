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

#include "messagebox.h"


static inline void ErrorBoxva(QWidget *parent, const char *msg, va_list args)
{
    char full_message[8192];
    vsnprintf(full_message, sizeof(full_message), msg, args);

    QMessageBox::critical(parent, "Error", full_message);
}

void ErrorBox(QWidget *parent, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    ErrorBoxva(parent, msg, args);
    va_end(args);
}

QMessageBox::StandardButton VMessageBox::question(QWidget *parent, const QString &title, const QString &text,
                            QMessageBox::StandardButtons buttons,
                            QMessageBox::StandardButton defaultButton)
{
    QMessageBox mb(QMessageBox::Question, title, text, QMessageBox::NoButton, parent);
    mb.setDefaultButton(defaultButton);

    if (buttons & QMessageBox::Ok) {
        QPushButton *button = mb.addButton(QMessageBox::Ok);
        button->setText(tr("OK"));
    }
#define add_button(x)                                               \
if (buttons & QMessageBox::x) {                             \
QPushButton *button = mb.addButton(QMessageBox::x); \
button->setText(tr(#x));                            \
}
    add_button(Open);
    add_button(Save);
    add_button(Cancel);
    add_button(Close);
    add_button(Discard);
    add_button(Apply);
    add_button(Reset);
    add_button(Yes);
    add_button(No);
    add_button(Abort);
    add_button(Retry);
    add_button(Ignore);
#undef add_button
    return (QMessageBox::StandardButton)mb.exec();
}

void VMessageBox::information(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox mb(QMessageBox::Information, title, text, QMessageBox::NoButton, parent);
    mb.addButton(tr("OK"), QMessageBox::AcceptRole);
    mb.exec();
}

void VMessageBox::warning(QWidget *parent, const QString &title, const QString &text, bool enableRichText)
{
    QMessageBox mb(QMessageBox::Warning, title, text, QMessageBox::NoButton, parent);
    if (enableRichText)
        mb.setTextFormat(Qt::RichText);
    mb.addButton(tr("OK"), QMessageBox::AcceptRole);
    mb.exec();
}

void VMessageBox::critical(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox mb(QMessageBox::Critical, title, text, QMessageBox::NoButton, parent);
    mb.addButton(tr("OK"), QMessageBox::AcceptRole);
    mb.exec();
}
