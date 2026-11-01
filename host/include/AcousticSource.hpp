#pragma once
#include <raylib.h>
#include <string>
#include <algorithm>

// Represents the moving sound source (jet) and calculates Mach physics
class AcousticSource {
public:
    AcousticSource(Vector2 startPos, float soundSpeed = 180.0f);

    void update(Vector2 inputVel, float dt, int screenW, int screenH);
    bool shouldEmit(float dt);
    void render() const;
    void reset(Vector2 centerPos);

    Vector2 getPosition() const { return position; }
    Vector2 getVelocity() const { return velocity; }
    float getSpeed() const;
    float getSpeedOfSound() const { return speedOfSound; }
    void setSpeedOfSound(float c) { speedOfSound = std::clamp(c, 80.0f, 360.0f); }

    float getMachNumber() const;
    float getMachAngle() const;        // sin(mu) = 1 / M (in radians)
    float getMachAngleDegrees() const; // in degrees
    std::string getRegimeName() const;

private:
    Vector2 position;
    Vector2 velocity;
    float heading;        // Angle the jet is facing (radians)
    float speedOfSound;   // Speed at which waves expand (px/sec)
    float emitTimer;      // Timer to emit a new wave every 0.075s
};
