#pragma once

#include <QSize>

#include "enums.h"


class VideoOutput
{
public:
    virtual ~VideoOutput() = default;
    virtual OutputKind kind() const = 0;
    virtual QSize surfaceSize() const = 0;
    virtual void requestRedraw() = 0;

};
