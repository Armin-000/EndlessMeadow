#include "../include/map.hpp"
#include "../include/terrain.hpp"

#include "raymath.h"

void DrawWorldMapOverlay(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 playerPos,
    bool visible
) {
    if (!visible) return;

    const float mapSize = 300.0f;
    const float padding = 26.0f;

    const float mapX = GetScreenWidth() - mapSize - padding;
    const float mapY = padding;

    const float centerX = mapX + mapSize * 0.5f;
    const float centerY = mapY + mapSize * 0.5f;

    DrawRectangleRounded(
        Rectangle{mapX, mapY, mapSize, mapSize},
        0.14f,
        20,
        Fade(Color{5, 12, 10, 255}, 0.92f)
    );

    const int samples = 72;
    const float worldViewRadius = 260.0f;
    const float cellSize = mapSize / samples;

    for (int y = 0; y < samples; y++) {
        for (int x = 0; x < samples; x++) {
            float nx = ((float)x / samples - 0.5f) * 2.0f;
            float nz = ((float)y / samples - 0.5f) * 2.0f;

            float worldX = playerPos.x + nx * worldViewRadius;
            float worldZ = playerPos.z + nz * worldViewRadius;

            float h = GetTerrainHeight(worldX, worldZ);

            Color c;

            if (h <= WATER_LEVEL + 0.15f) {
                c = Color{35, 95, 130, 230};      // water
            } else if (h < 5.0f) {
                c = Color{45, 120, 60, 230};      // meadow
            } else if (h < 14.0f) {
                c = Color{65, 135, 65, 230};      // hills
            } else if (h < 25.0f) {
                c = Color{110, 100, 78, 230};     // dirt/rock
            } else if (h < 42.0f) {
                c = Color{130, 130, 125, 230};    // mountain
            } else {
                c = Color{210, 215, 215, 235};    // snow/high peaks
            }

            DrawRectangle(
                (int)(mapX + x * cellSize),
                (int)(mapY + y * cellSize),
                (int)(cellSize + 1),
                (int)(cellSize + 1),
                c
            );
        }
    }

    DrawRectangleRoundedLines(
        Rectangle{mapX, mapY, mapSize, mapSize},
        0.14f,
        20,
        Color{90, 200, 120, 230}
    );

    DrawRectangle(
        (int)mapX,
        (int)mapY,
        (int)mapSize,
        34,
        Fade(BLACK, 0.35f)
    );

    DrawText(
        "LIVE TERRAIN MAP",
        mapX + 16,
        mapY + 10,
        18,
        Color{230, 255, 230, 255}
    );

    DrawCircle(
        (int)centerX,
        (int)centerY,
        6.0f,
        Color{255, 225, 80, 255}
    );

    DrawCircleLines(
        (int)centerX,
        (int)centerY,
        10.0f,
        WHITE
    );

    DrawText(
        "M - close",
        mapX + 16,
        mapY + mapSize - 26,
        15,
        Fade(RAYWHITE, 0.85f)
    );
}