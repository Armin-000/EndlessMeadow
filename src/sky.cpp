#include "../include/sky.hpp"

#include <cmath>

void DrawClouds(float time, Vector3 playerPos) {
    for (int i = 0; i < 26; i++) {
        float x = playerPos.x + sinf(i * 91.7f) * 900.0f;
        float z = playerPos.z + cosf(i * 71.3f) * 900.0f;

        x += sinf(time * 0.012f + i * 2.0f) * 130.0f;
        z += cosf(time * 0.010f + i * 1.4f) * 60.0f;

        float y = 260.0f + sinf(i * 0.7f) * 45.0f;
        float s = 10.0f + (i % 6) * 3.0f;

        Color c = Fade(Color{230, 238, 238, 255}, 0.38f);

        Vector3 p = {x, y, z};

        DrawSphere({p.x, p.y, p.z}, s * 1.25f, c);
        DrawSphere({p.x + s * 1.15f, p.y - 2.0f, p.z}, s * 0.95f, c);
        DrawSphere({p.x - s * 1.20f, p.y - 1.0f, p.z}, s * 0.90f, c);
        DrawSphere({p.x + s * 0.35f, p.y + 4.0f, p.z + s * 0.55f}, s * 0.82f, c);
        DrawSphere({p.x - s * 0.45f, p.y + 3.0f, p.z - s * 0.45f}, s * 0.78f, c);

        DrawSphere(
            {p.x, p.y - 7.0f, p.z},
            s * 1.7f,
            Fade(Color{190, 205, 210, 255}, 0.16f)
        );
    }
}