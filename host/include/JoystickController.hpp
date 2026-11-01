#pragma once
#include "IInputController.hpp"
#include <thread>
#include <atomic>
#include <string>

// Reads CSV data ("x,y,sw") from the Arduino over USB Serial at 115200 baud
class JoystickController : public IInputController {
public:
    JoystickController(const std::string& preferredPort = "");
    ~JoystickController() override;

    bool connect(const std::string& port = "");
    void disconnect();

    void update(float dt) override;
    Vector2 getVelocity() const override { return currentVelocity; }
    Vector2 getDeflection() const override { return { normX.load(), normY.load() }; }
    bool isResetTriggered() const override { return resetTriggered; }
    std::string getDeviceName() const override;
    bool isHardwareConnected() const override { return connected.load(); }

private:
    void readSerialLoop(); // Runs on a background thread so graphics never freeze

    int serialFd;
    std::string portName;
    std::thread workerThread;
    std::atomic<bool> running;
    std::atomic<bool> connected;

    // Latest joystick values shared between serial thread and main thread
    std::atomic<float> normX;
    std::atomic<float> normY;
    std::atomic<bool> buttonClicked;

    Vector2 currentVelocity;
    bool resetTriggered;
    float maxSpeed;
};
