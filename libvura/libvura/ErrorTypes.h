#pragma once
#include <QString>

enum class ErrorSeverity { Info, Warning, Critical };

enum class ErrorAction { None, RetryPlayback, ReloadLibrary, ReconnectStream };

struct PlayerError {
    QString title;
    QString message;
    ErrorSeverity severity;
    ErrorAction action = ErrorAction::None;
};
