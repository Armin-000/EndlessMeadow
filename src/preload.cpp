#include "preload.hpp"

#include <cmath>

void DrawLoadingScreen(Texture2D preloadTexture, int loadedInitialChunks, int totalInitialChunks)
{
    float progress = (float)loadedInitialChunks / (float)totalInitialChunks;

    BeginDrawing();
    ClearBackground(BLACK);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float scale = fmaxf(
        (float)screenW / preloadTexture.width,
        (float)screenH / preloadTexture.height
    );

    float drawW = preloadTexture.width * scale;
    float drawH = preloadTexture.height * scale;

    DrawTextureEx(
        preloadTexture,
        Vector2{(screenW - drawW) / 2.0f, (screenH - drawH) / 2.0f},
        0.0f,
        scale,
        WHITE
    );

    DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.18f));

    const char* title = "ENDLESS MEADOW";
    const char* subtitle = "Generating procedural world...";

    DrawText(title, screenW / 2 - MeasureText(title, 46) / 2, screenH - 170, 46, WHITE);
    DrawText(subtitle, screenW / 2 - MeasureText(subtitle, 22) / 2, screenH - 112, 22, RAYWHITE);

    int barW = screenW * 0.42f;
    int barH = 18;
    int barX = screenW / 2 - barW / 2;
    int barY = screenH - 70;

    DrawRectangleRounded(
        Rectangle{(float)barX, (float)barY, (float)barW, (float)barH},
        0.45f,
        16,
        Fade(BLACK, 0.55f)
    );

    DrawRectangleRounded(
        Rectangle{(float)barX, (float)barY, barW * progress, (float)barH},
        0.45f,
        16,
        Color{90, 255, 130, 255}
    );

    DrawText(
        TextFormat("%i%%", (int)(progress * 100.0f)),
        screenW / 2 - 22,
        screenH - 42,
        24,
        WHITE
    );

    EndDrawing();
}