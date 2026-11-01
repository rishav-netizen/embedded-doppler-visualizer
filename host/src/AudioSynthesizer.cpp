#include "AudioSynthesizer.hpp"
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

static AudioSynthesizer* g_audio = nullptr;

static void audioCallback(void* buffer, unsigned int frames) {
    if (g_audio) g_audio->processAudio((float*)buffer, frames);
}

AudioSynthesizer::AudioSynthesizer()
    : stream{0}
    , initialized(false)
    , muted(false)
    , targetFrequency(440.0f)
    , boomRequested(false)
    , currentFrequency(440.0f)
    , phase(0.0f)
    , boomLevel(0.0f)
    , noiseSeed(123456789u)
{
}

AudioSynthesizer::~AudioSynthesizer() {
    shutdown();
}

bool AudioSynthesizer::init() {
    g_audio = this;
    InitAudioDevice();
    if (!IsAudioDeviceReady()) return false;

    stream = LoadAudioStream(44100, 32, 1);
    SetAudioStreamCallback(stream, audioCallback);
    PlayAudioStream(stream);
    initialized = true;
    return true;
}

void AudioSynthesizer::shutdown() {
    if (initialized) {
        g_audio = nullptr;
        StopAudioStream(stream);
        UnloadAudioStream(stream);
        CloseAudioDevice();
        initialized = false;
    }
}

void AudioSynthesizer::processAudio(float* buffer, unsigned int frames) {
    float target = targetFrequency.load();
    bool isMuted = muted.load();

    if (boomRequested.exchange(false)) {
        boomLevel = 1.0f;
    }

    for (unsigned int i = 0; i < frames; i++) {
        if (isMuted || target <= 1.0f) {
            buffer[i] = 0.0f;
            continue;
        }

        // Smooth frequency transition so audio doesn't click
        currentFrequency += (target - currentFrequency) * 0.003f;
        phase += (2.0f * PI * currentFrequency) / 44100.0f;
        if (phase > 2.0f * PI) phase -= 2.0f * PI;

        float sample = std::sin(phase) * 0.12f;

        // Lock-free noise generator for sonic boom (avoids rand() mutex lock on macOS)
        if (boomLevel > 0.001f) {
            noiseSeed = noiseSeed * 1664525u + 1013904223u;
            float noise = ((float)(noiseSeed >> 16) / 32768.0f) - 1.0f;
            sample += (std::sin(phase * 0.2f) * 0.3f + noise * 0.2f) * boomLevel;
            boomLevel *= 0.9994f;
        } else {
            boomLevel = 0.0f;
        }

        buffer[i] = sample;
    }
}
