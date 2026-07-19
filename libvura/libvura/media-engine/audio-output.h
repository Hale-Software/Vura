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

#pragma once

#include <QThread>
#include <QDebug>
#include <atomic>
#include "frame-queue.h"

// Windows / WASAPI
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

/**
 * AudioOutput
 *
 * Owns the WASAPI exclusive-mode (or shared-mode) audio client.
 * Runs on its own thread, popping decoded PCM frames from m_frameQueue
 * and writing them to the hardware buffer.
 *
 * Exposes getPositionMs() so VideoWidget can use the audio clock
 * as the A/V sync master instead of QElapsedTimer.
 *
 * Expected input format (produced by AudioDecoder):
 *   Sample format : AV_SAMPLE_FMT_S16
 *   Sample rate   : 48000 Hz
 *   Channels      : 2 (stereo)
 *
 * These match what WASAPI shared mode almost universally negotiates.
 */
class AudioOutput : public QThread
{
    Q_OBJECT

public:
    explicit AudioOutput(FrameQueue* frameQueue, QObject* parent = nullptr);
    ~AudioOutput() override;

    /**
     * Initialise the WASAPI device and audio client.
     * Must be called from the main thread before start().
     * Returns false if no audio device is available.
     */
    bool init();

    /**
     * Returns the current audio playback position in milliseconds.
     * Queries IAudioClock for hardware-accurate position.
     * Thread-safe — can be called from the render thread.
     */
    int64_t getPositionMs() const;

    /**
     * Pause / resume the WASAPI stream.
     * Thread-safe via atomic flag; the run() loop checks it.
     */
    void setPaused(bool paused);

    /**
     * Flush the audio queue and reset the position after a seek.
     * Sets an atomic flag; the run() loop drains and re-anchors.
     */
    void requestFlush();

protected:
    void run() override;

private:
    void releaseResources();

    FrameQueue*       m_frameQueue    = nullptr;

    // WASAPI interfaces
    IMMDeviceEnumerator* m_enumerator = nullptr;
    IMMDevice*           m_device     = nullptr;
    IAudioClient*        m_client     = nullptr;
    IAudioRenderClient*  m_render     = nullptr;
    IAudioClock*         m_clock      = nullptr;

    WAVEFORMATEX         m_wfx        = {};
    UINT32               m_bufferFrames = 0;

    std::atomic<bool>    m_paused{false};
    std::atomic<bool>    m_flushRequested{false};
    std::atomic<bool>    m_abortRequested{false};

    // Position tracking
    // IAudioClock gives position in units of m_clockFreq ticks.
    // We cache frequency once at init time to avoid per-call QPC overhead.
    UINT64               m_clockFreq  = 0;

};
