#pragma once

#include "raylib.h"
#include "world_types.hpp"

#include <map>

void DrawWorldMapOverlay(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 playerPos,
    bool visible
);