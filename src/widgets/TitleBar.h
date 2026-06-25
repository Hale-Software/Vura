#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDebug>


namespace Ui {
class TitleBar;
}

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

private slots:
    void minimizeButton_clicked();
    void maximizeButton_clicked();
    void closeButton_clicked();

signals:
    void minimizeRequested();
    void maximizeRequested();
    void closeRequested();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::TitleBar *ui;

    QPoint m_dragPosition;

};
