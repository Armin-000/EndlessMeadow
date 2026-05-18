#pragma once

#include "raylib.h"
#include "world_types.hpp"

float HashNoise(int x, int z);
float SmoothNoise(float x, float z);
float FractalNoise(float x, float z, int octaves, float frequency, float persistence);
float RidgeNoise(float n);

float GetTerrainHeight(float x, float z);
Color GetTerrainColor(float h, float x, float z);

void RecalculateTerrainNormals(Mesh& mesh, int verticesPerSide);

Chunk GenerateChunk(int cx, int cz, Shader grassShader);