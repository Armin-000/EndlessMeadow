#pragma once

#include "raylib.h"

void DrawLoadingScreen(Texture2D preloadTexture, int loadedInitialChunks, int totalInitialChunks);

void DrawStartMenuScreen(
    Texture2D preloadTexture,
    bool& waitingForStart,
    bool& shouldExit
);