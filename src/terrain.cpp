#include "../include/terrain.hpp"
#include "../include/vegetation.hpp"

#include "raymath.h"

#include <cmath>
#include <cstdlib>

float HashNoise(int x, int z) {
    int n = x * 374761393 + z * 668265263;
    n = (n ^ (n >> 13)) * 1274126177;
    return 1.0f - ((n & 0x7fffffff) / 1073741824.0f);
}

float SmoothNoise(float x, float z) {
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

float FractalNoise(float x, float z, int octaves, float frequency, float persistence) {
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

float RidgeNoise(float n) {
    n = 1.0f - fabsf(n);
    return n * n;
}

float GetTerrainHeight(float x, float z) {
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

Color GetTerrainColor(float h, float x, float z) {
    float patch = FractalNoise(x + 900, z - 400, 3, 0.045f, 0.5f);
    float dirt = FractalNoise(x - 500, z + 700, 3, 0.035f, 0.52f);
    float stone = FractalNoise(x + 1400, z + 1200, 3, 0.022f, 0.5f);
    float cold = FractalNoise(x - 2200, z + 1700, 2, 0.012f, 0.55f);

    if (h < WATER_LEVEL + 0.4f) return Color{42, 86, 70, 255};

    if (h < 5.0f) {
        if (patch > 0.28f) return Color{72, 118, 67, 255};
        if (dirt > 0.30f) return Color{95, 92, 66, 255};
        return Color{58, 125, 70, 255};
    }

    if (h < 14.0f) {
        if (patch > 0.22f) return Color{91, 108, 68, 255};
        if (dirt > 0.28f) return Color{105, 94, 70, 255};
        return Color{74, 118, 67, 255};
    }

    if (h < 25.0f) {
        if (stone > 0.12f) return Color{112, 108, 98, 255};
        return Color{95, 92, 78, 255};
    }

    if (h < 42.0f) {
        if (cold > 0.0f) return Color{142, 148, 150, 255};
        return Color{118, 116, 108, 255};
    }

    return Color{205, 210, 212, 255};
}

void RecalculateTerrainNormals(Mesh& mesh, int verticesPerSide) {
    for (int i = 0; i < mesh.vertexCount * 3; i++) {
        mesh.normals[i] = 0.0f;
    }

    for (int i = 0; i < mesh.triangleCount * 3; i += 3) {
        int i0 = mesh.indices[i + 0];
        int i1 = mesh.indices[i + 1];
        int i2 = mesh.indices[i + 2];

        Vector3 v0 = {
            mesh.vertices[i0 * 3 + 0],
            mesh.vertices[i0 * 3 + 1],
            mesh.vertices[i0 * 3 + 2]
        };

        Vector3 v1 = {
            mesh.vertices[i1 * 3 + 0],
            mesh.vertices[i1 * 3 + 1],
            mesh.vertices[i1 * 3 + 2]
        };

        Vector3 v2 = {
            mesh.vertices[i2 * 3 + 0],
            mesh.vertices[i2 * 3 + 1],
            mesh.vertices[i2 * 3 + 2]
        };

        Vector3 edge1 = Vector3Subtract(v1, v0);
        Vector3 edge2 = Vector3Subtract(v2, v0);
        Vector3 normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));

        mesh.normals[i0 * 3 + 0] += normal.x;
        mesh.normals[i0 * 3 + 1] += normal.y;
        mesh.normals[i0 * 3 + 2] += normal.z;

        mesh.normals[i1 * 3 + 0] += normal.x;
        mesh.normals[i1 * 3 + 1] += normal.y;
        mesh.normals[i1 * 3 + 2] += normal.z;

        mesh.normals[i2 * 3 + 0] += normal.x;
        mesh.normals[i2 * 3 + 1] += normal.y;
        mesh.normals[i2 * 3 + 2] += normal.z;
    }

    for (int i = 0; i < mesh.vertexCount; i++) {
        Vector3 n = {
            mesh.normals[i * 3 + 0],
            mesh.normals[i * 3 + 1],
            mesh.normals[i * 3 + 2]
        };

        n = Vector3Normalize(n);

        mesh.normals[i * 3 + 0] = n.x;
        mesh.normals[i * 3 + 1] = n.y;
        mesh.normals[i * 3 + 2] = n.z;
    }
}

Chunk GenerateChunk(int cx, int cz, Shader grassShader) {
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

    RecalculateTerrainNormals(mesh, verticesPerSide);

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