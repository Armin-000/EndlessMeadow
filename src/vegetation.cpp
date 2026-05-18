#include "../include/vegetation.hpp"
#include "../include/terrain.hpp"

#include "raymath.h"

#include <cmath>

const char* grassVertexShader = R"(
#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;
uniform float time;
uniform float windStrength;

out vec4 fragColor;

void main()
{
    vec3 pos = vertexPosition;

    float influence = vertexColor.a;

    float waveA = sin(time * 3.2 + pos.x * 0.09 + pos.z * 0.07);
    float waveB = sin(time * 6.1 + pos.z * 0.13 + pos.x * 0.03);
    float wind = (waveA * 0.75 + waveB * 0.35) * windStrength * influence;

    pos.x += wind;
    pos.z += wind * 0.42;

    fragColor = vec4(vertexColor.rgb, 1.0);

    gl_Position = mvp * vec4(pos, 1.0);
}
)";

const char* grassFragmentShader = R"(
#version 330

in vec4 fragColor;
out vec4 finalColor;

void main()
{
    finalColor = fragColor;
}
)";

Color RandomGrassColor() {
    int green = GetRandomValue(130, 255);

    return Color{
        static_cast<unsigned char>(GetRandomValue(35, 80)),
        static_cast<unsigned char>(green),
        static_cast<unsigned char>(GetRandomValue(35, 90)),
        255
    };
}

Color RandomFlowerColor() {
    int type = GetRandomValue(0, 5);

    if (type == 0) return Color{255, 235, 90, 255};
    if (type == 1) return Color{255, 125, 185, 255};
    if (type == 2) return Color{180, 130, 255, 255};
    if (type == 3) return Color{255, 255, 255, 255};
    if (type == 4) return Color{255, 165, 75, 255};

    return Color{120, 190, 255, 255};
}

void WriteVertex(
    Mesh& mesh,
    int vertexIndex,
    Vector3 p,
    Vector3 n,
    Color c
) {
    mesh.vertices[vertexIndex * 3 + 0] = p.x;
    mesh.vertices[vertexIndex * 3 + 1] = p.y;
    mesh.vertices[vertexIndex * 3 + 2] = p.z;

    mesh.normals[vertexIndex * 3 + 0] = n.x;
    mesh.normals[vertexIndex * 3 + 1] = n.y;
    mesh.normals[vertexIndex * 3 + 2] = n.z;

    mesh.texcoords[vertexIndex * 2 + 0] = 0.0f;
    mesh.texcoords[vertexIndex * 2 + 1] = 0.0f;

    mesh.colors[vertexIndex * 4 + 0] = c.r;
    mesh.colors[vertexIndex * 4 + 1] = c.g;
    mesh.colors[vertexIndex * 4 + 2] = c.b;
    mesh.colors[vertexIndex * 4 + 3] = c.a;
}

Mesh GenerateGrassMesh(float startX, float startZ, float chunkWorldSize) {
    const int bladeCount = GRASS_PER_CHUNK;
    const int vertexCount = bladeCount * 4;
    const int triangleCount = bladeCount * 2;

    Mesh mesh = {0};
    mesh.vertexCount = vertexCount;
    mesh.triangleCount = triangleCount;

    mesh.vertices = (float*)MemAlloc(vertexCount * 3 * sizeof(float));
    mesh.normals = (float*)MemAlloc(vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)MemAlloc(vertexCount * 2 * sizeof(float));
    mesh.colors = (unsigned char*)MemAlloc(vertexCount * 4 * sizeof(unsigned char));
    mesh.indices = (unsigned short*)MemAlloc(triangleCount * 3 * sizeof(unsigned short));

    int v = 0;
    int idx = 0;

    int generated = 0;
    int attempts = 0;
    int maxAttempts = bladeCount * 8;

    while (generated < bladeCount && attempts < maxAttempts) {
        attempts++;

        float gxWorld = startX + GetRandomValue(0, 10000) / 10000.0f * chunkWorldSize;
        float gzWorld = startZ + GetRandomValue(0, 10000) / 10000.0f * chunkWorldSize;
        float gy = GetTerrainHeight(gxWorld, gzWorld);

        if (gy < WATER_LEVEL + 0.2f || gy > 18.0f) continue;

        float h = 0.12f + GetRandomValue(0, 1000) / 1000.0f * 0.38f;
        float w = 0.12f + GetRandomValue(0, 1000) / 1000.0f * 0.18f;

        gxWorld += (GetRandomValue(-1000, 1000) / 1000.0f) * 0.55f;
        gzWorld += (GetRandomValue(-1000, 1000) / 1000.0f) * 0.55f;

        gy = GetTerrainHeight(gxWorld, gzWorld);

        float localX = gxWorld - startX;
        float localZ = gzWorld - startZ;

        float angle = GetRandomValue(0, 10000) / 10000.0f * PI * 2.0f;
        float rx = cosf(angle) * w;
        float rz = sinf(angle) * w;

        Color c = RandomGrassColor();

        Vector3 p0 = {localX - rx, gy + 0.03f, localZ - rz};
        Vector3 p1 = {localX + rx, gy + 0.03f, localZ + rz};
        Vector3 p2 = {localX - rx * 0.35f, gy + h, localZ - rz * 0.35f};
        Vector3 p3 = {localX + rx * 0.35f, gy + h, localZ + rz * 0.35f};

        Color baseColor = Color{c.r, c.g, c.b, 0};
        Color tipColor = Color{c.r, c.g, c.b, 255};

        WriteVertex(mesh, v + 0, p0, {0, 1, 0}, baseColor);
        WriteVertex(mesh, v + 1, p1, {0, 1, 0}, baseColor);
        WriteVertex(mesh, v + 2, p2, {0, 1, 0}, tipColor);
        WriteVertex(mesh, v + 3, p3, {0, 1, 0}, tipColor);

        mesh.indices[idx++] = v + 0;
        mesh.indices[idx++] = v + 1;
        mesh.indices[idx++] = v + 2;

        mesh.indices[idx++] = v + 2;
        mesh.indices[idx++] = v + 1;
        mesh.indices[idx++] = v + 3;

        v += 4;
        generated++;
    }

    mesh.vertexCount = v;
    mesh.triangleCount = idx / 3;

    UploadMesh(&mesh, false);

    return mesh;
}

Mesh GenerateFlowerMesh(float startX, float startZ, float chunkWorldSize) {
    const int maxFlowers = FLOWERS_PER_CHUNK;
    const int verticesPerFlower = 8;
    const int trianglesPerFlower = 4;

    Mesh mesh = {0};
    mesh.vertexCount = maxFlowers * verticesPerFlower;
    mesh.triangleCount = maxFlowers * trianglesPerFlower;

    mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.colors = (unsigned char*)MemAlloc(mesh.vertexCount * 4 * sizeof(unsigned char));
    mesh.indices = (unsigned short*)MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short));

    int v = 0;
    int idx = 0;
    int generated = 0;
    int attempts = 0;
    int maxAttempts = maxFlowers * 5;

    while (generated < maxFlowers && attempts < maxAttempts) {
        attempts++;

        float fx = startX + GetRandomValue(0, 10000) / 10000.0f * chunkWorldSize;
        float fz = startZ + GetRandomValue(0, 10000) / 10000.0f * chunkWorldSize;
        float fy = GetTerrainHeight(fx, fz);

        float flowerZone = FractalNoise(fx + 2500, fz - 1800, 3, 0.035f, 0.5f);

        bool meadowHeight = fy > WATER_LEVEL + 0.45f && fy < 11.5f;
        bool flowerPatch = flowerZone > -0.10f;
        bool randomDensity = GetRandomValue(0, 100) > 32;

        if (!meadowHeight || !flowerPatch || !randomDensity) continue;

        float localX = fx - startX;
        float localZ = fz - startZ;

        float size = 0.55f + GetRandomValue(0, 1000) / 1000.0f * 0.75f;
        float stemHeight = 0.30f * size;
        float blossomHeight = 0.48f * size;
        float halfStem = 0.018f * size;
        float halfPetal = 0.13f * size;

        Color stemColor = Color{45, 150, 55, 255};
        Color flowerColor = RandomFlowerColor();

        Vector3 s0 = {localX - halfStem, fy + 0.04f, localZ};
        Vector3 s1 = {localX + halfStem, fy + 0.04f, localZ};
        Vector3 s2 = {localX - halfStem, fy + stemHeight, localZ};
        Vector3 s3 = {localX + halfStem, fy + stemHeight, localZ};

        Vector3 p0 = {localX - halfPetal, fy + blossomHeight, localZ};
        Vector3 p1 = {localX + halfPetal, fy + blossomHeight, localZ};
        Vector3 p2 = {localX - halfPetal * 0.55f, fy + blossomHeight + halfPetal, localZ};
        Vector3 p3 = {localX + halfPetal * 0.55f, fy + blossomHeight + halfPetal, localZ};

        WriteVertex(mesh, v + 0, s0, {0, 1, 0}, stemColor);
        WriteVertex(mesh, v + 1, s1, {0, 1, 0}, stemColor);
        WriteVertex(mesh, v + 2, s2, {0, 1, 0}, stemColor);
        WriteVertex(mesh, v + 3, s3, {0, 1, 0}, stemColor);

        WriteVertex(mesh, v + 4, p0, {0, 1, 0}, flowerColor);
        WriteVertex(mesh, v + 5, p1, {0, 1, 0}, flowerColor);
        WriteVertex(mesh, v + 6, p2, {0, 1, 0}, flowerColor);
        WriteVertex(mesh, v + 7, p3, {0, 1, 0}, flowerColor);

        mesh.indices[idx++] = v + 0;
        mesh.indices[idx++] = v + 1;
        mesh.indices[idx++] = v + 2;

        mesh.indices[idx++] = v + 2;
        mesh.indices[idx++] = v + 1;
        mesh.indices[idx++] = v + 3;

        mesh.indices[idx++] = v + 4;
        mesh.indices[idx++] = v + 5;
        mesh.indices[idx++] = v + 6;

        mesh.indices[idx++] = v + 6;
        mesh.indices[idx++] = v + 5;
        mesh.indices[idx++] = v + 7;

        v += verticesPerFlower;
        generated++;
    }

    mesh.vertexCount = v;
    mesh.triangleCount = idx / 3;

    UploadMesh(&mesh, false);

    return mesh;
}

void DrawNature(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 playerPos,
    float time
) {
    for (const auto& item : chunks) {
        const Chunk& c = item.second;

        for (const Tree& t : c.trees) {
            if (Vector3Distance(playerPos, t.position) > 120.0f) continue;

            float wind = sinf(time * 1.8f + t.sway) * 0.28f * t.size;

            if (t.shakeTimer > 0.0f) {
                wind += sinf(time * 55.0f) * t.shakeTimer * 2.5f;
            }

            DrawCylinder(
                t.position,
                0.18f * t.size,
                0.25f * t.size,
                2.4f * t.size,
                10,
                Color{100, 65, 35, 255}
            );

            Vector3 crown = {
                t.position.x + wind,
                t.position.y + 2.8f * t.size,
                t.position.z + wind * 0.35f
            };

            DrawSphere(crown, 1.25f * t.size, Color{38, 130, 52, 255});
            DrawSphere({crown.x + 0.7f * t.size, crown.y - 0.45f * t.size, crown.z}, 0.9f * t.size, Color{35, 115, 48, 255});
            DrawSphere({crown.x - 0.7f * t.size, crown.y - 0.35f * t.size, crown.z}, 0.9f * t.size, Color{45, 145, 58, 255});
        }

        for (const Rock& r : c.rocks) {
            if (Vector3Distance(playerPos, r.position) > 90.0f) continue;

            float shape = sinf(r.position.x * 0.12f + r.position.z * 0.08f) * 0.18f;

            DrawCube(
                r.position,
                r.size * (1.45f + shape),
                r.size * 0.72f,
                r.size * 1.12f,
                Color{98, 98, 94, 255}
            );

            DrawSphere(
                {r.position.x + 0.28f * r.size, r.position.y + 0.20f * r.size, r.position.z - 0.18f * r.size},
                r.size * 0.48f,
                Color{130, 130, 124, 255}
            );

            DrawSphere(
                {r.position.x - 0.35f * r.size, r.position.y + 0.10f * r.size, r.position.z + 0.22f * r.size},
                r.size * 0.36f,
                Color{82, 82, 78, 255}
            );
        }
    }
}

void DrawFlowerModels(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 playerPos
) {
    for (const auto& item : chunks) {
        const Chunk& c = item.second;

        if (!c.hasFlowerMesh) continue;

        float chunkWorldSize = CHUNK_SIZE * WORLD_SCALE;
        float chunkCenterX = c.key.x * chunkWorldSize + chunkWorldSize * 0.5f;
        float chunkCenterZ = c.key.z * chunkWorldSize + chunkWorldSize * 0.5f;

        float dist = Vector2Distance(
            {playerPos.x, playerPos.z},
            {chunkCenterX, chunkCenterZ}
        );

        if (dist < 120.0f) {
            DrawModel(c.flowerModel, {0, 0, 0}, 1.0f, WHITE);
        }
    }
}