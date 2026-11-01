#pragma once
#include <raylib.h>
#include <string>

// Base interface for Polymorphism: main() can use either Joystick or Keyboard through this pointer
class IInputController {
public:
    virtual ~IInputController() = default;

    virtual void update(float dt) = 0;
    virtual Vector2 getVelocity() const = 0;
    virtual Vector2 getDeflection() const = 0; // Normalized stick position [-1, 1]
    virtual bool isResetTriggered() const = 0;
    virtual std::string getDeviceName() const = 0;
    virtual bool isHardwareConnected() const = 0;
};
