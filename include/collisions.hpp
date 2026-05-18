#pragma once

#include "raylib.h"
#include "world_types.hpp"

#include <map>

bool IsInsideCylinderXZ(
    Vector3 point,
    Vector3 center,
    float radius
);

bool CheckTreeCollision(
    const Tree& tree,
    Vector3 playerPos,
    float playerRadius,
    float playerHeight
);

float GetRockTopYAtPoint(
    const Rock& rock,
    Vector3 playerPos
);

bool CheckRockSideCollision(
    const Rock& rock,
    Vector3 playerPos,
    float playerRadius,
    float playerHeight
);

bool CheckNatureCollision(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 newPlayerPos,
    float playerRadius,
    float playerHeight
);

float GetSmartGroundY(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 playerPos,
    float terrainY,
    float playerRadius,
    float velocityY
);