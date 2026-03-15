#ifndef HPLAYER_PERMISSIONS_H
#define HPLAYER_PERMISSIONS_H

#include <QDialog>


QT_BEGIN_NAMESPACE

namespace Ui {
    class PermissionsDialog;
}

QT_END_NAMESPACE

class PermissionsDialog : public QDialog {
    Q_OBJECT

public:
    explicit PermissionsDialog(QWidget *parent = nullptr);

    ~PermissionsDialog() override;

private:
    Ui::PermissionsDialog *ui;

};

#endif //HPLAYER_PERMISSIONS_H