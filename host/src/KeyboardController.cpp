#include "KeyboardController.hpp"
#include <cmath>
#include <algorithm>

KeyboardController::KeyboardController(float normalSpeed, float boostSpeed)
    : velocity{0.0f, 0.0f}
    , normalMaxSpeed(normalSpeed)
    , boostMaxSpeed(boostSpeed)
    , resetPressed(false)
{
}

void KeyboardController::update(float dt) {
    resetPressed = IsKeyPressed(KEY_R) || IsKeyPressed(KEY_SPACE);

    // 1. Read WASD or Arrow keys
    Vector2 dir = {0.0f, 0.0f};
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) dir.x += 1.0f;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) dir.x -= 1.0f;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) dir.y += 1.0f;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) dir.y -= 1.0f;

    // Normalize diagonal movement
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.0f) {
        dir.x /= len;
        dir.y /= len;
    }

    // 2. Hold Shift to go Supersonic (boost speed > speed of sound)
    bool boosting = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    float maxSpeed = boosting ? boostMaxSpeed : normalMaxSpeed;

    // 3. Smoothly accelerate or slow down
    if (len > 0.0f) {
        velocity.x += dir.x * 600.0f * dt;
        velocity.y += dir.y * 600.0f * dt;

        float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (speed > maxSpeed) {
            velocity.x = (velocity.x / speed) * maxSpeed;
            velocity.y = (velocity.y / speed) * maxSpeed;
        }
    } else {
        // Friction when keys are released
        velocity.x -= velocity.x * 7.0f * dt;
        velocity.y -= velocity.y * 7.0f * dt;
        if (std::abs(velocity.x) < 0.5f) velocity.x = 0.0f;
        if (std::abs(velocity.y) < 0.5f) velocity.y = 0.0f;
    }
}

Vector2 KeyboardController::getDeflection() const {
    return {
        std::clamp(velocity.x / normalMaxSpeed, -1.0f, 1.0f),
        std::clamp(velocity.y / normalMaxSpeed, -1.0f, 1.0f)
    };
}
