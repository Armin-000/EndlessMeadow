#pragma once

#include "raylib.h"
#include <vector>

struct WoodDrop {
    Vector3 position;
    Vector3 velocity;
    float radius;
    float life;
    bool picked;
    bool magnetized;
    float magnetTimer;
};

void SpawnWoodDrops(
    std::vector<WoodDrop>& drops,
    Vector3 treePos,
    int count
);

void UpdateWoodDrops(
    std::vector<WoodDrop>& drops,
    Vector3 playerPos,
    float dt,
    int inventorySlots[10]
);

void DrawWoodDrops(const std::vector<WoodDrop>& drops);