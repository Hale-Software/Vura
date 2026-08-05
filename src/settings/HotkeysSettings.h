//
// Created by halea on 8/5/2026.
//

#ifndef VURA_HOTKEYSSETTINGS_H
#define VURA_HOTKEYSSETTINGS_H

#include <QWidget>


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

private:
    Ui::HotkeysSettings *ui;
};


#endif //VURA_HOTKEYSSETTINGS_H
