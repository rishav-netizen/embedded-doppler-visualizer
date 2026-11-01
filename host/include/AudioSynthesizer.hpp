#pragma once
#include <raylib.h>
#include <atomic>

// Generates a real-time sine wave tone at the perceived Doppler frequency
class AudioSynthesizer {
public:
    AudioSynthesizer();
    ~AudioSynthesizer();

    bool init();
    void shutdown();

    void setTargetFrequency(float freq) { targetFrequency.store(freq); }
    void triggerSonicBoom() { boomRequested.store(true); }
    void toggleMute() { muted.store(!muted.load()); }
    bool isMuted() const { return muted.load(); }

    void processAudio(float* buffer, unsigned int frames);

private:
    AudioStream stream;
    bool initialized;
    std::atomic<bool> muted;
    std::atomic<float> targetFrequency;
    std::atomic<bool> boomRequested;

    float currentFrequency;
    float phase;
    float boomLevel;
    unsigned int noiseSeed;
};
