#pragma once

#include <QWidget>
#include <QString>
#include <QEvent>
#include <QEnterEvent>

class QPushButton;
class QToolButton;
class QLabel;
class QTime;

QT_BEGIN_NAMESPACE
namespace Ui {
class ContinuePlaybackWidget;
}
QT_END_NAMESPACE


class ContinuePlaybackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContinuePlaybackWidget(qint64 savedPosition, QWidget *parent = nullptr);
    ~ContinuePlaybackWidget() override;

signals:
    void continuePlayback(qint64 savedPosition);
    void closeWidget();
    void mouseEntered();
    void mouseLeft();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void continuePlaybackClicked();
    void closeWidgetClicked();

private:
    Ui::ContinuePlaybackWidget *ui;

    qint64 m_savedPosition;

};
