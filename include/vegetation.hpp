#pragma once

#include "raylib.h"
#include "world_types.hpp"

#include <map>

extern const char* grassVertexShader;
extern const char* grassFragmentShader;

Color RandomGrassColor();
Color RandomFlowerColor();

void WriteVertex(
    Mesh& mesh,
    int vertexIndex,
    Vector3 p,
    Vector3 n,
    Color c
);

Mesh GenerateGrassMesh(float startX, float startZ, float chunkWorldSize);
Mesh GenerateFlowerMesh(float startX, float startZ, float chunkWorldSize);

void DrawNature(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 playerPos,
    float time
);

void DrawFlowerModels(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 playerPos
);