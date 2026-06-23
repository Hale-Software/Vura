#include "errordialog.h"
#include "ui_errordialog.h"


ErrorDialog::ErrorDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ErrorDialog)
{
    ui->setupUi(this);

    connect(&ErrorService::instance(), &ErrorService::errorOccurred, this, &ErrorDialog::showError);
    connect(ui->closeButton, &QPushButton::clicked, this, &ErrorDialog::closeButton_Clicked);
    connect(ui->errorActionButton, &QPushButton::clicked, this, &ErrorDialog::errorActionButton_Clicked);

    this->hide();
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}

void ErrorDialog::showError(const PlayerError &error)
{
    ui->errorActionButton->show();

    m_error = error;
    this->setWindowTitle(error.title);
    ui->errorLabel->setText(error.message);

    if (error.action == ErrorAction::None) {
        ui->errorActionButton->hide();

    } else if (error.action == ErrorAction::RetryPlayback) {
        ui->errorActionButton->setText("Retry");

    } else if (error.action == ErrorAction::ReloadLibrary) {
        ui->errorActionButton->setText("Reload Library");

    } else if (error.action == ErrorAction::ReconnectStream) {
        ui->errorActionButton->setText("Reconnect Stream");

    }

    this->show();
}

void ErrorDialog::closeButton_Clicked()
{
    this->hide();
}

void ErrorDialog::errorActionButton_Clicked()
{

}
