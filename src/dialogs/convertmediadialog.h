#ifndef VURA_CONVERTMEDIADIALOG_H
#define VURA_CONVERTMEDIADIALOG_H

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


#endif //VURA_CONVERTMEDIADIALOG_H
