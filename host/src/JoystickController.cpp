#include "JoystickController.hpp"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <glob.h>

// Helper to auto-find Arduino USB port on macOS / Linux
static std::string findArduinoPort() {
    const char* patterns[] = {
        "/dev/cu.usbmodem*",
        "/dev/cu.usbserial*",
        "/dev/cu.wchusbserial*",
        "/dev/ttyACM*",
        "/dev/ttyUSB*"
    };
    glob_t g;
    for (const char* pat : patterns) {
        if (glob(pat, 0, nullptr, &g) == 0 && g.gl_pathc > 0) {
            std::string found = g.gl_pathv[0];
            globfree(&g);
            return found;
        }
        globfree(&g);
    }
    return "";
}

JoystickController::JoystickController(const std::string& preferredPort)
    : serialFd(-1)
    , running(false)
    , connected(false)
    , normX(0.0f)
    , normY(0.0f)
    , buttonClicked(false)
    , currentVelocity{0.0f, 0.0f}
    , resetTriggered(false)
    , maxSpeed(380.0f)
{
    connect(preferredPort);
}

JoystickController::~JoystickController() {
    disconnect();
}

bool JoystickController::connect(const std::string& port) {
    disconnect();

    portName = port.empty() ? findArduinoPort() : port;
    if (portName.empty()) return false;

    // Open USB serial port in non-blocking mode so it never freezes the app
    serialFd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serialFd < 0) return false;

    // Configure 115200 baud, 8-N-1 to match Serial.begin(115200) in Arduino firmware
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(serialFd, &tty) != 0) {
        close(serialFd);
        serialFd = -1;
        return false;
    }

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);
    tty.c_cflag |= (CLOCAL | CREAD | CS8);
    tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR | IGNCR);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    tcflush(serialFd, TCIFLUSH);
    if (tcsetattr(serialFd, TCSANOW, &tty) != 0) {
        close(serialFd);
        serialFd = -1;
        return false;
    }

    running = true;
    connected = true;
    workerThread = std::thread(&JoystickController::readSerialLoop, this);
    std::cout << "[Joystick] Connected on " << portName << "\n";
    return true;
}

void JoystickController::disconnect() {
    running = false;
    if (workerThread.joinable()) workerThread.join();
    if (serialFd >= 0) {
        close(serialFd);
        serialFd = -1;
    }
    connected = false;
    normX = 0.0f;
    normY = 0.0f;
}

void JoystickController::readSerialLoop() {
    std::string line;
    char buf[128];
    int lastSw = 1; // Switch uses INPUT_PULLUP (1 = unpressed, 0 = pressed)

    while (running.load()) {
        // Wait up to 30ms for incoming serial data without spinning CPU or blocking forever
        struct pollfd pfd = { serialFd, POLLIN, 0 };
        int ret = poll(&pfd, 1, 30);

        if (ret > 0 && (pfd.revents & POLLIN)) {
            ssize_t n = read(serialFd, buf, sizeof(buf) - 1);
            if (n > 0) {
                for (ssize_t i = 0; i < n; i++) {
                    char c = buf[i];
                    if (c == '\n' || c == '\r') {
                        int rawX = 512, rawY = 512, sw = 1;
                        // Parse CSV sent by Arduino: xValue,yValue,switchValue
                        if (!line.empty() && sscanf(line.c_str(), "%d,%d,%d", &rawX, &rawY, &sw) >= 2) {
                            // Deadzone of 35 around center (512) to ignore potentiometer jitter
                            float dx = (float)(rawX - 512);
                            float dy = (float)(rawY - 512);
                            if (std::abs(dx) < 35.0f) dx = 0.0f;
                            if (std::abs(dy) < 35.0f) dy = 0.0f;

                            // Convert from [-512, 512] to [-1.0, 1.0]
                            normX.store(std::clamp(dx / 512.0f, -1.0f, 1.0f));
                            normY.store(std::clamp(dy / 512.0f, -1.0f, 1.0f));

                            if (lastSw == 1 && sw == 0) buttonClicked.store(true);
                            lastSw = sw;
                        }
                        line.clear();
                    } else {
                        line += c;
                        if (line.size() > 64) line.clear();
                    }
                }
            } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                connected.store(false);
                break;
            }
        }
    }
}

void JoystickController::update(float dt) {
    resetTriggered = buttonClicked.exchange(false);

    // Smoothly interpolate velocity toward joystick target
    Vector2 target = { normX.load() * maxSpeed, normY.load() * maxSpeed };
    currentVelocity.x += (target.x - currentVelocity.x) * 12.0f * dt;
    currentVelocity.y += (target.y - currentVelocity.y) * 12.0f * dt;

    if (std::abs(currentVelocity.x) < 0.2f) currentVelocity.x = 0.0f;
    if (std::abs(currentVelocity.y) < 0.2f) currentVelocity.y = 0.0f;
}

std::string JoystickController::getDeviceName() const {
    return connected.load() ? ("KY-023 Joystick (" + portName + ")") : "Joystick (Disconnected)";
}
