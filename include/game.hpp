#pragma once

#include "world.hpp"
#include "map.hpp"
#include "inventory.hpp"

#include "raylib.h"
#include "raymath.h"

#include <cmath>
#include <map>
#include <vector>
#include <cstdlib>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const float PLAYER_SPEED = 18.0f;
const float PLAYER_SPRINT_MULTIPLIER = 3.2f;
const float PLAYER_FLY_SPEED = 24.0f;

const float JUMP_FORCE = 15.0f;
const float GRAVITY = 38.0f;

inline void DrawLoadingScreen(Texture2D preloadTexture, int loadedInitialChunks, int totalInitialChunks) {
    float progress = (float)loadedInitialChunks / (float)totalInitialChunks;

    BeginDrawing();
    ClearBackground(BLACK);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float scale = fmaxf(
        (float)screenW / preloadTexture.width,
        (float)screenH / preloadTexture.height
    );

    float drawW = preloadTexture.width * scale;
    float drawH = preloadTexture.height * scale;

    DrawTextureEx(
        preloadTexture,
        Vector2{(screenW - drawW) / 2.0f, (screenH - drawH) / 2.0f},
        0.0f,
        scale,
        WHITE
    );

    DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.18f));

    const char* title = "ENDLESS MEADOW";
    const char* subtitle = "Generating procedural world...";

    DrawText(title, screenW / 2 - MeasureText(title, 46) / 2, screenH - 170, 46, WHITE);
    DrawText(subtitle, screenW / 2 - MeasureText(subtitle, 22) / 2, screenH - 112, 22, RAYWHITE);

    int barW = screenW * 0.42f;
    int barH = 18;
    int barX = screenW / 2 - barW / 2;
    int barY = screenH - 70;

    DrawRectangleRounded(
        Rectangle{(float)barX, (float)barY, (float)barW, (float)barH},
        0.45f,
        16,
        Fade(BLACK, 0.55f)
    );

    DrawRectangleRounded(
        Rectangle{(float)barX, (float)barY, barW * progress, (float)barH},
        0.45f,
        16,
        Color{90, 255, 130, 255}
    );

    DrawText(
        TextFormat("%i%%", (int)(progress * 100.0f)),
        screenW / 2 - 22,
        screenH - 42,
        24,
        WHITE
    );

    EndDrawing();
}

inline void UnloadChunks(std::map<ChunkKey, Chunk>& chunks) {
    for (auto& item : chunks) {
        UnloadModel(item.second.terrainModel);
        UnloadModel(item.second.grassModel);

        if (item.second.hasFlowerMesh) {
            UnloadModel(item.second.flowerModel);
        }
    }

    chunks.clear();
}

inline void RunGame() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Endless Meadow - GPU Grass C++ World");

    ChangeDirectory(GetApplicationDirectory());

    Texture2D preloadTexture = LoadTexture("assets/preload.png");

    if (preloadTexture.id == 0) {
        TraceLog(LOG_ERROR, "Nije ucitana assets/preload.png");
    }
    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    SetExitKey(KEY_NULL);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    MaximizeWindow();
    SetTargetFPS(60);

    Shader grassShader = LoadShaderFromMemory(grassVertexShader, grassFragmentShader);
    int grassTimeLoc = GetShaderLocation(grassShader, "time");
    int windStrengthLoc = GetShaderLocation(grassShader, "windStrength");

    Shader waterShader = LoadShaderFromMemory(waterVertexShader, waterFragmentShader);
    int waterTimeLoc = GetShaderLocation(waterShader, "time");

    Mesh waterMesh = GenMeshPlane(760.0f, 760.0f, 140, 140);

    Model waterModel = LoadModelFromMesh(waterMesh);

    waterModel.materials[0].shader = waterShader;

    float windStrength = 0.65f;

    Camera3D camera = {0};
    camera.position = {0, 12, 20};
    camera.target = {0, 2, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 playerPos = {0, 0, 0};
    Vector3 velocity = {0, 0, 0};

    float playerYaw = 180.0f;
    bool isMoving = false;

    float cameraYaw = 0.0f;
    float cameraPitch = 13.0f;
    float cameraDistance = 30.0f;

    bool grounded = false;
    bool pauseMenu = false;
    bool showMap = false;
    bool showInventory = false;
    int inventorySlots[10] = {0};
    int selectedSlot = -1;

    float attackTimer = 0.0f;
    float boomTimer = 0.0f;
    Vector3 boomPos = {0, 0, 0};

    bool flyMode = false;
    float lastSpacePressTime = -10.0f;

    std::map<ChunkKey, Chunk> chunks;

    DisableCursor();

    float chunkWorldSize = CHUNK_SIZE * WORLD_SCALE;
    int pcx = 0;
    int pcz = 0;

    int totalInitialChunks = (VIEW_DISTANCE * 2 + 1) * (VIEW_DISTANCE * 2 + 1);
    int loadedInitialChunks = 0;

    for (int z = -VIEW_DISTANCE; z <= VIEW_DISTANCE; z++) {
        for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; x++) {
            ChunkKey key = {pcx + x, pcz + z};

            DrawLoadingScreen(preloadTexture, loadedInitialChunks, totalInitialChunks);

            chunks[key] = GenerateChunk(key.x, key.z, grassShader);

            loadedInitialChunks++;
        }
    }

    EnableCursor();
    bool waitingForStart = true;

    while (waitingForStart && !WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        float scale = fmaxf(
            (float)screenW / preloadTexture.width,
            (float)screenH / preloadTexture.height
        );

        float drawW = preloadTexture.width * scale;
        float drawH = preloadTexture.height * scale;

        DrawTextureEx(
            preloadTexture,
            Vector2{
                (screenW - drawW) / 2.0f,
                (screenH - drawH) / 2.0f
            },
            0.0f,
            scale,
            WHITE
        );

        DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.22f));

        const char* title = "ENDLESS MEADOW";

        DrawText(
            title,
            screenW / 2 - MeasureText(title, 58) / 2,
            120,
            58,
            WHITE
        );

        Rectangle startBtn = {
            screenW / 2.0f - 180,
            screenH / 2.0f + 40,
            360,
            72
        };

        Rectangle exitBtn = {
            screenW / 2.0f - 180,
            screenH / 2.0f + 132,
            360,
            72
        };

        bool hoverStart = CheckCollisionPointRec(GetMousePosition(), startBtn);
        bool hoverExit = CheckCollisionPointRec(GetMousePosition(), exitBtn);

        Color startGlow = hoverStart
            ? Color{120, 255, 170, 255}
            : Color{70, 220, 130, 235};

        Color exitGlow = hoverExit
            ? Color{255, 120, 120, 255}
            : Color{185, 70, 70, 225};

        DrawRectangleRounded(
            Rectangle{
                startBtn.x - 4,
                startBtn.y - 4,
                startBtn.width + 8,
                startBtn.height + 8
            },
            0.30f,
            24,
            Fade(startGlow, 0.18f)
        );

        DrawRectangleRounded(
            startBtn,
            0.30f,
            24,
            Fade(BLACK, 0.55f)
        );

        DrawRectangleRoundedLines(
            startBtn,
            0.30f,
            24,
            startGlow
        );

        DrawRectangleRounded(
            Rectangle{
                exitBtn.x - 4,
                exitBtn.y - 4,
                exitBtn.width + 8,
                exitBtn.height + 8
            },
            0.30f,
            24,
            Fade(exitGlow, 0.15f)
        );

        DrawRectangleRounded(
            exitBtn,
            0.30f,
            24,
            Fade(BLACK, 0.55f)
        );

        DrawRectangleRoundedLines(
            exitBtn,
            0.30f,
            24,
            exitGlow
        );

        DrawText(
            "START GAME",
            screenW / 2 - MeasureText("START GAME", 36) / 2,
            screenH / 2 + 58,
            36,
            WHITE
        );

        DrawText(
            "EXIT",
            screenW / 2 - MeasureText("EXIT", 34) / 2,
            screenH / 2 + 150,
            34,
            RAYWHITE
        );

        const char* author = "By Armin Lisic";

        DrawText(
            author,
            screenW - MeasureText(author, 20) - 26,
            screenH - 34,
            20,
            Fade(RAYWHITE, 0.82f)
        );

        EndDrawing();

        if (hoverStart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            DisableCursor();
            waitingForStart = false;
        }

        if (hoverExit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            UnloadTexture(preloadTexture);
            UnloadShader(grassShader);
            CloseWindow();
            return;
        }
    }
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float time = GetTime();

        if (attackTimer > 0.0f) attackTimer -= dt;
        if (boomTimer > 0.0f) boomTimer -= dt;

        for (auto& item : chunks) {
            for (Tree& tree : item.second.trees) {
                if (tree.shakeTimer > 0.0f) {
                    tree.shakeTimer -= dt;
                }
            }
        }

        SetShaderValue(grassShader, grassTimeLoc, &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(grassShader, windStrengthLoc, &windStrength, SHADER_UNIFORM_FLOAT);
        SetShaderValue(waterShader, waterTimeLoc, &time, SHADER_UNIFORM_FLOAT);

        if (!pauseMenu) {
            Vector2 mouse = GetMouseDelta();
            cameraYaw -= mouse.x * 0.18f;
            cameraPitch += mouse.y * 0.12f;
        }

        if (cameraPitch < -89.0f) cameraPitch = -89.0f;
        if (cameraPitch > 89.0f) cameraPitch = 89.0f;

        if (IsKeyPressed(KEY_ESCAPE)) {
            pauseMenu = !pauseMenu;

            if (pauseMenu) EnableCursor();
            else DisableCursor();
        }

        if (!pauseMenu && IsKeyPressed(KEY_E)) {
            showInventory = !showInventory;

            selectedSlot = -1;

            if (showInventory) {
                EnableCursor();
            } else {
                DisableCursor();
            }
        }

        if (!pauseMenu && IsKeyPressed(KEY_M)) {
            showMap = !showMap;
        }

        float yawRad = cameraYaw * DEG2RAD;

        Vector3 forward = {-sinf(yawRad), 0, -cosf(yawRad)};
        Vector3 right = {cosf(yawRad), 0, -sinf(yawRad)};

        Vector3 input = {0, 0, 0};

        if (!pauseMenu && !showInventory) {
            if (IsKeyDown(KEY_W)) input = Vector3Add(input, forward);
            if (IsKeyDown(KEY_S)) input = Vector3Subtract(input, forward);
            if (IsKeyDown(KEY_D)) input = Vector3Add(input, right);
            if (IsKeyDown(KEY_A)) input = Vector3Subtract(input, right);
        }

        isMoving = false;

        float currentSpeed = PLAYER_SPEED;

        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            currentSpeed *= PLAYER_SPRINT_MULTIPLIER;
        }

        if (flyMode) {
            currentSpeed = PLAYER_FLY_SPEED;

            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                currentSpeed *= PLAYER_SPRINT_MULTIPLIER;
            }
        }

        if (Vector3Length(input) > 0.01f) {
            input = Vector3Normalize(input);

            float playerRadius = 0.42f;
            float playerHeight = 1.9f;

            Vector3 nextX = playerPos;
            nextX.x += input.x * currentSpeed * dt;

            if (!CheckNatureCollision(chunks, nextX, playerRadius, playerHeight)) {
                playerPos.x = nextX.x;
            }

            Vector3 nextZ = playerPos;
            nextZ.z += input.z * currentSpeed * dt;

            if (!CheckNatureCollision(chunks, nextZ, playerRadius, playerHeight)) {
                playerPos.z = nextZ.z;
            }

            playerYaw = atan2f(input.x, input.z) * RAD2DEG;
            isMoving = true;
        }

        if (!pauseMenu && !showInventory && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            const float hitRange = 4.2f;
            const float minDot = 0.45f;

            Vector3 attackForward = {
                sinf(playerYaw * DEG2RAD),
                0.0f,
                cosf(playerYaw * DEG2RAD)
            };

            Chunk* targetChunk = nullptr;
            int targetTreeIndex = -1;
            float bestDistance = 99999.0f;

            for (auto& item : chunks) {
                Chunk& chunk = item.second;

                for (int i = 0; i < (int)chunk.trees.size(); i++) {
                    Tree& tree = chunk.trees[i];

                    Vector3 toTree = Vector3Subtract(tree.position, playerPos);
                    toTree.y = 0.0f;

                    float dist = Vector3Length(toTree);

                    if (dist > hitRange || dist < 0.01f) {
                        continue;
                    }

                    Vector3 dirToTree = Vector3Normalize(toTree);
                    float dot = Vector3DotProduct(attackForward, dirToTree);

                    if (dot > minDot && dist < bestDistance) {
                        bestDistance = dist;
                        targetChunk = &chunk;
                        targetTreeIndex = i;
                    }
                }
            }

            if (targetChunk != nullptr && targetTreeIndex >= 0) {
                Tree& tree = targetChunk->trees[targetTreeIndex];

                tree.hitCount++;
                tree.shakeTimer = 0.25f;
                attackTimer = 0.22f;

                if (tree.hitCount >= 3) {
                    boomPos = tree.position;
                    boomPos.y += 1.6f;
                    boomTimer = 0.45f;

                    targetChunk->trees.erase(
                        targetChunk->trees.begin() + targetTreeIndex
                    );

                    bool addedWood = false;

                    for (int i = 0; i < 10; i++) {
                        if (inventorySlots[i] > 0) {
                            inventorySlots[i] += 3;
                            addedWood = true;
                            break;
                        }
                    }

                    if (!addedWood) {
                        inventorySlots[0] = 3;
                    }
                }
            }
        }
        float terrainY = GetTerrainHeight(playerPos.x, playerPos.z);

        float playerRadius = 0.42f;
        float playerHeight = 1.9f;

        float groundY = GetSmartGroundY(
            chunks,
            playerPos,
            terrainY,
            playerRadius,
            velocity.y
        );

        if (!pauseMenu && IsKeyPressed(KEY_SPACE)) {
            float now = GetTime();

            if (now - lastSpacePressTime < 0.32f) {
                flyMode = !flyMode;
                velocity.y = 0.0f;

                if (flyMode) {
                    playerPos.y += 2.0f;
                    grounded = false;
                }
            } else if (!flyMode && grounded) {
                velocity.y = JUMP_FORCE;
                grounded = false;
            }

            lastSpacePressTime = now;
        }

        if (flyMode) {
            velocity.y = 0.0f;

            if (!pauseMenu && IsKeyDown(KEY_SPACE)) {
                playerPos.y += PLAYER_FLY_SPEED * dt;
            }

            if (!pauseMenu && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
                playerPos.y -= PLAYER_FLY_SPEED * dt;
            }

            float minFlyY = terrainY + 1.2f;
            if (playerPos.y < minFlyY) {
                playerPos.y = minFlyY;
            }
        } else {
            velocity.y -= GRAVITY * dt;
            playerPos.y += velocity.y * dt;

            if (playerPos.y <= groundY) {
                playerPos.y = groundY;
                velocity.y = 0;
                grounded = true;
            } else {
                grounded = false;
            }
        }

        chunkWorldSize = CHUNK_SIZE * WORLD_SCALE;
        pcx = (int)floorf(playerPos.x / chunkWorldSize);
        pcz = (int)floorf(playerPos.z / chunkWorldSize);

        for (int z = -VIEW_DISTANCE; z <= VIEW_DISTANCE; z++) {
            for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; x++) {
                ChunkKey key = {pcx + x, pcz + z};

                if (chunks.find(key) == chunks.end()) {
                    chunks[key] = GenerateChunk(key.x, key.z, grassShader);
                }
            }
        }

        std::vector<ChunkKey> remove;

        for (const auto& item : chunks) {
            int dx = item.first.x - pcx;
            int dz = item.first.z - pcz;

            if (abs(dx) > VIEW_DISTANCE + 1 || abs(dz) > VIEW_DISTANCE + 1) {
                remove.push_back(item.first);
            }
        }

        for (const ChunkKey& key : remove) {
            UnloadModel(chunks[key].terrainModel);
            UnloadModel(chunks[key].grassModel);

            if (chunks[key].hasFlowerMesh) {
                UnloadModel(chunks[key].flowerModel);
            }

            chunks.erase(key);
        }

        float pitchRad = cameraPitch * DEG2RAD;

        camera.up = {0, 1, 0};

        if (cameraPitch > 89.0f) cameraPitch = 89.0f;
        if (cameraPitch < -89.0f) cameraPitch = -89.0f;

        Vector3 camOffset = {
            sinf(yawRad) * cosf(pitchRad) * cameraDistance,
            sinf(pitchRad) * cameraDistance,
            cosf(yawRad) * cosf(pitchRad) * cameraDistance
        };

        camera.target = {playerPos.x, playerPos.y + 1.4f, playerPos.z};
        camera.position = Vector3Add(camera.target, camOffset);

        float cameraGroundY = GetTerrainHeight(camera.position.x, camera.position.z) + 1.2f;

        if (camera.position.y < cameraGroundY) {
            camera.position.y = cameraGroundY;
        }

        BeginDrawing();
        ClearBackground(Color{135, 170, 190, 255});

        BeginMode3D(camera);
        DrawClouds(time, playerPos);

        float sunTime = GetTime() * 0.03f;

        Vector3 sunPos = {
            cosf(sunTime) * 420.0f,
            180.0f + sinf(sunTime) * 90.0f,
            140.0f
        };

        DrawSphere(sunPos, 28.0f, Color{255, 238, 190, 255});
        DrawSphere(sunPos, 70.0f, Fade(Color{255, 196, 105, 255}, 0.16f));
        DrawSphere(sunPos, 125.0f, Fade(Color{255, 150, 80, 255}, 0.07f));
        DrawSphere(sunPos, 210.0f, Fade(Color{255, 120, 60, 255}, 0.025f));

        for (const auto& item : chunks) {
            const Chunk& c = item.second;

            float chunkCenterX = c.key.x * chunkWorldSize + chunkWorldSize * 0.5f;
            float chunkCenterZ = c.key.z * chunkWorldSize + chunkWorldSize * 0.5f;
            float dist = Vector2Distance({playerPos.x, playerPos.z}, {chunkCenterX, chunkCenterZ});

            DrawModel(c.terrainModel, {0, 0, 0}, 1.0f, WHITE);

            if (dist < 145.0f) {
                DrawModel(c.grassModel, {0, 0, 0}, 1.0f, WHITE);
            }
        }

        DrawWaterAroundPlayer(waterModel, playerPos);

        // GPU flower rendering
        DrawFlowerModels(chunks, playerPos);

        DrawNature(chunks, playerPos, time);
        bool isSprinting = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

            DrawCuteAlien(
                playerPos,
                playerYaw,
                isMoving,
                isSprinting,
                velocity.y,
                grounded,
                time,
                attackTimer
            );

            if (boomTimer > 0.0f) {
                float p = boomTimer / 0.45f;
                float size = (1.0f - p) * 2.4f;

                DrawSphere(boomPos, size, Fade(Color{220, 220, 210, 255}, p));

                DrawSphere(
                    {boomPos.x + 0.8f, boomPos.y + 0.3f, boomPos.z},
                    size * 0.55f,
                    Fade(Color{240, 240, 230, 255}, p)
                );

                DrawSphere(
                    {boomPos.x - 0.7f, boomPos.y - 0.1f, boomPos.z + 0.4f},
                    size * 0.45f,
                    Fade(Color{210, 210, 200, 255}, p)
                );
            }

        EndMode3D();
        DrawWorldMapOverlay(chunks, playerPos, showMap);
        DrawInventoryOverlay(showInventory, inventorySlots, selectedSlot);

        DrawRectangle(16, 16, 520, 220, Fade(BLACK, 0.48f));
        DrawText("Endless Meadow - GPU Grass Prototype", 28, 28, 20, WHITE);
        DrawText("WASD = move | SHIFT = sprint", 28, 60, 18, RAYWHITE);
        DrawText("SPACE = jump | Double SPACE = fly mode", 28, 86, 18, RAYWHITE);
        DrawText("Fly mode: SPACE up | CTRL down", 28, 112, 18, RAYWHITE);
        DrawText("Mouse = camera | M = map | E = backpack | ESC = pause", 28, 138, 18, RAYWHITE);
        DrawText(TextFormat("Chunks: %i | Grass/chunk: %i | FPS: %i", (int)chunks.size(), GRASS_PER_CHUNK, GetFPS()), 28, 166, 18, RAYWHITE);

        if (flyMode) {
            DrawText("FLY MODE ACTIVE", 28, 192, 18, SKYBLUE);
        } else {
            DrawText("GROUND MODE", 28, 192, 18, GREEN);
        }

        if (pauseMenu) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));

            int boxW = 360;
            int boxH = 220;
            int boxX = GetScreenWidth() / 2 - boxW / 2;
            int boxY = GetScreenHeight() / 2 - boxH / 2;

            DrawRectangleRounded(
                Rectangle{(float)boxX, (float)boxY, (float)boxW, (float)boxH},
                0.16f,
                16,
                Color{25, 35, 30, 245}
            );

            DrawText("PAUSED", boxX + 118, boxY + 35, 32, WHITE);
            DrawText("Press ESC to continue", boxX + 88, boxY + 83, 18, RAYWHITE);

            Rectangle exitBtn = {
                (float)boxX + 80,
                (float)boxY + 130,
                200,
                52
            };

            bool hoverExit = CheckCollisionPointRec(GetMousePosition(), exitBtn);

            DrawRectangleRounded(
                exitBtn,
                0.25f,
                16,
                hoverExit ? Color{220, 80, 70, 255} : Color{165, 55, 50, 255}
            );

            DrawText("EXIT", boxX + 150, boxY + 145, 24, WHITE);

            if (hoverExit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                break;
            }
        }

        EndDrawing();
    }

    UnloadChunks(chunks);

    UnloadModel(waterModel);
    UnloadShader(waterShader);

    UnloadShader(grassShader);
    UnloadTexture(preloadTexture);

    CloseWindow();
}