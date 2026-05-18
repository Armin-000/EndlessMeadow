#include "../include/alien.hpp"

#include "raymath.h"

#include "rlgl.h"

#include <cmath>

void DrawCuteAlien(
    Vector3 pos,
    float yawDeg,
    bool isMoving,
    bool isSprinting,
    float verticalVelocity,
    bool grounded,
    float time,
    float attackTimer
) {
    float yaw = yawDeg * DEG2RAD;

    Vector3 forward = {sinf(yaw), 0, cosf(yaw)};
    Vector3 right = {cosf(yaw), 0, -sinf(yaw)};

    bool falling = !grounded && verticalVelocity < -4.0f;

    float runSpeed = 2.0f;

    if (isMoving && !isSprinting) runSpeed = 10.0f;
    if (isMoving && isSprinting) runSpeed = 17.0f;

    float runCycle = sinf(time * runSpeed);
    float runCycleOpposite = sinf(time * runSpeed + PI);

    float walk = isMoving ? runCycle : 0.0f;

    float bodyBob = isMoving
        ? fabsf(runCycle) * (isSprinting ? 0.14f : 0.08f)
        : sinf(time * 2.0f) * 0.015f;

    float bodyLean = isMoving
        ? (isSprinting ? 0.18f : 0.09f)
        : 0.0f;

    if (falling) {
        bodyBob = sinf(time * 8.0f) * 0.035f;
        bodyLean = -0.05f;
    }

    float footY = pos.y + 0.02f;

    Color bodyColor = Color{78, 185, 125, 255};
    Color headColor = Color{110, 220, 155, 255};
    Color darkDetail = Color{32, 58, 45, 255};

    Vector3 body = {pos.x, footY + 0.82f + bodyBob, pos.z};
    Vector3 head = {pos.x, footY + 1.56f + bodyBob, pos.z};

    body = Vector3Add(body, Vector3Scale(forward, bodyLean));
    head = Vector3Add(head, Vector3Scale(forward, bodyLean * 1.4f));

    DrawSphere(body, 0.42f, bodyColor);
    DrawSphere(head, 0.56f, headColor);

    // Cute brown backpack - fixed and rotated with alien
    Vector3 backpackCenter = Vector3Subtract(
        body,
        Vector3Scale(forward, 0.38f)
    );

    backpackCenter.y += 0.02f;

    Color backpackColor = Color{120, 72, 38, 255};
    Color backpackDark = Color{78, 45, 25, 255};
    Color backpackLight = Color{165, 105, 60, 255};

    rlPushMatrix();

        rlTranslatef(backpackCenter.x, backpackCenter.y, backpackCenter.z);
        rlRotatef(yawDeg, 0.0f, 1.0f, 0.0f);

        // Main backpack body
        DrawCube(
            {0.0f, 0.0f, 0.0f},
            0.46f,
            0.58f,
            0.24f,
            backpackColor
        );

        DrawCubeWires(
            {0.0f, 0.0f, 0.0f},
            0.47f,
            0.59f,
            0.25f,
            backpackDark
        );

        // Outside pocket
        DrawCube(
            {0.0f, -0.08f, -0.145f},
            0.30f,
            0.22f,
            0.045f,
            backpackLight
        );

        // Top flap
        DrawCube(
            {0.0f, 0.18f, -0.15f},
            0.34f,
            0.12f,
            0.05f,
            backpackDark
        );

        // Left strap
        DrawCube(
            {-0.17f, 0.02f, 0.145f},
            0.045f,
            0.50f,
            0.045f,
            backpackDark
        );

        // Right strap
        DrawCube(
            {0.17f, 0.02f, 0.145f},
            0.045f,
            0.50f,
            0.045f,
            backpackDark
        );

    rlPopMatrix();

    Vector3 faceCenter = Vector3Add(head, Vector3Scale(forward, 0.48f));

    Vector3 eyeL = Vector3Add(faceCenter, Vector3Scale(right, -0.17f));
    Vector3 eyeR = Vector3Add(faceCenter, Vector3Scale(right, 0.17f));

    DrawSphere(eyeL, 0.12f, BLACK);
    DrawSphere(eyeR, 0.12f, BLACK);

    DrawSphere(
        Vector3Add(eyeL, {0.03f, 0.03f, 0.05f}),
        0.03f,
        WHITE
    );

    DrawSphere(
        Vector3Add(eyeR, {0.03f, 0.03f, 0.05f}),
        0.03f,
        WHITE
    );

    Vector3 antLBase = Vector3Add(head, Vector3Scale(right, -0.22f));
    antLBase.y += 0.36f;

    Vector3 antRBase = Vector3Add(head, Vector3Scale(right, 0.22f));
    antRBase.y += 0.36f;

    Vector3 antLTop = Vector3Add(
        antLBase,
        Vector3Add(Vector3Scale(right, -0.10f), {0, 0.42f, 0})
    );

    Vector3 antRTop = Vector3Add(
        antRBase,
        Vector3Add(Vector3Scale(right, 0.10f), {0, 0.42f, 0})
    );

    DrawLine3D(antLBase, antLTop, darkDetail);
    DrawLine3D(antRBase, antRTop, darkDetail);

    DrawSphere(antLTop, 0.08f, Color{255, 220, 110, 255});
    DrawSphere(antRTop, 0.08f, Color{255, 220, 110, 255});

    float legSwing = walk * (isSprinting ? 0.52f : 0.34f);

    if (falling) {
        legSwing = 0.12f * sinf(time * 5.0f);
    }

    Vector3 hipL = Vector3Add(body, Vector3Scale(right, -0.16f));
    Vector3 hipR = Vector3Add(body, Vector3Scale(right, 0.16f));

    hipL.y -= 0.30f;
    hipR.y -= 0.30f;

    Vector3 footBase = {pos.x, footY, pos.z};

    Vector3 footL = Vector3Add(footBase, Vector3Scale(right, -0.16f));
    Vector3 footR = Vector3Add(footBase, Vector3Scale(right, 0.16f));

    footL = Vector3Add(footL, Vector3Scale(forward, legSwing));
    footR = Vector3Add(footR, Vector3Scale(forward, -legSwing));

    DrawLine3D(hipL, footL, darkDetail);
    DrawLine3D(hipR, footR, darkDetail);

    DrawSphere(footL, 0.10f, Color{48, 110, 75, 255});
    DrawSphere(footR, 0.10f, Color{48, 110, 75, 255});

    Vector3 armL1 = Vector3Add(body, Vector3Scale(right, -0.42f));
    Vector3 armR1 = Vector3Add(body, Vector3Scale(right, 0.42f));

    float armSwing = isMoving
        ? runCycleOpposite * (isSprinting ? 0.58f : 0.34f)
        : sinf(time * 2.0f) * 0.04f;

    float attackPunch = 0.0f;

    if (attackTimer > 0.0f) {
        attackPunch = sinf((attackTimer / 0.22f) * PI) * 0.75f;
    }

    Vector3 armL2;
    Vector3 armR2;

    if (falling) {
        armL2 = Vector3Add(
            armL1,
            Vector3Add(Vector3Scale(right, -0.10f), {0, 0.42f, 0})
        );

        armR2 = Vector3Add(
            armR1,
            Vector3Add(Vector3Scale(right, 0.10f), {0, 0.42f, 0})
        );
    } else {
        armL2 = Vector3Add(
            armL1,
            Vector3Add(Vector3Scale(forward, armSwing), {0, -0.42f, 0})
        );

        armR2 = Vector3Add(
            armR1,
            Vector3Add(Vector3Scale(forward, -armSwing + attackPunch), {0, -0.42f, 0})
        );
    }

    DrawLine3D(armL1, armL2, darkDetail);
    DrawLine3D(armR1, armR2, darkDetail);

    DrawSphere(armL2, 0.08f, Color{58, 140, 90, 255});
    DrawSphere(armR2, 0.08f, Color{58, 140, 90, 255});
}