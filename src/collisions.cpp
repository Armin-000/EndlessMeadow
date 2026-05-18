#include "../include/collisions.hpp"

#include "raymath.h"

#include <cmath>

bool IsInsideCylinderXZ(
    Vector3 point,
    Vector3 center,
    float radius
) {
    Vector2 p = {point.x, point.z};
    Vector2 c = {center.x, center.z};

    return Vector2Distance(p, c) < radius;
}

bool CheckTreeCollision(
    const Tree& tree,
    Vector3 playerPos,
    float playerRadius,
    float playerHeight
) {
    float trunkRadius = 0.22f * tree.size;
    float trunkHeight = 2.4f * tree.size;

    float playerFeetY = playerPos.y;
    float playerHeadY = playerPos.y + playerHeight;

    float treeBottomY = tree.position.y;
    float treeTopY = tree.position.y + trunkHeight;

    bool overlapsY =
        playerHeadY > treeBottomY &&
        playerFeetY < treeTopY;

    bool overlapsXZ =
        IsInsideCylinderXZ(
            playerPos,
            tree.position,
            playerRadius + trunkRadius
        );

    return overlapsY && overlapsXZ;
}

float GetRockTopYAtPoint(
    const Rock& rock,
    Vector3 playerPos
) {
    float radiusX = rock.size * 0.85f;
    float radiusZ = rock.size * 0.65f;
    float height = rock.size * 0.72f;

    float dx = playerPos.x - rock.position.x;
    float dz = playerPos.z - rock.position.z;

    float nx = dx / radiusX;
    float nz = dz / radiusZ;

    float d = nx * nx + nz * nz;

    if (d > 1.0f) {
        return -999999.0f;
    }

    float dome = sqrtf(1.0f - d);

    return rock.position.y + dome * height;
}

bool CheckRockSideCollision(
    const Rock& rock,
    Vector3 playerPos,
    float playerRadius,
    float playerHeight
) {
    float rockRadius = rock.size * 0.85f;
    float rockBottomY = rock.position.y - 0.1f;
    float rockTopY = rock.position.y + rock.size * 0.72f;

    float playerFeetY = playerPos.y;
    float playerHeadY = playerPos.y + playerHeight;

    bool overlapsY =
        playerHeadY > rockBottomY &&
        playerFeetY < rockTopY - 0.25f;

    bool overlapsXZ =
        IsInsideCylinderXZ(
            playerPos,
            rock.position,
            playerRadius + rockRadius
        );

    return overlapsY && overlapsXZ;
}

bool CheckNatureCollision(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 newPlayerPos,
    float playerRadius,
    float playerHeight
) {
    for (const auto& item : chunks) {
        const Chunk& c = item.second;

        for (const Tree& t : c.trees) {
            if (CheckTreeCollision(
                t,
                newPlayerPos,
                playerRadius,
                playerHeight
            )) {
                return true;
            }
        }

        for (const Rock& r : c.rocks) {
            if (CheckRockSideCollision(
                r,
                newPlayerPos,
                playerRadius,
                playerHeight
            )) {
                return true;
            }
        }
    }

    return false;
}

float GetSmartGroundY(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 playerPos,
    float terrainY,
    float playerRadius,
    float velocityY
) {
    float groundY = terrainY;

    if (velocityY > 0.0f) {
        return groundY;
    }

    for (const auto& item : chunks) {
        const Chunk& c = item.second;

        for (const Rock& r : c.rocks) {
            float rockTopY = GetRockTopYAtPoint(r, playerPos);

            if (rockTopY < -99999.0f) {
                continue;
            }

            bool closeEnoughToTop =
                playerPos.y >= rockTopY - 0.65f &&
                playerPos.y <= rockTopY + 1.2f;

            if (closeEnoughToTop) {
                groundY = fmaxf(groundY, rockTopY);
            }
        }
    }

    return groundY;
}