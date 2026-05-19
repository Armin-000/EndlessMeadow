#include "../include/item_drops.hpp"
#include "../include/terrain.hpp"

#include "raymath.h"
#include <cmath>
#include "rlgl.h"
#include <algorithm>

static float RandomFloat(float min, float max)
{
    return min + (float)GetRandomValue(0, 10000) / 10000.0f * (max - min);
}

static void AddWoodToInventory(int inventorySlots[10], int amount)
{
    for (int i = 0; i < 10; i++) {
        if (inventorySlots[i] > 0) {
            inventorySlots[i] += amount;
            return;
        }
    }

    for (int i = 0; i < 10; i++) {
        if (inventorySlots[i] == 0) {
            inventorySlots[i] = amount;
            return;
        }
    }
}

void SpawnWoodDrops(
    std::vector<WoodDrop>& drops,
    Vector3 treePos,
    int count
) {
    for (int i = 0; i < count; i++) {
        float angle = RandomFloat(0.0f, PI * 2.0f);
        float speed = RandomFloat(1.8f, 3.4f);

        WoodDrop drop;
        drop.position = {
            treePos.x,
            treePos.y + 1.0f,
            treePos.z
        };

        drop.velocity = {
            cosf(angle) * speed,
            RandomFloat(2.8f, 4.2f),
            sinf(angle) * speed
        };

        drop.radius = RandomFloat(0.11f, 0.16f);
        drop.life = 0.0f;
        drop.picked = false;
        drop.magnetized = false;
        drop.magnetTimer = 0.0f;

        drops.push_back(drop);
    }
}

void UpdateWoodDrops(
    std::vector<WoodDrop>& drops,
    Vector3 playerPos,
    float dt,
    int inventorySlots[10]
) {
    const float gravity = 18.0f;
    const float pickupDistance = 2.0f;

    for (WoodDrop& drop : drops) {
        if (drop.picked) continue;

        drop.life += dt;

        float groundY = GetTerrainHeight(drop.position.x, drop.position.z);

        // VAŽNO: pošto smo povećali drva, centar mora biti više iznad tla
        float visualGroundOffset = drop.radius * 1.45f;

        if (!drop.magnetized) {
            drop.velocity.y -= gravity * dt;

            drop.position.x += drop.velocity.x * dt;
            drop.position.y += drop.velocity.y * dt;
            drop.position.z += drop.velocity.z * dt;
        }

        if (!drop.magnetized && drop.position.y < groundY + visualGroundOffset) {
            drop.position.y = groundY + visualGroundOffset;

            if (fabsf(drop.velocity.y) > 0.45f) {
                drop.velocity.y *= -0.28f;
            } else {
                drop.velocity.y = 0.0f;
            }

            drop.velocity.x *= 0.78f;
            drop.velocity.z *= 0.78f;
        }

        // Pickup samo po XZ udaljenosti, ne po Y
        Vector2 dropXZ = {drop.position.x, drop.position.z};
        Vector2 playerXZ = {playerPos.x, playerPos.z};

        float distXZ = Vector2Distance(dropXZ, playerXZ);

        if (distXZ < pickupDistance && !drop.magnetized) {
            drop.magnetized = true;
            drop.magnetTimer = 0.0f;
        }

        if (drop.magnetized) {
            drop.magnetTimer += dt;

            Vector3 target = {
                playerPos.x,
                playerPos.y + 1.15f,
                playerPos.z
            };

            Vector3 toPlayer = Vector3Subtract(target, drop.position);
            float distToPlayer = Vector3Length(toPlayer);

            if (distToPlayer > 0.01f) {
                Vector3 dir = Vector3Normalize(toPlayer);

                float magnetSpeed = 8.0f + drop.magnetTimer * 18.0f;

                drop.position = Vector3Add(
                    drop.position,
                    Vector3Scale(dir, magnetSpeed * dt)
                );
            }

            drop.velocity = {0.0f, 0.0f, 0.0f};

            if (distToPlayer < 0.35f || drop.magnetTimer > 0.8f) {
                AddWoodToInventory(inventorySlots, 1);
                drop.picked = true;
            }
        }
    }

    drops.erase(
        std::remove_if(
            drops.begin(),
            drops.end(),
            [](const WoodDrop& drop) {
                return drop.picked;
            }
        ),
        drops.end()
    );
}

void DrawWoodDrops(const std::vector<WoodDrop>& drops)
{
    for (const WoodDrop& drop : drops) {
        if (drop.picked) continue;

        float spin = drop.magnetized
            ? drop.life * 720.0f
            : drop.life * 180.0f;

        rlPushMatrix();

            rlTranslatef(drop.position.x, drop.position.y, drop.position.z);
            rlRotatef(spin, 1.0f, 1.0f, 0.0f);

        float length = drop.radius * 4.2f;
        float thickness = drop.radius * 1.15f;

        DrawCylinder(
            {0.0f, 0.0f, 0.0f},
            thickness,
            thickness,
            length,
            12,
            Color{125, 76, 38, 255}
        );

        DrawCylinderWires(
            {0.0f, 0.0f, 0.0f},
            thickness,
            thickness,
            length,
            12,
            Color{70, 40, 22, 255}
        );

        // Jedan svjetliji kraj trupca
        DrawCylinder(
            {0.0f, length * 0.51f, 0.0f},
            thickness * 0.92f,
            thickness * 0.92f,
            0.025f,
            12,
            Color{180, 125, 70, 255}
        );

        rlPopMatrix();
    }
}