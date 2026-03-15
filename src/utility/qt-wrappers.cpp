#include "qt-wrappers.h"

#include <QWidget>
#include <QLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QDataStream>
#include <QKeyEvent>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>


static inline void VuraErrorBoxva(QWidget *parent, const char *msg, va_list args)
{
    char full_message[8192];
    vsnprintf(full_message, sizeof(full_message), msg, args);

    QMessageBox::critical(parent, "Error", full_message);
}

void VuraErrorBox(QWidget *parent, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    VuraErrorBoxva(parent, msg, args);
    va_end(args);
}

QMessageBox::StandardButton VuraMessageBox::question(QWidget *parent, const QString &title, const QString &text,
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

void VuraMessageBox::information(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox mb(QMessageBox::Information, title, text, QMessageBox::NoButton, parent);
    mb.addButton(tr("OK"), QMessageBox::AcceptRole);
    mb.exec();
}

void VuraMessageBox::warning(QWidget *parent, const QString &title, const QString &text, bool enableRichText)
{
    QMessageBox mb(QMessageBox::Warning, title, text, QMessageBox::NoButton, parent);
    if (enableRichText)
        mb.setTextFormat(Qt::RichText);
    mb.addButton(tr("OK"), QMessageBox::AcceptRole);
    mb.exec();
}

void VuraMessageBox::critical(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox mb(QMessageBox::Critical, title, text, QMessageBox::NoButton, parent);
    mb.addButton(tr("OK"), QMessageBox::AcceptRole);
    mb.exec();
}
