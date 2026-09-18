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

#include "audio-output.h"

// Pull in the WASAPI GUIDs without a separate .lib
#include <initguid.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>

// Helper: log HRESULT failures with a label
#define CHECK_HR(hr, msg) \
    if (FAILED(hr)) { qWarning() << "AudioOutput:" << (msg) << "hr=" << hr; return false; }

#define CHECK_HR_VOID(hr, msg) \
    if (FAILED(hr)) { qWarning() << "AudioOutput:" << (msg) << "hr=" << hr; return; }

// -------------------------------------------------------------------------
// Target output format — must match what AudioDecoder resamples to
// -------------------------------------------------------------------------
static constexpr int    k_sampleRate  = 48000;
static constexpr int    k_channels    = 2;
static constexpr int    k_bitsPerSamp = 16;          // S16

AudioOutput::AudioOutput(FrameQueue* frameQueue, QObject* parent)
    : QThread(parent), m_frameQueue(frameQueue) {}

AudioOutput::~AudioOutput()
{
    m_abortRequested = true;
    wait();
    releaseResources();
}

// -------------------------------------------------------------------------
// Initialisation (call from main thread before start())
// -------------------------------------------------------------------------

bool AudioOutput::init()
{
    HRESULT hr;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                          CLSCTX_ALL, IID_PPV_ARGS(&m_enumerator));
    CHECK_HR(hr, "CoCreateInstance MMDeviceEnumerator");

    hr = m_enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_device);
    CHECK_HR(hr, "GetDefaultAudioEndpoint");

    hr = m_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL,
                             nullptr, reinterpret_cast<void**>(&m_client));
    CHECK_HR(hr, "Activate IAudioClient");

    // Build the wave format we want
    m_wfx.wFormatTag      = WAVE_FORMAT_PCM;
    m_wfx.nChannels       = k_channels;
    m_wfx.nSamplesPerSec  = k_sampleRate;
    m_wfx.wBitsPerSample  = k_bitsPerSamp;
    m_wfx.nBlockAlign     = (k_channels * k_bitsPerSamp) / 8;
    m_wfx.nAvgBytesPerSec = k_sampleRate * m_wfx.nBlockAlign;
    m_wfx.cbSize          = 0;

    // Shared mode, event-driven, 50 ms buffer
    REFERENCE_TIME bufferDuration = 500000; // 100-ns units → 50 ms
    hr = m_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
                              0,
                              bufferDuration, 0,
                              &m_wfx, nullptr);
    CHECK_HR(hr, "IAudioClient::Initialize");

    hr = m_client->GetBufferSize(&m_bufferFrames);
    CHECK_HR(hr, "GetBufferSize");

    hr = m_client->GetService(IID_PPV_ARGS(&m_render));
    CHECK_HR(hr, "GetService IAudioRenderClient");

    hr = m_client->GetService(IID_PPV_ARGS(&m_clock));
    CHECK_HR(hr, "GetService IAudioClock");

    hr = m_clock->GetFrequency(&m_clockFreq);
    CHECK_HR(hr, "IAudioClock::GetFrequency");

    return true;
}

// -------------------------------------------------------------------------
// Position query — called from the render thread (VideoWidget)
// -------------------------------------------------------------------------

int64_t AudioOutput::getPositionMs() const
{
    if (!m_clock || m_clockFreq == 0) return 0;

    UINT64 pos  = 0;
    UINT64 qpc  = 0;
    const HRESULT hr = m_clock->GetPosition(&pos, &qpc);
    if (FAILED(hr)) return 0;

    // pos is in units of m_clockFreq ticks per second
    return static_cast<int64_t>((pos * 1000ULL) / m_clockFreq);
}

void AudioOutput::setPaused(const bool paused)
{
    m_paused = paused;
}

void AudioOutput::requestFlush()
{
    m_flushRequested = true;
}

// -------------------------------------------------------------------------
// Run loop
// -------------------------------------------------------------------------

void AudioOutput::run()
{
    if (!m_client || !m_render) {
        qWarning() << "AudioOutput: not initialised, thread exiting";
        return;
    }

    m_client->Start();

    while (!m_abortRequested) {

        // --- Flush after seek ---
        if (m_flushRequested.exchange(false)) {
            m_client->Stop();
            m_client->Reset();   // Clears the hardware buffer and resets clock
            m_frameQueue->clear();
            m_client->Start();
        }

        // --- Pause handling ---
        if (m_paused) {
            m_client->Stop();
            while (m_paused && !m_abortRequested && !m_flushRequested)
                QThread::msleep(10);
            if (!m_abortRequested)
                m_client->Start();
            continue;
        }

        // --- How much space is available in the hardware buffer? ---
        UINT32 padding = 0;
        HRESULT hr = m_client->GetCurrentPadding(&padding);
        if (FAILED(hr)) break;

        const UINT32 available = m_bufferFrames - padding;
        if (available == 0) {
            // Buffer is full — sleep for half the buffer duration
            QThread::msleep(10);
            continue;
        }

        // --- Get the next decoded audio frame ---
        AVFrame* frame = m_frameQueue->tryPop();
        if (!frame) {
            QThread::msleep(5);
            continue;
        }

        // frame->data[0] contains interleaved S16 stereo PCM
        // frame->nb_samples is the number of sample frames in this AVFrame
        const int frameSamples = frame->nb_samples;
        constexpr int bytesPerFrame = k_channels * (k_bitsPerSamp / 8);

        // We may need to write in chunks if the AVFrame is larger than
        // the available WASAPI buffer space
        int samplesRemaining = frameSamples;
        const uint8_t* src = frame->data[0];

        while (samplesRemaining > 0 && !m_abortRequested) {
            UINT32 pad2 = 0;
            m_client->GetCurrentPadding(&pad2);
            UINT32 canWrite = m_bufferFrames - pad2;

            if (canWrite == 0) {
                QThread::msleep(5);
                continue;
            }

            const UINT32 toWrite = qMin(canWrite, static_cast<UINT32>(samplesRemaining));

            BYTE* dest = nullptr;
            hr = m_render->GetBuffer(toWrite, &dest);
            if (FAILED(hr)) break;

            memcpy(dest, src, toWrite * bytesPerFrame);
            m_render->ReleaseBuffer(toWrite, 0);

            src              += toWrite * bytesPerFrame;
            samplesRemaining -= static_cast<int>(toWrite);
        }

        av_frame_free(&frame);
    }

    m_client->Stop();
}

// -------------------------------------------------------------------------
// Cleanup
// -------------------------------------------------------------------------

void AudioOutput::releaseResources()
{
    if (m_clock)      { m_clock->Release();      m_clock      = nullptr; }
    if (m_render)     { m_render->Release();     m_render     = nullptr; }
    if (m_client)     { m_client->Release();     m_client     = nullptr; }
    if (m_device)     { m_device->Release();     m_device     = nullptr; }
    if (m_enumerator) { m_enumerator->Release(); m_enumerator = nullptr; }
}
