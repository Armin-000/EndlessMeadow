#pragma once

#include "raylib.h"

void DrawLoadingScreen(
    Texture2D preloadTexture,
    int loadedInitialChunks,
    int totalInitialChunks
);