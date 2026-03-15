#ifndef HPLAYER_WHATSNEW_H
#define HPLAYER_WHATSNEW_H

#include <QDialog>


QT_BEGIN_NAMESPACE

namespace Ui {
    class WhatsNewDialog;
}

QT_END_NAMESPACE

class WhatsNewDialog : public QDialog {
    Q_OBJECT

public:
    explicit WhatsNewDialog(QWidget *parent = nullptr);

    ~WhatsNewDialog() override;

private:
    Ui::WhatsNewDialog *ui;

};

#endif //HPLAYER_WHATSNEW_H