#include <raylib.h>

typedef struct WaveFront
{
    Vector2 origin;
    
};

int main() {
    // 1. Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Embedded Doppler Visualizer");
    SetTargetFPS(60); // We want 60 FPS to match your Arduino delay

    // Source properties
    Vector2 sourcePos = { (float)screenWidth / 2, (float)screenHeight / 2 };
    float speed = 5.0f;

    // 2. Main game loop
    while (!WindowShouldClose()) { // Detect window close button or ESC key
        
        // --- Update ---
        // Basic keyboard controls (fallback before we add serial)
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) sourcePos.x += speed;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) sourcePos.x -= speed;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) sourcePos.y -= speed;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) sourcePos.y += speed;

        // Keep the source inside the screen
        if (sourcePos.x < 0) sourcePos.x = 0;
        if (sourcePos.x > screenWidth) sourcePos.x = screenWidth;
        if (sourcePos.y < 0) sourcePos.y = 0;
        if (sourcePos.y > screenHeight) sourcePos.y = screenHeight;

        // --- Draw ---
        BeginDrawing();
        ClearBackground(RAYWHITE); // Clear the screen with a white color

        // Draw the sound source as a red circle
        DrawCircleV(sourcePos, 10, RED);

        DrawText("Use Arrow Keys to move the source (for now!)", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    // 3. De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}
