#pragma once
#include "IInputController.hpp"

// Fallback controller using WASD / Arrow keys when Arduino isn't connected
class KeyboardController : public IInputController {
public:
    KeyboardController(float normalSpeed = 170.0f, float boostSpeed = 380.0f);

    void update(float dt) override;
    Vector2 getVelocity() const override { return velocity; }
    Vector2 getDeflection() const override;
    bool isResetTriggered() const override { return resetPressed; }
    std::string getDeviceName() const override { return "Keyboard (WASD + Shift Boost)"; }
    bool isHardwareConnected() const override { return false; }

private:
    Vector2 velocity;
    float normalMaxSpeed;
    float boostMaxSpeed;
    bool resetPressed;
};
