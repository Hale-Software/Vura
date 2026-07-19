#pragma once
#include <QObject>
#include "ErrorTypes.h"


class ErrorService : public QObject {
    Q_OBJECT

public:
    static ErrorService& instance() {
        static ErrorService inst;
        return inst;
    }

    // Call this from anywhere (Media Engine, File IO, etc.)
    void postError(const PlayerError& error) {
        emit errorOccurred(error);
    }

signals:
        void errorOccurred(const PlayerError& error);

private:
    ErrorService() = default;

};
