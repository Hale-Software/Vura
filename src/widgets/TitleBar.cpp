#include "TitleBar.h"
#include "ui_TitleBar.h"


TitleBar::TitleBar(QWidget *parent) : QWidget(parent), ui(new Ui::TitleBar)
{
    ui->setupUi(this);
    connect(ui->minimizeButton, &QPushButton::clicked, this, &TitleBar::minimizeButton_clicked);
    connect(ui->maximizeButton, &QPushButton::clicked, this, &TitleBar::maximizeButton_clicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &TitleBar::closeButton_clicked);
}

TitleBar::~TitleBar()
{
    delete ui;
}

void TitleBar::minimizeButton_clicked()
{
    emit minimizeRequested();
}

void TitleBar::maximizeButton_clicked()
{
    emit maximizeRequested();
}

void TitleBar::closeButton_clicked()
{
    emit closeRequested();
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && this->geometry().contains(event->pos())) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && !m_dragPosition.isNull()) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}
