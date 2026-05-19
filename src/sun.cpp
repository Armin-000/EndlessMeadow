#include "../include/sun.hpp"

#include "rlgl.h"
#include <cmath>

void DrawSun(float time, Vector3 playerPos)
{
    float pulse = sinf(time * 1.2f) * 1.8f;

    Vector3 sunPos =
    {
        playerPos.x + 180.0f,
        180.0f,
        playerPos.z - 320.0f
    };

    BeginBlendMode(BLEND_ADDITIVE);
    rlDisableDepthMask();

    // Massive orange atmosphere
    DrawSphereEx(
        sunPos,
        82.0f + pulse,
        32,
        32,
        Fade(Color{255, 120, 20, 255}, 0.028f)
    );

    // Deep sunset glow
    DrawSphereEx(
        sunPos,
        62.0f + pulse * 0.6f,
        40,
        40,
        Fade(Color{255, 160, 40, 255}, 0.055f)
    );

    // Golden atmospheric layer
    DrawSphereEx(
        sunPos,
        46.0f,
        48,
        48,
        Fade(Color{255, 190, 70, 255}, 0.12f)
    );

    // Rich yellow body
    DrawSphereEx(
        sunPos,
        30.0f,
        64,
        64,
        Color{255, 205, 70, 255}
    );

    // Warm golden core
    DrawSphereEx(
        sunPos,
        20.0f,
        64,
        64,
        Color{255, 225, 110, 255}
    );

    // Tiny hot center, not pure white
    DrawSphereEx(
        sunPos,
        8.0f,
        64,
        64,
        Color{255, 245, 180, 255}
    );

    rlEnableDepthMask();
    EndBlendMode();
}

void UnloadSun()
{
}