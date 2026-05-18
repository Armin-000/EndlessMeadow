#pragma once

#include "raylib.h"

#include <vector>

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
    int hitCount = 0;
    float shakeTimer = 0.0f;
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