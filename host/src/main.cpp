#include <raylib.h>
#include "AcousticSource.hpp"
#include "WavefrontPool.hpp"
#include "Observer.hpp"
#include "AudioSynthesizer.hpp"
#include "JoystickController.hpp"
#include "KeyboardController.hpp"

#include <cmath>
#include <cstdio>
#include <algorithm>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

// Helper to draw the HUD panels (Telemetry, Mach Gauge, and Joystick Gimbal)
static void drawHUD(int screenW, int screenH,
                    const AcousticSource& source,
                    const Observer& observer,
                    const WavefrontPool& wavePool,
                    const IInputController* controller,
                    const AudioSynthesizer& audio,
                    bool paused,
                    float simTime)
{
    float mach = source.getMachNumber();
    float speed = source.getSpeed();
    float c = source.getSpeedOfSound();
    float f0 = observer.getBaseFrequency();
    float fObs = observer.computeObservedFrequency(source.getPosition(), source.getVelocity(), c);

    // 1. Top-Center Mach Speedometer Bar
    int gx = (screenW - 360) / 2, gy = 14, gw = 360, gh = 46;
    DrawRectangleRounded({(float)gx, (float)gy, (float)gw, (float)gh}, 0.18f, 4, {30, 33, 48, 235});
    DrawRectangleRoundedLinesEx({(float)gx, (float)gy, (float)gw, (float)gh}, 0.18f, 4, 1.2f, {49, 54, 76, 255});

    int barX = gx + 12, barY = gy + 24, barW = gw - 24;
    DrawRectangleRounded({(float)barX, (float)barY, (float)barW, 10.0f}, 0.5f, 4, {20, 22, 32, 255});

    int fillW = (int)(std::min(mach / 2.2f, 1.0f) * barW);
    Color machColor = (mach < 0.95f) ? (Color){166, 227, 161, 255}
                    : (mach <= 1.05f) ? (Color){249, 226, 175, 255}
                    : (Color){243, 139, 168, 255};
    if (fillW > 2) {
        DrawRectangleRounded({(float)barX, (float)barY, (float)fillW, 10.0f}, 0.5f, 4, machColor);
    }

    // Mark Mach 1.0 Sound Barrier tick on the bar
    int m1X = barX + (int)((1.0f / 2.2f) * barW);
    DrawLine(m1X, barY - 2, m1X, barY + 12, WHITE);

    DrawText(TextFormat("MACH %.2f%s", mach, paused ? " [PAUSED]" : ""), gx + 14, gy + 7, 12, {205, 214, 244, 255});
    std::string regime = source.getRegimeName();
    DrawText(regime.c_str(), gx + gw - MeasureText(regime.c_str(), 11) - 14, gy + 8, 11, {130, 142, 170, 255});

    // 2. Top-Left Physics Telemetry Card + Live Doppler Wave Scope
    DrawRectangleRounded({18, 18, 275, 210}, 0.08f, 4, {30, 33, 48, 235});
    DrawRectangleRoundedLinesEx({18, 18, 275, 210}, 0.08f, 4, 1.2f, {49, 54, 76, 255});
    DrawText("ACOUSTIC TELEMETRY", 32, 28, 10, {130, 142, 170, 255});

    DrawText(TextFormat("Flight Speed (v):     %.1f px/s", speed), 32, 48, 11, {205, 214, 244, 255});
    DrawText(TextFormat("Speed of Sound (c):   %.0f px/s ([ / ])", c), 32, 68, 11, {169, 177, 214, 255});
    if (mach >= 1.0f) {
        DrawText(TextFormat("Mach Cone Angle (mu): %.1f deg", source.getMachAngleDegrees()), 32, 88, 11, {243, 139, 168, 255});
    } else {
        DrawText("Mach Cone Angle (mu): N/A (Subsonic)", 32, 88, 11, {108, 112, 134, 255});
    }
    DrawText(TextFormat("Base Pitch (f0):      %.0f Hz", f0), 32, 108, 11, {169, 177, 214, 255});

    Color pitchColor = (fObs > f0 + 1.0f) ? (Color){166, 227, 161, 255}
                     : (fObs < f0 - 1.0f) ? (Color){243, 139, 168, 255}
                     : (Color){205, 214, 244, 255};
    DrawText(TextFormat("Observed Pitch (f'):  %.1f Hz", fObs), 32, 128, 11, pitchColor);

    // Lightweight oscilloscope (steps by 4px so it uses very few draw calls)
    int sx = 32, sy = 152, sw = 244, sh = 26;
    DrawRectangleRounded({(float)sx, (float)sy, (float)sw, (float)sh}, 0.25f, 4, {20, 22, 32, 255});
    float ratio = std::clamp(fObs / f0, 0.35f, 3.0f);
    float midY = sy + sh / 2.0f;
    float prevY = midY + std::sin(-simTime * 10.0f) * 9.0f;
    for (int i = 4; i <= sw; i += 4) {
        float curY = midY + std::sin(((float)i / sw) * 4.0f * PI * ratio - simTime * 10.0f) * 9.0f;
        DrawLine(sx + i - 4, (int)prevY, sx + i, (int)curY, pitchColor);
        prevY = curY;
    }

    // 3. Top-Right Hardware Status & Stick Gimbal
    int rx = screenW - 298;
    DrawRectangleRounded({(float)rx, 18, 280, 195}, 0.08f, 4, {30, 33, 48, 235});
    DrawRectangleRoundedLinesEx({(float)rx, 18, 280, 195}, 0.08f, 4, 1.2f, {49, 54, 76, 255});
    DrawText("HARDWARE & INPUT", rx + 14, 28, 10, {130, 142, 170, 255});

    bool hw = controller->isHardwareConnected();
    DrawCircle(rx + 20, 52, 4.0f, hw ? (Color){166, 227, 161, 255} : (Color){249, 226, 175, 255});
    DrawText(controller->getDeviceName().c_str(), rx + 32, 46, 11, {205, 214, 244, 255});
    DrawText(TextFormat("Active Waves: %d / %d", wavePool.getActiveCount(), WavefrontPool::MAX_WAVES),
             rx + 14, 68, 11, {169, 177, 214, 255});
    DrawText(audio.isMuted() ? "Audio: MUTED (Press M)" : "Audio: 44.1 kHz Active (Press M)",
             rx + 14, 88, 11, audio.isMuted() ? (Color){243, 139, 168, 255} : (Color){137, 180, 250, 255});

    // Mini Stick Deflection Circle
    Vector2 def = controller->getDeflection();
    int cx = rx + 52, cy = 155;
    DrawCircle(cx, cy, 32.0f, {20, 22, 32, 255});
    DrawCircleLines(cx, cy, 32.0f, {59, 66, 97, 240});
    int kx = cx + (int)(def.x * 24.0f), ky = cy + (int)(def.y * 24.0f);
    DrawLine(cx, cy, kx, ky, {166, 227, 161, 200});
    DrawCircle(kx, ky, 7.0f, {166, 227, 161, 255});

    int tiltPct = (int)(std::min(1.0f, std::sqrt(def.x * def.x + def.y * def.y)) * 100.0f);
    DrawText(TextFormat("Stick Tilt: %d%%", tiltPct), rx + 102, 132, 11, {205, 214, 244, 255});
    DrawText(TextFormat("Axes: X:%+.2f  Y:%+.2f", def.x, def.y), rx + 102, 152, 11, {169, 177, 214, 255});
    DrawText("SW Pin D2: Active LOW", rx + 102, 172, 10, {108, 112, 134, 255});

    // 4. Bottom Controls Bar
    int fy = screenH - 30;
    DrawRectangle(0, fy, screenW, 30, {20, 22, 32, 245});
    const char* controls = "Stick/WASD: Steer | Shift: Supersonic | SW/Space/R: Reset | P: Pause | [/]: Sound Speed | Tab: Switch Input | M: Mute";
    DrawText(controls, (screenW - MeasureText(controls, 11)) / 2, fy + 9, 11, {140, 155, 185, 255});
}

int main(int argc, char* argv[]) {
    const int screenWidth = 1200;
    const int screenHeight = 800;

    // 1. Initialize Raylib Window (60 FPS to match Arduino's delay(1000/60))
    InitWindow(screenWidth, screenHeight, "Embedded Doppler Visualizer - IIIT Bhubaneswar");
    SetTargetFPS(60);

    // 2. Initialize Simulation Objects (OOP Composition)
    AcousticSource source({screenWidth / 2.0f, screenHeight / 2.0f}, 180.0f);
    WavefrontPool wavePool;
    Observer observer({screenWidth * 0.75f, screenHeight * 0.45f}, 440.0f);
    AudioSynthesizer audio;
    audio.init();

    // 3. Initialize Input Controllers (OOP Polymorphism via IInputController*)
    std::string portArg = (argc > 1) ? argv[1] : "";
    JoystickController joystick(portArg);
    KeyboardController keyboard(170.0f, 380.0f);

    // Automatically use Arduino Joystick if plugged in, otherwise Keyboard
    IInputController* controller = joystick.isHardwareConnected()
        ? static_cast<IInputController*>(&joystick)
        : static_cast<IInputController*>(&keyboard);

    bool wasInsideCone = false;
    bool paused = false;
    float simTime = 0.0f;

    // 4. Main 60 FPS Simulation Loop
    while (!WindowShouldClose()) {
        float dt = std::min(GetFrameTime(), 0.05f);

        // Keyboard shortcuts for live classroom demo
        if (IsKeyPressed(KEY_P)) paused = !paused;
        if (IsKeyPressed(KEY_M)) audio.toggleMute();
        if (IsKeyDown(KEY_LEFT_BRACKET))  source.setSpeedOfSound(source.getSpeedOfSound() - 60.0f * dt);
        if (IsKeyDown(KEY_RIGHT_BRACKET)) source.setSpeedOfSound(source.getSpeedOfSound() + 60.0f * dt);

        // Press TAB to toggle between Arduino Joystick and Keyboard
        if (IsKeyPressed(KEY_TAB)) {
            if (controller == &keyboard && joystick.isHardwareConnected()) {
                controller = &joystick;
            } else {
                controller = &keyboard;
            }
        }
        // Press C to re-scan USB ports if Arduino was plugged in after launch
        if (IsKeyPressed(KEY_C)) {
            if (joystick.connect()) controller = &joystick;
        }

        // Allow dragging the green Listener station with mouse anytime
        observer.update();

        // --- Physics & Acoustics Update ---
        if (!paused) {
            simTime += dt;

            controller->update(dt);
            if (controller->isResetTriggered()) {
                source.reset({screenWidth / 2.0f, screenHeight / 2.0f});
                wavePool.clear();
                wasInsideCone = false;
            }

            // Update source position and emit expanding circular waves
            source.update(controller->getVelocity(), dt, screenWidth, screenHeight);
            if (source.shouldEmit(dt)) {
                wavePool.emitWave(source.getPosition());
            }
            wavePool.update(source.getSpeedOfSound(), dt);

            // Update Doppler pitch sent to the audio synthesizer
            float fObs = observer.computeObservedFrequency(
                source.getPosition(), source.getVelocity(), source.getSpeedOfSound()
            );
            audio.setTargetFrequency(fObs);

            // Check if the supersonic Mach cone just swept across the Listener (Sonic Boom!)
            float mach = source.getMachNumber();
            bool insideCone = false;
            if (mach >= 1.0f && source.getSpeed() > 1.0f) {
                Vector2 sPos = source.getPosition();
                Vector2 sVel = source.getVelocity();
                Vector2 oPos = observer.getPosition();
                float dx = oPos.x - sPos.x;
                float dy = oPos.y - sPos.y;

                // Distance behind the jet along its velocity vector
                float dParallel = (dx * sVel.x + dy * sVel.y) / source.getSpeed();
                if (dParallel < 0.0f) {
                    float distBehind = -dParallel;
                    float dPerp = std::sqrt(std::max(0.0f, (dx * dx + dy * dy) - dParallel * dParallel));
                    float coneRadius = distBehind / std::sqrt(mach * mach - 1.0f);
                    if (dPerp <= coneRadius) insideCone = true;
                }
            }

            if (!wasInsideCone && insideCone) {
                audio.triggerSonicBoom();
            }
            wasInsideCone = insideCone;
        } else {
            audio.setTargetFrequency(0.0f);
        }

        // --- Render Frame ---
        BeginDrawing();
        ClearBackground({24, 25, 37, 255});

        // Background coordinate grid
        for (int x = 0; x < screenWidth; x += 50) DrawLine(x, 0, x, screenHeight, {36, 40, 56, 140});
        for (int y = 0; y < screenHeight; y += 50) DrawLine(0, y, screenWidth, y, {36, 40, 56, 140});

        // Draw waves (blue when subsonic, orange when supersonic)
        Color waveColor = (source.getMachNumber() >= 1.0f)
            ? (Color){245, 169, 127, 255}
            : (Color){137, 180, 250, 255};
        wavePool.render(waveColor);

        // Draw Listener, Jet Source, and HUD
        observer.render(source.getPosition());
        source.render();
        drawHUD(screenWidth, screenHeight, source, observer, wavePool, controller, audio, paused, simTime);

        EndDrawing();
    }

    // Cleanly stop serial thread and audio stream BEFORE closing window
    joystick.disconnect();
    audio.shutdown();
    CloseWindow();
    return 0;
}
