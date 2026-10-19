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

#include <QWidget>
#include <QModelIndex>

class QSettings;
class QSortFilterProxyModel;
class QDebug;

class HotkeyModel;
class KeySequenceDelegate;


QT_BEGIN_NAMESPACE
namespace Ui {
class HotkeysSettings;
}
QT_END_NAMESPACE

class HotkeysSettings : public QWidget
{
    Q_OBJECT

public:
    explicit HotkeysSettings(QWidget *parent = nullptr);
    ~HotkeysSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void search_Changed(const QString &text);
    void searchScope_Changed(int index);
    void selection_Changed();
    void clearHotkey_Clicked();
    void restoreDefault_Clicked();
    void model_Modified();
    void model_ErrorMessage(const QString &message);

private:
    int currentSourceRow() const;
    bool confirmReassignment(const QString &requestingLabel, const QString &currentOwnerLabel,
                             const QKeySequence &sequence);

    Ui::HotkeysSettings *ui;

    HotkeyModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxyModel = nullptr;
    KeySequenceDelegate *m_delegate = nullptr;

};
