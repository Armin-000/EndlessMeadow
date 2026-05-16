#pragma once

#include "world.hpp"

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

inline void DrawLoadingScreen(int loadedInitialChunks, int totalInitialChunks) {
    float progress = (float)loadedInitialChunks / (float)totalInitialChunks;

    BeginDrawing();

    ClearBackground(Color{18, 28, 24, 255});

    const char* title = "ENDLESS MEADOW";
    const char* subtitle = "Generating procedural world...";

    int titleSize = 42;
    int subSize = 20;

    int titleWidth = MeasureText(title, titleSize);
    int subWidth = MeasureText(subtitle, subSize);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    int centerX = screenW / 2;
    int centerY = screenH / 2;

    DrawText(title, centerX - titleWidth / 2, centerY - 120, titleSize, WHITE);
    DrawText(subtitle, centerX - subWidth / 2, centerY - 62, subSize, RAYWHITE);

    int barW = 460;
    int barH = 22;

    int barX = centerX - barW / 2;
    int barY = centerY;

    DrawRectangleRounded(
        Rectangle{(float)barX, (float)barY, (float)barW, (float)barH},
        0.45f,
        16,
        Color{45, 58, 50, 255}
    );

    DrawRectangleRounded(
        Rectangle{(float)barX, (float)barY, barW * progress, (float)barH},
        0.45f,
        16,
        Color{90, 220, 120, 255}
    );

    DrawText(
        TextFormat("%i%%", (int)(progress * 100.0f)),
        centerX - 18,
        centerY + 42,
        24,
        GREEN
    );

    EndDrawing();
}

inline void UnloadChunks(std::map<ChunkKey, Chunk>& chunks) {
    for (auto& item : chunks) {
        UnloadModel(item.second.terrainModel);
        UnloadModel(item.second.grassModel);
    }

    chunks.clear();
}

inline void RunGame() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Endless Meadow - GPU Grass C++ World");
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

    float windStrength = 0.65f;

    Camera3D camera = {0};
    camera.position = {0, 12, 20};
    camera.target = {0, 2, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 60;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 playerPos = {0, 0, 0};
    Vector3 velocity = {0, 0, 0};

    float playerYaw = 180.0f;
    bool isMoving = false;

    float cameraYaw = 0.0f;
    float cameraPitch = 18.0f;
    float cameraDistance = 22.0f;

    bool grounded = false;
    bool pauseMenu = false;

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

            DrawLoadingScreen(loadedInitialChunks, totalInitialChunks);

            chunks[key] = GenerateChunk(key.x, key.z, grassShader);

            loadedInitialChunks++;
        }
    }

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float time = GetTime();

        SetShaderValue(grassShader, grassTimeLoc, &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(grassShader, windStrengthLoc, &windStrength, SHADER_UNIFORM_FLOAT);

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

        float yawRad = cameraYaw * DEG2RAD;

        Vector3 forward = {-sinf(yawRad), 0, -cosf(yawRad)};
        Vector3 right = {cosf(yawRad), 0, -sinf(yawRad)};

        Vector3 input = {0, 0, 0};

        if (!pauseMenu) {
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

            playerPos.x += input.x * currentSpeed * dt;
            playerPos.z += input.z * currentSpeed * dt;

            playerYaw = atan2f(input.x, input.z) * RAD2DEG;
            isMoving = true;
        }

        float terrainY = GetTerrainHeight(playerPos.x, playerPos.z);

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

            if (playerPos.y <= terrainY) {
                playerPos.y = terrainY;
                velocity.y = 0;
                grounded = true;
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
        ClearBackground(Color{155, 205, 255, 255});

        BeginMode3D(camera);

        Vector3 sunPos = {260, 170, 130};
        DrawSphere(sunPos, 26.0f, Color{255, 245, 170, 255});
        DrawSphere(sunPos, 38.0f, Fade(Color{255, 220, 90, 255}, 0.18f));
        DrawSphere(sunPos, 58.0f, Fade(Color{255, 200, 80, 255}, 0.08f));

        for (const auto& item : chunks) {
            const Chunk& c = item.second;

            float chunkCenterX = c.key.x * chunkWorldSize + chunkWorldSize * 0.5f;
            float chunkCenterZ = c.key.z * chunkWorldSize + chunkWorldSize * 0.5f;
            float dist = Vector2Distance({playerPos.x, playerPos.z}, {chunkCenterX, chunkCenterZ});

            DrawModel(c.terrainModel, {0, 0, 0}, 1.0f, WHITE);

            if (dist < 260.0f) {
                DrawModel(c.grassModel, {0, 0, 0}, 1.0f, WHITE);
            }
        }

        DrawWaterAroundPlayer(playerPos, time);

        DrawNature(chunks, playerPos, time);
        DrawCuteAlien(playerPos, playerYaw, isMoving, time);

        EndMode3D();

        DrawRectangle(16, 16, 520, 220, Fade(BLACK, 0.48f));
        DrawText("Endless Meadow - GPU Grass Prototype", 28, 28, 20, WHITE);
        DrawText("WASD = move | SHIFT = sprint", 28, 60, 18, RAYWHITE);
        DrawText("SPACE = jump | Double SPACE = fly mode", 28, 86, 18, RAYWHITE);
        DrawText("Fly mode: SPACE up | CTRL down", 28, 112, 18, RAYWHITE);
        DrawText("Mouse = camera | ESC = unlock mouse", 28, 138, 18, RAYWHITE);
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
    UnloadShader(grassShader);
    CloseWindow();
}