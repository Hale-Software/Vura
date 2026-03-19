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

#include <QApplication>
#include <QMessageBox>
#include <QWidget>
#include <QWindow>
#include <QThread>
#include <QObject>
#include <QLayout>
#include <QComboBox>
#include <QDataStream>
#include <QKeyEvent>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>

#define QT_UTF8(str) QString::fromUtf8(str, -1)
#define QT_TO_UTF8(str) str.toUtf8().constData()
#define MAX_LABEL_LENGTH 80

class QDataStream;
class QComboBox;
class QWidget;
class QLayout;
class QString;
class QLabel;
class QToolBar;

class VuraMessageBox : public QObject {
    Q_OBJECT

public:
    static QMessageBox::StandardButton question(
            QWidget *parent, const QString &title, const QString &text,
            QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No),
            QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static void information(QWidget *parent, const QString &title, const QString &text);
    static void warning(QWidget *parent, const QString &title, const QString &text, bool enableRichText = false);
    static void critical(QWidget *parent, const QString &title, const QString &text);

};

void VuraErrorBox(QWidget *parent, const char *msg, ...);
