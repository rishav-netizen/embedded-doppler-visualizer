#include "WavefrontPool.hpp"

WavefrontPool::WavefrontPool() : nextIndex(0), activeCount(0) {
    clear();
}

void WavefrontPool::emitWave(Vector2 pos) {
    // Grab the next slot in our circular array
    waves[nextIndex].origin = pos;
    waves[nextIndex].radius = 2.0f;
    waves[nextIndex].alpha = 1.0f;
    waves[nextIndex].active = true;

    // Wrap index around when it reaches 300
    nextIndex = (nextIndex + 1) % MAX_WAVES;
}

void WavefrontPool::update(float speedOfSound, float dt) {
    activeCount = 0;
    for (int i = 0; i < MAX_WAVES; i++) {
        if (waves[i].active) {
            waves[i].update(speedOfSound, dt);
            if (waves[i].active) {
                activeCount++;
            }
        }
    }
}

void WavefrontPool::render(Color waveColor) const {
    for (int i = 0; i < MAX_WAVES; i++) {
        if (waves[i].active) {
            waves[i].draw(waveColor);
        }
    }
}

void WavefrontPool::clear() {
    for (int i = 0; i < MAX_WAVES; i++) {
        waves[i].active = false;
        waves[i].radius = 0.0f;
        waves[i].alpha = 0.0f;
    }
    nextIndex = 0;
    activeCount = 0;
}
