#include "../include/atmosphere.hpp"

Color GetAtmosphereColor(float time)
{
    return Color{170, 205, 220, 255};
}

void DrawAtmosphereGradient()
{
    DrawRectangleGradientV(
        0,
        0,
        GetScreenWidth(),
        GetScreenHeight(),

        Color{90, 160, 255, 255},
        Color{255, 210, 140, 255}
    );
}