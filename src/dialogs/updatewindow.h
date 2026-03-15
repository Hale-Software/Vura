#ifndef HPLAYER_UPDATEWINDOW_H
#define HPLAYER_UPDATEWINDOW_H

#include <QDialog>


QT_BEGIN_NAMESPACE

namespace Ui {
    class UpdateDialog;
}

QT_END_NAMESPACE

class UpdateDialog : public QDialog {
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = nullptr);

    ~UpdateDialog() override;

private:
    Ui::UpdateDialog *ui;

};

#endif //HPLAYER_UPDATEWINDOW_H