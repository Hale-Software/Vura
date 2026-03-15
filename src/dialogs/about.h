#ifndef HABOUT_H
#define HABOUT_H

#include <QDialog>
#include <QLabel>
#include <QTextBrowser>
#include "../components/ClickableLabel.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class AboutDialog;
}

QT_END_NAMESPACE

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);

    ~AboutDialog() override;

private:
    Ui::AboutDialog *ui;

};

#endif //HABOUT_H
