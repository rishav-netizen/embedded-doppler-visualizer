#include "Observer.hpp"
#include <cmath>
#include <algorithm>

Observer::Observer(Vector2 startPos, float restingFreq)
    : position(startPos)
    , baseFrequency(restingFreq)
    , dragging(false)
{
}

void Observer::update() {
    Vector2 mouse = GetMousePosition();

    // Drag listener around with left mouse button
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(mouse, position, 22.0f)) {
        dragging = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        dragging = false;
    }
    if (dragging) {
        position = mouse;
    }
}

float Observer::computeObservedFrequency(Vector2 sourcePos, Vector2 sourceVel, float speedOfSound) const {
    // 1. Vector from sound source to listener
    float dx = position.x - sourcePos.x;
    float dy = position.y - sourcePos.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 5.0f) return baseFrequency;

    // 2. Radial velocity component towards the listener (dot product)
    float vRadial = (sourceVel.x * (dx / dist)) + (sourceVel.y * (dy / dist));

    // 3. Classical Doppler equation: f_obs = f0 * (c / (c - v_radial))
    float denom = std::max(15.0f, speedOfSound - vRadial);
    float fObs = baseFrequency * (speedOfSound / denom);

    return std::clamp(fObs, 100.0f, 2000.0f);
}

void Observer::render(Vector2 sourcePos) const {
    // Dashed/faint sightline from source to listener
    DrawLineEx(sourcePos, position, 1.2f, {86, 95, 137, 130});

    Color green = {166, 227, 161, 255};
    DrawCircleV(position, 15.0f, {30, 33, 48, 240});
    DrawCircleLines((int)position.x, (int)position.y, 15.0f, green);
    DrawCircleV(position, 4.0f, green);
    DrawText("LISTENER", (int)position.x - 24, (int)position.y + 19, 10, {169, 177, 214, 220});
}
