#pragma once
#include "Wavefront.hpp"

// Object Pool: reuses a fixed array of 300 waves instead of using new/delete every frame
class WavefrontPool {
public:
    static const int MAX_WAVES = 300;

    WavefrontPool();

    void emitWave(Vector2 pos);
    void update(float speedOfSound, float dt);
    void render(Color waveColor) const;
    void clear();
    int getActiveCount() const { return activeCount; }

private:
    Wavefront waves[MAX_WAVES];
    int nextIndex;
    int activeCount;
};
