#ifndef HPLAYER_HOTKEYWIDGET_H
#define HPLAYER_HOTKEYWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QKeySequenceEdit>
#include <QKeySequence>
#include <QToolButton>


QT_BEGIN_NAMESPACE

namespace Ui {
    class HotkeyWidget;
}

QT_END_NAMESPACE

class HotkeyWidget : public QWidget {
    Q_OBJECT

public:
    explicit HotkeyWidget(QString action, QString defaultHotkey, QString hotkey, QWidget *parent = nullptr);
    ~HotkeyWidget() override;

    QString getHotkey();
    void setHotkey(QString hotkey);
    void clearHotkey();

signals:
    void hotkeyChanged(QString action, QString oldHotkey, QString newHotkey);

private slots:
    void keySequence_Changed(const QKeySequence &keySequence);
    void revertButton_Clicked();
    void clearButton_Clicked();

private:
    Ui::HotkeyWidget *ui;
    QString h_action;
    QString h_defaultHotkey;
    QString h_hotkey;

};


#endif //HPLAYER_HOTKEYWIDGET_H
