#include "hotkeywidget.h"
#include "ui_HotkeyWidget.h"

#include "settingswindow.h"


HotkeyWidget::HotkeyWidget(QString action, QString defaultHotkey, QString hotkey, QWidget *parent) : QWidget(parent), ui(new Ui::HotkeyWidget)
{
    ui->setupUi(this);

    h_action = action;
    h_defaultHotkey = defaultHotkey;
    h_hotkey = hotkey;

    QString hotkeyBox_ToolTip = "Hotkey combination for " + action + ".";
    ui->actionLabel->setText(action);
    ui->hotkeyBox->setKeySequence(hotkey);

    connect(ui->hotkeyBox, &QKeySequenceEdit::keySequenceChanged, this, &HotkeyWidget::keySequence_Changed);
    connect(ui->resetButton, &QToolButton::clicked, this, &HotkeyWidget::revertButton_Clicked);
    connect(ui->clearButton, &QToolButton::clicked, this, &HotkeyWidget::clearButton_Clicked);
}

HotkeyWidget::~HotkeyWidget()
{
    delete ui;
}

QString HotkeyWidget::getHotkey()
{
    return h_hotkey;
}

void HotkeyWidget::setHotkey(QString hotkey)
{
    h_hotkey = hotkey;
    ui->hotkeyBox->setKeySequence(hotkey);
}

void HotkeyWidget::clearHotkey()
{
    h_hotkey.clear();
    ui->hotkeyBox->clear();
}

void HotkeyWidget::keySequence_Changed(const QKeySequence &keySequence)
{
    QString newHotkey = keySequence.toString();
    if (newHotkey != h_hotkey)
        emit hotkeyChanged(h_action, h_hotkey, newHotkey);
}

void HotkeyWidget::revertButton_Clicked()
{
    if (ui->hotkeyBox->keySequence() != h_defaultHotkey)
        ui->hotkeyBox->setKeySequence(h_defaultHotkey);
}

void HotkeyWidget::clearButton_Clicked()
{
    h_hotkey.clear();
    ui->hotkeyBox->clear();
}
