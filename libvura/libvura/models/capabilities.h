#pragma once

#include <qtypes.h>


struct Capabilities
{
    bool variableRate = false;
    bool pitchCorrection = false;
    bool externalSubtitles = false;
    bool gapless = false;
    bool hardwareDecode = false;
    bool audioDeviceSelect = false;
    std::pair<qreal, qreal> rateRange = { 1.0, 1.0 };
};
