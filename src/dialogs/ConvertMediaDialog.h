#pragma once

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ConvertMediaDialog;
}
QT_END_NAMESPACE

class ConvertMediaDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConvertMediaDialog(QWidget *parent = nullptr, const QString &initialOption = "Convert/Save");
    ~ConvertMediaDialog() override;

private:
    Ui::ConvertMediaDialog *ui;
    QString m_initialOption = "Convert/Save";

};
