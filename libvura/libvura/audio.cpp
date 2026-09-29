/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "audio.h"
#include "logging/categories.h"

#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioFormat>
#include <QString>
#include <QDebug>


void logDefaultAudioDeviceSpecs()
{
    qCInfo(Core) << "Hardware Audio Specifications:";

    QAudioDevice defaultOutput = QMediaDevices::defaultAudioOutput();
    if (!defaultOutput.isNull()) {
        qCInfo(Core) << "-  Name: " << defaultOutput.description();
        qCInfo(Core) << "-  Default: " << (defaultOutput.isDefault() ? "true" : "false");

        int minSampleRate = defaultOutput.minimumSampleRate();
        int maxSampleRate = defaultOutput.maximumSampleRate();
        QString samplesPerSecondStr = QString("Samples Per Second: Support ranges from %1 Hz to %2 Hz")
                                             .arg(minSampleRate)
                                             .arg(maxSampleRate);
        qCInfo(Core) << "-  " << samplesPerSecondStr;

        int minChannels = defaultOutput.minimumChannelCount();
        int maxChannels = defaultOutput.maximumChannelCount();
        QString channelCountStr = QString("Num Speakers/Channels: Supports up to %1 channel(s) (Min: %2)")
                                         .arg(maxChannels)
                                         .arg(minChannels);
        qCInfo(Core) << "-  " << channelCountStr;

        if (maxChannels >= 6) {
            qCInfo(Core) << "-  Channel Layout Capability: Surrond Sound (5.1 or greater)";
        } else if (maxChannels == 2) {
            qCInfo(Core) << "-  Channel Layout Capability: Stereo";
        } else {
            qCInfo(Core) << "-  Channel Layout Capability: Mono";
        }

        qCInfo(Core) << "-  Supported Sample Formats:";
        QList<QAudioFormat::SampleFormat> formats = defaultOutput.supportedSampleFormats();
        for (auto format : formats) {
            switch (format) {
                case QAudioFormat::UInt8:
                    qCInfo(Core) << "-      Unsigned 8-bit Integer";
                    break;
                case QAudioFormat::Int16:
                    qCInfo(Core) << "-      Signed 16-bit Integer";
                    break;
                case QAudioFormat::Int32:
                    qCInfo(Core) << "-      Signed 32-bit Integer";
                    break;
                case QAudioFormat::Float:
                    qCInfo(Core) << "-      32-bit Floating Point";
                    break;
                default:
                    qCInfo(Core) << "-      Unknown Format";
                    break;
            }
        }

        QAudioFormat preferredFormat = defaultOutput.preferredFormat();
        int bytesPerFrame = preferredFormat.bytesPerFrame();
        int preferredSampleRate = preferredFormat.sampleRate();
        int minBufferSize = bytesPerFrame * (preferredSampleRate * 0.02);
        int maxBufferSize = bytesPerFrame * (preferredSampleRate * 0.20);

        qCInfo(Core) << "-  Buffering Profile:";
        qCInfo(Core) << "-    Buffering Type: Dynamic ring buffer allocated via underlying OS API ("
#if defined(Q_OS_WIN)
        << "WASAPI"
#elif defined(Q_OS_MAC)
        << "CoreAudio"
#elif defined(Q_OS_LINUX)
        << "PulseAudio / PipeWire / ALSA"
#else
        << "Generic Multimedia Layer"
#endif
        << ")";
        qCInfo(Core) << "-    Preferred Frame Payload Byte Size: " << bytesPerFrame << " bytes per frame";
        qCInfo(Core) << "-    Recommended Low-Latency Safe Buffer: " << minBufferSize << " bytes (~20ms)";
        qCInfo(Core) << "-    Recommended Maximum Safe Buffer Window: " << maxBufferSize << " bytes (~200ms)";
    } else {
        qCInfo(Core) << "-  Error: No audio output hardware device detected.";
    }
    qCInfo(Core) << "---------------------------------";
}
