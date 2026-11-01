#pragma once
#include <raylib.h>

// Represents a single expanding circular sound wave
struct Wavefront {
    Vector2 origin = {0.0f, 0.0f};
    float radius = 0.0f;
    float alpha = 0.0f;
    bool active = false;

    void update(float speedOfSound, float dt) {
        if (!active) return;

        // Radius grows at the speed of sound: r = r + c * dt
        radius += speedOfSound * dt;

        // Fade out as the wave expands (max radius = 650 px for fast rendering)
        alpha = 1.0f - (radius / 650.0f);
        if (alpha <= 0.02f) {
            alpha = 0.0f;
            active = false;
        }
    }

    void draw(Color color) const {
        if (!active || alpha <= 0.02f) return;
        color.a = (unsigned char)(alpha * 220.0f);
        DrawCircleLines((int)origin.x, (int)origin.y, radius, color);
    }
};
