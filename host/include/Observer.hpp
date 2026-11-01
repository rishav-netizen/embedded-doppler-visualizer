#pragma once
#include <raylib.h>

// Stationary or mouse-draggable microphone that measures the Doppler pitch shift
class Observer {
public:
    Observer(Vector2 startPos, float restingFreq = 440.0f);

    void update();
    float computeObservedFrequency(Vector2 sourcePos, Vector2 sourceVel, float speedOfSound) const;
    void render(Vector2 sourcePos) const;

    Vector2 getPosition() const { return position; }
    float getBaseFrequency() const { return baseFrequency; }

private:
    Vector2 position;
    float baseFrequency;
    bool dragging;
};
