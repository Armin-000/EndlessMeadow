#include "../include/inventory.hpp"

#include "raylib.h"

void DrawInventoryOverlay(
    bool visible,
    int inventorySlots[10],
    int& selectedSlot
)
{
    if (!visible) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float panelW = 560.0f;
    float panelH = 390.0f;
    float panelX = screenW / 2.0f - panelW / 2.0f;
    float panelY = screenH / 2.0f - panelH / 2.0f;

    DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.45f));

    DrawRectangleRounded(
        Rectangle{panelX, panelY, panelW, panelH},
        0.10f,
        22,
        Color{38, 26, 18, 245}
    );

    DrawRectangleRoundedLines(
        Rectangle{panelX, panelY, panelW, panelH},
        0.10f,
        22,
        Color{185, 120, 70, 255}
    );

    DrawText("ALIEN BACKPACK", (int)panelX + 30, (int)panelY + 24, 28, RAYWHITE);
    DrawText("Click item, then click empty slot to move it", (int)panelX + 32, (int)panelY + 58, 16, Fade(RAYWHITE, 0.70f));

    const int cols = 5;
    const int rows = 2;

    const float slotSize = 76.0f;
    const float gap = 18.0f;

    const float gridW = cols * slotSize + (cols - 1) * gap;
    const float startX = panelX + panelW / 2.0f - gridW / 2.0f;
    const float startY = panelY + 120.0f;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < cols * rows; i++) {
        int col = i % cols;
        int row = i / cols;

        float x = startX + col * (slotSize + gap);
        float y = startY + row * (slotSize + gap);

        Rectangle slot = {x, y, slotSize, slotSize};
        bool hover = CheckCollisionPointRec(mouse, slot);

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (selectedSlot == -1) {
                if (inventorySlots[i] > 0) {
                    selectedSlot = i;
                }
            } else {
                if (inventorySlots[i] == 0) {
                    inventorySlots[i] = inventorySlots[selectedSlot];
                    inventorySlots[selectedSlot] = 0;
                    selectedSlot = -1;
                } else if (i == selectedSlot) {
                    selectedSlot = -1;
                }
            }
        }

        DrawRectangleRounded(
            slot,
            0.14f,
            14,
            hover ? Color{82, 55, 35, 255} : Color{54, 37, 25, 255}
        );

        DrawRectangleRoundedLines(
            slot,
            0.14f,
            14,
            hover ? Color{245, 185, 105, 255} : Color{145, 95, 55, 210}
        );

        if (selectedSlot == i) {
            DrawRectangleRoundedLines(
                Rectangle{x - 4, y - 4, slotSize + 8, slotSize + 8},
                0.16f,
                14,
                Color{255, 230, 120, 255}
            );
        }

        DrawText(
            TextFormat("%02d", i + 1),
            (int)(x + 8),
            (int)(y + 7),
            12,
            Fade(RAYWHITE, 0.35f)
        );

        if (inventorySlots[i] > 0) {
            DrawRectangleRounded(
                Rectangle{x + 18, y + 18, slotSize - 36, slotSize - 36},
                0.18f,
                10,
                Color{130, 82, 42, 255}
            );

            const char* woodText = TextFormat("Wood x%i", inventorySlots[i]);

            DrawText(
                woodText,
                (int)(x + slotSize / 2 - MeasureText(woodText, 11) / 2),
                (int)(y + 54),
                11,
                RAYWHITE
            );
        } else {
            DrawText(
                "+",
                (int)(x + slotSize / 2 - MeasureText("+", 22) / 2),
                (int)(y + slotSize / 2 - 13),
                22,
                Fade(RAYWHITE, hover ? 0.55f : 0.25f)
            );
        }
    }

    DrawText(
        "Press E to close",
        (int)(panelX + panelW - 150),
        (int)(panelY + panelH - 36),
        16,
        Fade(RAYWHITE, 0.85f)
    );
}