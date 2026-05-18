#pragma once

#include "raylib.h"
#include "world_types.hpp"

extern const char* waterVertexShader;
extern const char* waterFragmentShader;

void DrawWaterAroundPlayer(Model waterModel, Vector3 playerPos);