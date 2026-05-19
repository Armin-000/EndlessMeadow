#include "preload.hpp"

#include "raylib.h"
#include "raymath.h"

#include <cmath>

void DrawLoadingScreen(Texture2D preloadTexture, int loadedInitialChunks, int totalInitialChunks)
{
    float progress = (float)loadedInitialChunks / (float)totalInitialChunks;
    progress = Clamp(progress, 0.0f, 1.0f);

    float time = GetTime();

    BeginDrawing();

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    ClearBackground(Color{8, 14, 12, 255});

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
        Fade(WHITE, 0.92f)
    );

    DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.34f));
    DrawRectangleGradientV(
        0,
        0,
        screenW,
        screenH,
        Fade(BLACK, 0.12f),
        Fade(BLACK, 0.72f)
    );

    for (int i = 0; i < 42; i++)
    {
        float x = fmodf(i * 97.0f + time * 18.0f, (float)screenW);
        float y = fmodf(i * 53.0f + sinf(time * 0.8f + i) * 18.0f, (float)screenH);
        float radius = 1.3f + sinf(time * 1.7f + i) * 0.7f;

        DrawCircleV(
            Vector2{x, y},
            radius,
            Fade(Color{170, 255, 185, 255}, 0.18f)
        );
    }

    const char* title = "ENDLESS MEADOW";

    float titlePulse = 0.75f + sinf(time * 2.2f) * 0.10f;

    DrawText(
        title,
        screenW / 2 - MeasureText(title, 54) / 2,
        screenH - 205,
        54,
        Fade(Color{130, 255, 165, 255}, 0.18f)
    );

    DrawText(
        title,
        screenW / 2 - MeasureText(title, 50) / 2,
        screenH - 202,
        50,
        Fade(WHITE, titlePulse)
    );

    const char* loadingTexts[] = {
        "Growing procedural grass...",
        "Streaming meadow chunks...",
        "Painting flowers...",
        "Preparing soft clouds...",
        "Awakening the alien explorer..."
    };

    int textIndex = ((int)(time * 1.4f)) % 5;
    const char* subtitle = loadingTexts[textIndex];

    DrawText(
        subtitle,
        screenW / 2 - MeasureText(subtitle, 22) / 2,
        screenH - 132,
        22,
        Fade(RAYWHITE, 0.86f)
    );

    int panelW = screenW * 0.50f;
    int panelH = 64;
    int panelX = screenW / 2 - panelW / 2;
    int panelY = screenH - 94;

    DrawRectangleRounded(
        Rectangle{(float)panelX, (float)panelY, (float)panelW, (float)panelH},
        0.28f,
        24,
        Fade(Color{8, 18, 13, 255}, 0.72f)
    );

    DrawRectangleRoundedLines(
        Rectangle{(float)panelX, (float)panelY, (float)panelW, (float)panelH},
        0.28f,
        24,
        Fade(Color{160, 255, 180, 255}, 0.30f)
    );

    int barW = panelW - 54;
    int barH = 14;
    int barX = panelX + 27;
    int barY = panelY + 18;

    DrawRectangleRounded(
        Rectangle{(float)barX, (float)barY, (float)barW, (float)barH},
        0.50f,
        18,
        Fade(BLACK, 0.60f)
    );

    DrawRectangleRounded(
        Rectangle{(float)barX, (float)barY, barW * progress, (float)barH},
        0.50f,
        18,
        Color{92, 245, 135, 255}
    );

    float shineX = barX + fmodf(time * 170.0f, (float)barW);

    DrawRectangleGradientH(
        (int)shineX - 42,
        barY,
        84,
        barH,
        Fade(WHITE, 0.0f),
        Fade(WHITE, 0.22f)
    );

    const char* percentText = TextFormat("%i%%", (int)(progress * 100.0f));

    DrawText(
        percentText,
        screenW / 2 - MeasureText(percentText, 21) / 2,
        panelY + 38,
        21,
        Fade(WHITE, 0.92f)
    );

    EndDrawing();
}