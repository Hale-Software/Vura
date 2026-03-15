//
// Created by halea on 1/30/2026.
//

#ifndef HPLAYER_MEDIAINFORMATION_H
#define HPLAYER_MEDIAINFORMATION_H

#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui {
    class MediaInformation;
}

QT_END_NAMESPACE

class MediaInformation : public QWidget {
    Q_OBJECT

public:
    explicit MediaInformation(QWidget *parent = nullptr);

    ~MediaInformation() override;

private:
    Ui::MediaInformation *ui;
};


#endif //HPLAYER_MEDIAINFORMATION_H