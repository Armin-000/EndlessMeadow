#pragma once

#include "raylib.h"
#include "raymath.h"

#include <cmath>
#include <map>
#include <vector>
#include <cstdlib>

const int CHUNK_SIZE = 28;
const int CHUNK_RESOLUTION = 32;
const int VIEW_DISTANCE = 4;

const float WORLD_SCALE = 3.0f;
const int GRASS_PER_CHUNK = 9000;
const int FLOWERS_PER_CHUNK = 220;

const float WATER_LEVEL = -2.0f;

struct ChunkKey {
    int x;
    int z;

    bool operator<(const ChunkKey& other) const {
        if (x != other.x) return x < other.x;
        return z < other.z;
    }
};

struct Tree {
    Vector3 position;
    float size;
    float sway;
};

struct Rock {
    Vector3 position;
    float size;
};

struct Flower {
    Vector3 position;
    float size;
    Color color;
    float sway;
};

struct Chunk {
    ChunkKey key;

    Mesh terrainMesh;
    Model terrainModel;

    Mesh grassMesh;
    Model grassModel;

    Mesh flowerMesh;
    Model flowerModel;
    bool hasFlowerMesh = false;

    std::vector<Tree> trees;
    std::vector<Rock> rocks;
};

static const char* grassVertexShader = R"(
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

static const char* grassFragmentShader = R"(
#version 330

in vec4 fragColor;
out vec4 finalColor;

void main()
{
    finalColor = fragColor;
}
)";

inline float HashNoise(int x, int z) {
    int n = x * 374761393 + z * 668265263;
    n = (n ^ (n >> 13)) * 1274126177;
    return 1.0f - ((n & 0x7fffffff) / 1073741824.0f);
}

inline float SmoothNoise(float x, float z) {
    int ix = (int)floorf(x);
    int iz = (int)floorf(z);

    float fx = x - ix;
    float fz = z - iz;

    float a = HashNoise(ix, iz);
    float b = HashNoise(ix + 1, iz);
    float c = HashNoise(ix, iz + 1);
    float d = HashNoise(ix + 1, iz + 1);

    float ux = fx * fx * (3.0f - 2.0f * fx);
    float uz = fz * fz * (3.0f - 2.0f * fz);

    return Lerp(Lerp(a, b, ux), Lerp(c, d, ux), uz);
}

inline float FractalNoise(float x, float z, int octaves, float frequency, float persistence) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++) {
        value += SmoothNoise(x * frequency, z * frequency) * amplitude;
        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return value / maxValue;
}

inline float RidgeNoise(float n) {
    n = 1.0f - fabsf(n);
    return n * n;
}

inline float GetTerrainHeight(float x, float z) {
    float base = FractalNoise(x, z, 5, 0.018f, 0.52f);
    float detail = FractalNoise(x + 300, z - 200, 4, 0.075f, 0.45f);
    float mountains = FractalNoise(x - 800, z + 500, 3, 0.006f, 0.6f);

    float ridgeBase = FractalNoise(x - 1200, z + 900, 4, 0.009f, 0.52f);
    float ridges = RidgeNoise(ridgeBase);

    float height = base * 20.0f;
    height += detail * 3.2f;
    height += powf(fmaxf(mountains, 0.0f), 2.3f) * 55.0f;
    height += ridges * 10.0f;

    height += sinf(x * 0.018f) * 2.0f;
    height += cosf(z * 0.016f) * 2.0f;

    return height;
}

inline Color GetTerrainColor(float h, float x, float z) {
    float patch = FractalNoise(x + 900, z - 400, 3, 0.055f, 0.5f);
    float dirt = FractalNoise(x - 500, z + 700, 3, 0.04f, 0.52f);
    float stone = FractalNoise(x + 1400, z + 1200, 3, 0.025f, 0.5f);

    if (h < WATER_LEVEL + 0.4f) return Color{55, 105, 75, 255};

    if (h < 5.0f) {
        if (patch > 0.28f) return Color{78, 145, 72, 255};
        if (dirt > 0.34f) return Color{92, 130, 68, 255};
        return Color{65, 165, 85, 255};
    }

    if (h < 14.0f) {
        if (patch > 0.24f) return Color{105, 125, 72, 255};
        if (dirt > 0.32f) return Color{112, 103, 74, 255};
        return Color{92, 150, 72, 255};
    }

    if (h < 25.0f) {
        if (stone > 0.18f) return Color{132, 126, 118, 255};
        return Color{120, 112, 95, 255};
    }

    if (h < 42.0f) {
        if (stone > 0.12f) return Color{145, 145, 140, 255};
        return Color{125, 118, 105, 255};
    }

    return Color{205, 210, 215, 255};
}

inline Color RandomGrassColor() {
    int green = GetRandomValue(130, 255);

    return Color{
        static_cast<unsigned char>(GetRandomValue(35, 80)),
        static_cast<unsigned char>(green),
        static_cast<unsigned char>(GetRandomValue(35, 90)),
        255
    };
}

inline Color RandomFlowerColor() {
    int type = GetRandomValue(0, 5);

    if (type == 0) return Color{255, 235, 90, 255};
    if (type == 1) return Color{255, 125, 185, 255};
    if (type == 2) return Color{180, 130, 255, 255};
    if (type == 3) return Color{255, 255, 255, 255};
    if (type == 4) return Color{255, 165, 75, 255};
    return Color{120, 190, 255, 255};
}

inline void WriteVertex(
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

inline Mesh GenerateGrassMesh(float startX, float startZ, float chunkWorldSize) {
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

inline Mesh GenerateFlowerMesh(float startX, float startZ, float chunkWorldSize) {
    // CPU optimization:
    // Instead of hundreds of DrawSphere/DrawCylinder calls per frame,
    // all flowers become one GPU mesh per chunk.
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

        // Quad 1: green stem.
        Vector3 s0 = {localX - halfStem, fy + 0.04f, localZ};
        Vector3 s1 = {localX + halfStem, fy + 0.04f, localZ};
        Vector3 s2 = {localX - halfStem, fy + stemHeight, localZ};
        Vector3 s3 = {localX + halfStem, fy + stemHeight, localZ};

        // Quad 2: colored flower head, billboard-like crossed petal card.
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

        // Stem quad.
        mesh.indices[idx++] = v + 0;
        mesh.indices[idx++] = v + 1;
        mesh.indices[idx++] = v + 2;

        mesh.indices[idx++] = v + 2;
        mesh.indices[idx++] = v + 1;
        mesh.indices[idx++] = v + 3;

        // Flower quad.
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

inline Chunk GenerateChunk(int cx, int cz, Shader grassShader) {
    Chunk chunk;
    chunk.key = {cx, cz};

    int verticesPerSide = CHUNK_RESOLUTION + 1;
    int vertexCount = verticesPerSide * verticesPerSide;
    int triangleCount = CHUNK_RESOLUTION * CHUNK_RESOLUTION * 2;

    Mesh mesh = {0};
    mesh.vertexCount = vertexCount;
    mesh.triangleCount = triangleCount;

    mesh.vertices = (float*)MemAlloc(vertexCount * 3 * sizeof(float));
    mesh.normals = (float*)MemAlloc(vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)MemAlloc(vertexCount * 2 * sizeof(float));
    mesh.colors = (unsigned char*)MemAlloc(vertexCount * 4 * sizeof(unsigned char));
    mesh.indices = (unsigned short*)MemAlloc(triangleCount * 3 * sizeof(unsigned short));

    float chunkWorldSize = CHUNK_SIZE * WORLD_SCALE;
    float startX = cx * chunkWorldSize;
    float startZ = cz * chunkWorldSize;

    int index = 0;

    for (int z = 0; z < verticesPerSide; z++) {
        for (int x = 0; x < verticesPerSide; x++) {
            float localX = ((float)x / CHUNK_RESOLUTION) * chunkWorldSize;
            float localZ = ((float)z / CHUNK_RESOLUTION) * chunkWorldSize;

            float worldX = startX + localX;
            float worldZ = startZ + localZ;
            float y = GetTerrainHeight(worldX, worldZ);

            mesh.vertices[index * 3 + 0] = localX;
            mesh.vertices[index * 3 + 1] = y;
            mesh.vertices[index * 3 + 2] = localZ;

            mesh.normals[index * 3 + 0] = 0;
            mesh.normals[index * 3 + 1] = 1;
            mesh.normals[index * 3 + 2] = 0;

            mesh.texcoords[index * 2 + 0] = (float)x / CHUNK_RESOLUTION;
            mesh.texcoords[index * 2 + 1] = (float)z / CHUNK_RESOLUTION;

            Color c = GetTerrainColor(y, worldX, worldZ);
            mesh.colors[index * 4 + 0] = c.r;
            mesh.colors[index * 4 + 1] = c.g;
            mesh.colors[index * 4 + 2] = c.b;
            mesh.colors[index * 4 + 3] = c.a;

            index++;
        }
    }

    int idx = 0;

    for (int z = 0; z < CHUNK_RESOLUTION; z++) {
        for (int x = 0; x < CHUNK_RESOLUTION; x++) {
            unsigned short tl = z * verticesPerSide + x;
            unsigned short tr = tl + 1;
            unsigned short bl = (z + 1) * verticesPerSide + x;
            unsigned short br = bl + 1;

            mesh.indices[idx++] = tl;
            mesh.indices[idx++] = bl;
            mesh.indices[idx++] = tr;

            mesh.indices[idx++] = tr;
            mesh.indices[idx++] = bl;
            mesh.indices[idx++] = br;
        }
    }

    UploadMesh(&mesh, false);

    chunk.terrainMesh = mesh;
    chunk.terrainModel = LoadModelFromMesh(chunk.terrainMesh);
    chunk.terrainModel.transform = MatrixTranslate(startX, 0, startZ);

    chunk.grassMesh = GenerateGrassMesh(startX, startZ, chunkWorldSize);
    chunk.grassModel = LoadModelFromMesh(chunk.grassMesh);
    chunk.grassModel.transform = MatrixTranslate(startX, 0, startZ);
    chunk.grassModel.materials[0].shader = grassShader;

    chunk.flowerMesh = GenerateFlowerMesh(startX, startZ, chunkWorldSize);
    chunk.hasFlowerMesh = chunk.flowerMesh.vertexCount > 0;
    if (chunk.hasFlowerMesh) {
        chunk.flowerModel = LoadModelFromMesh(chunk.flowerMesh);
        chunk.flowerModel.transform = MatrixTranslate(startX, 0, startZ);
    }

    for (int i = 0; i < 70; i++) {
        float tx = startX + GetRandomValue(0, 10000) / 10000.0f * chunkWorldSize;
        float tz = startZ + GetRandomValue(0, 10000) / 10000.0f * chunkWorldSize;
        float ty = GetTerrainHeight(tx, tz);

        if (ty > WATER_LEVEL + 0.4f && ty < 17.0f && GetRandomValue(0, 100) > 55) {
            chunk.trees.push_back({
                {tx, ty, tz},
                0.8f + GetRandomValue(0, 1000) / 1000.0f * 1.5f,
                GetRandomValue(0, 1000) / 1000.0f * 6.28f
            });
        }
    }

    for (int i = 0; i < 28; i++) {
        float rx = startX + GetRandomValue(0, 10000) / 10000.0f * chunkWorldSize;
        float rz = startZ + GetRandomValue(0, 10000) / 10000.0f * chunkWorldSize;
        float ry = GetTerrainHeight(rx, rz);

        bool mountainRock = ry > 8.0f && GetRandomValue(0, 100) > 35;
        bool meadowRock = ry <= 8.0f && ry > WATER_LEVEL + 0.5f && GetRandomValue(0, 100) > 78;

        if (mountainRock || meadowRock) {
            chunk.rocks.push_back({
                {rx, ry + 0.2f, rz},
                0.5f + GetRandomValue(0, 1000) / 1000.0f * 1.8f
            });
        }
    }

    return chunk;
}

inline void DrawWaterAroundPlayer(Vector3 playerPos, float time) {
    float size = 900.0f;
    float pulse = sinf(time * 0.7f) * 0.04f;

    DrawPlane(
        {playerPos.x, WATER_LEVEL + pulse, playerPos.z},
        Vector2{size, size},
        Fade(Color{35, 125, 180, 255}, 0.70f)
    );
}

inline void DrawNature(const std::map<ChunkKey, Chunk>& chunks, Vector3 playerPos, float time) {
    for (const auto& item : chunks) {
        const Chunk& c = item.second;

        for (const Tree& t : c.trees) {
            if (Vector3Distance(playerPos, t.position) > 120.0f) continue;

            float wind = sinf(time * 1.8f + t.sway) * 0.28f * t.size;

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

inline void DrawFlowerModels(const std::map<ChunkKey, Chunk>& chunks, Vector3 playerPos) {
    for (const auto& item : chunks) {
        const Chunk& c = item.second;

        if (!c.hasFlowerMesh) continue;

        float chunkWorldSize = CHUNK_SIZE * WORLD_SCALE;
        float chunkCenterX = c.key.x * chunkWorldSize + chunkWorldSize * 0.5f;
        float chunkCenterZ = c.key.z * chunkWorldSize + chunkWorldSize * 0.5f;

        float dist = Vector2Distance({playerPos.x, playerPos.z}, {chunkCenterX, chunkCenterZ});

        if (dist < 120.0f) {
            DrawModel(c.flowerModel, {0, 0, 0}, 1.0f, WHITE);
        }
    }
}

inline void DrawClouds(float time, Vector3 playerPos) {
    for (int i = 0; i < 32; i++) {
        float x = playerPos.x + sinf(i * 91.7f) * 520.0f;
        float z = playerPos.z + cosf(i * 71.3f) * 520.0f;

        x += sinf(time * 0.015f + i) * 90.0f;

        float y = 95.0f + sinf(i * 0.7f) * 15.0f;
        float s = 10.0f + (i % 5) * 4.0f;

        Color c = Fade(WHITE, 0.78f);
        Vector3 p = {x, y, z};

        DrawSphere(p, s, c);
        DrawSphere({p.x + s * 0.7f, p.y + 2.0f, p.z}, s * 0.8f, c);
        DrawSphere({p.x - s * 0.7f, p.y + 1.0f, p.z}, s * 0.75f, c);
        DrawSphere({p.x, p.y + 3.0f, p.z + s * 0.4f}, s * 0.7f, c);
    }
}

inline void DrawCuteAlien(Vector3 pos, float yawDeg, bool isMoving, float time) {
    float yaw = yawDeg * DEG2RAD;

    Vector3 forward = {sinf(yaw), 0, cosf(yaw)};
    Vector3 right = {cosf(yaw), 0, -sinf(yaw)};

    float walk = isMoving ? sinf(time * 10.0f) : 0.0f;
    float bodyBob = isMoving ? fabsf(sinf(time * 10.0f)) * 0.08f : 0.0f;

    float footY = pos.y + 0.02f;

    Vector3 body = {pos.x, footY + 0.78f + bodyBob, pos.z};
    Vector3 head = {pos.x, footY + 1.62f + bodyBob, pos.z};

    DrawSphere(body, 0.46f, Color{125, 255, 175, 255});
    DrawSphere(head, 0.60f, Color{155, 255, 195, 255});

    Vector3 faceCenter = Vector3Add(head, Vector3Scale(forward, 0.52f));

    DrawSphere(Vector3Add(faceCenter, Vector3Scale(right, -0.18f)), 0.12f, BLACK);
    DrawSphere(Vector3Add(faceCenter, Vector3Scale(right, 0.18f)), 0.12f, BLACK);

    Vector3 antLBase = Vector3Add(head, Vector3Scale(right, -0.25f));
    antLBase.y += 0.42f;

    Vector3 antRBase = Vector3Add(head, Vector3Scale(right, 0.25f));
    antRBase.y += 0.42f;

    Vector3 antLTop = Vector3Add(antLBase, Vector3Add(Vector3Scale(right, -0.15f), Vector3{0, 0.45f, 0}));
    Vector3 antRTop = Vector3Add(antRBase, Vector3Add(Vector3Scale(right, 0.15f), Vector3{0, 0.45f, 0}));

    DrawLine3D(antLBase, antLTop, DARKGREEN);
    DrawLine3D(antRBase, antRTop, DARKGREEN);
    DrawSphere(antLTop, 0.10f, YELLOW);
    DrawSphere(antRTop, 0.10f, YELLOW);

    float legSwing = walk * 0.32f;

    Vector3 hipL = Vector3Add(body, Vector3Scale(right, -0.22f));
    Vector3 hipR = Vector3Add(body, Vector3Scale(right, 0.22f));

    hipL.y -= 0.34f;
    hipR.y -= 0.34f;

    Vector3 footBase = {pos.x, footY, pos.z};

    Vector3 footL = Vector3Add(footBase, Vector3Scale(right, -0.22f));
    Vector3 footR = Vector3Add(footBase, Vector3Scale(right, 0.22f));

    footL = Vector3Add(footL, Vector3Scale(forward, legSwing));
    footR = Vector3Add(footR, Vector3Scale(forward, -legSwing));

    DrawLine3D(hipL, footL, Color{60, 180, 105, 255});
    DrawLine3D(hipR, footR, Color{60, 180, 105, 255});

    DrawSphere(footL, 0.13f, Color{50, 150, 90, 255});
    DrawSphere(footR, 0.13f, Color{50, 150, 90, 255});

    Vector3 armL1 = Vector3Add(body, Vector3Scale(right, -0.48f));
    Vector3 armR1 = Vector3Add(body, Vector3Scale(right, 0.48f));

    Vector3 armL2 = Vector3Add(armL1, Vector3Add(Vector3Scale(forward, -legSwing * 0.7f), Vector3{0, -0.45f, 0}));
    Vector3 armR2 = Vector3Add(armR1, Vector3Add(Vector3Scale(forward, legSwing * 0.7f), Vector3{0, -0.45f, 0}));

    DrawLine3D(armL1, armL2, Color{80, 220, 130, 255});
    DrawLine3D(armR1, armR2, Color{80, 220, 130, 255});
}
