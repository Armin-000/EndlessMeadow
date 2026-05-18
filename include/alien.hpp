#pragma once

#include "raylib.h"

void DrawCuteAlien(
    Vector3 pos,
    float yawDeg,
    bool isMoving,
    bool isSprinting,
    float verticalVelocity,
    bool grounded,
    float time,
    float attackTimer
);