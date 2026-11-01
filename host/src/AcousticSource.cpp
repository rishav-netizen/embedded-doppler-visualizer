#include "AcousticSource.hpp"
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

AcousticSource::AcousticSource(Vector2 startPos, float soundSpeed)
    : position(startPos)
    , velocity{0.0f, 0.0f}
    , heading(0.0f)
    , speedOfSound(soundSpeed)
    , emitTimer(0.0f)
{
}

void AcousticSource::update(Vector2 inputVel, float dt, int screenW, int screenH) {
    velocity = inputVel;

    // Move position using velocity: x = x + v * dt
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    // Keep inside screen boundaries
    position.x = std::clamp(position.x, 15.0f, (float)screenW - 15.0f);
    position.y = std::clamp(position.y, 15.0f, (float)screenH - 15.0f);

    // Update heading angle if moving
    if (getSpeed() > 1.0f) {
        heading = std::atan2(velocity.y, velocity.x);
    }
}

bool AcousticSource::shouldEmit(float dt) {
    emitTimer += dt;
    if (emitTimer >= 0.075f) { // Emit ~13 waves per second
        emitTimer -= 0.075f;
        return true;
    }
    return false;
}

float AcousticSource::getSpeed() const {
    return std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
}

float AcousticSource::getMachNumber() const {
    return getSpeed() / speedOfSound;
}

float AcousticSource::getMachAngle() const {
    float mach = getMachNumber();
    if (mach < 1.0f) return 0.0f; // Mach cone only exists when supersonic (M >= 1)
    return std::asin(std::min(1.0f, 1.0f / mach));
}

float AcousticSource::getMachAngleDegrees() const {
    return getMachAngle() * (180.0f / PI);
}

std::string AcousticSource::getRegimeName() const {
    float mach = getMachNumber();
    if (mach < 0.02f) return "Stationary (M = 0)";
    if (mach < 0.96f) return "Subsonic (M < 1.0)";
    if (mach <= 1.04f) return "Sound Barrier (M = 1.0)";
    return "Supersonic (M > 1.0)";
}

void AcousticSource::reset(Vector2 centerPos) {
    position = centerPos;
    velocity = {0.0f, 0.0f};
    emitTimer = 0.0f;
}

void AcousticSource::render() const {
    float mach = getMachNumber();

    // 1. Draw V-shaped Supersonic Mach Cone when M >= 1.0
    if (mach >= 1.0f && getSpeed() > 1.0f) {
        float mu = getMachAngle();
        float len = 550.0f;

        // Cone lines trail behind the jet at +/- mu angle
        float leftAngle  = heading + PI - mu;
        float rightAngle = heading + PI + mu;

        Vector2 leftPt  = { position.x + std::cos(leftAngle) * len,  position.y + std::sin(leftAngle) * len };
        Vector2 rightPt = { position.x + std::cos(rightAngle) * len, position.y + std::sin(rightAngle) * len };

        DrawTriangle(position, leftPt, rightPt, {243, 139, 168, 22});
        DrawLineEx(position, leftPt, 2.0f, {243, 139, 168, 230});
        DrawLineEx(position, rightPt, 2.0f, {243, 139, 168, 230});
    }

    // 2. Draw Jet Triangle
    Vector2 nose      = { position.x + std::cos(heading) * 14.0f,        position.y + std::sin(heading) * 14.0f };
    Vector2 leftWing  = { position.x + std::cos(heading + 2.4f) * 9.0f,  position.y + std::sin(heading + 2.4f) * 9.0f };
    Vector2 rightWing = { position.x + std::cos(heading - 2.4f) * 9.0f,  position.y + std::sin(heading - 2.4f) * 9.0f };
    Vector2 tail      = { position.x - std::cos(heading) * 8.0f,         position.y - std::sin(heading) * 8.0f };

    Color jetColor = (mach >= 1.0f) ? (Color){243, 139, 168, 255} : (Color){205, 214, 244, 255};
    DrawTriangle(nose, leftWing, tail, jetColor);
    DrawTriangle(nose, tail, rightWing, jetColor);
    DrawTriangleLines(nose, leftWing, tail, WHITE);
    DrawTriangleLines(nose, tail, rightWing, WHITE);
}
