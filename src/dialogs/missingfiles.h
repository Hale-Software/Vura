#ifndef HPLAYER_MISSINGFILES_H
#define HPLAYER_MISSINGFILES_H

#include <QDialog>


QT_BEGIN_NAMESPACE

namespace Ui {
    class MissingFilesDialog;
}

QT_END_NAMESPACE

class MissingFilesDialog : public QDialog {
    Q_OBJECT

public:
    explicit MissingFilesDialog(QWidget *parent = nullptr);

    ~MissingFilesDialog() override;

private:
    Ui::MissingFilesDialog *ui;

};

#endif //HPLAYER_MISSINGFILES_H