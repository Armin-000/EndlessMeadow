
# AI_CONTEXT.md

# Endless Meadow — AI Development Context

Endless Meadow is a cinematic procedural meadow prototype and experimental mini-engine built in modern C++ using raylib and OpenGL.

The project focuses on:

- procedural terrain generation
- real-time chunk streaming
- stylized vegetation
- animated alien character
- live terrain minimap
- backpack inventory system
- basic resource gathering
- cinematic exploration
- modular rendering systems

---

# Current Project Structure

```txt
include/
├── game.hpp
├── world.hpp
├── world_types.hpp
├── terrain.hpp
├── vegetation.hpp
├── water.hpp
├── sky.hpp
├── alien.hpp
├── collisions.hpp
├── map.hpp
└── inventory.hpp

src/
├── main.cpp
├── world.cpp
├── terrain.cpp
├── vegetation.cpp
├── water.cpp
├── sky.cpp
├── alien.cpp
├── collisions.cpp
├── map.cpp
└── inventory.cpp
```

---

# Main Entry Point

`src/main.cpp` is intentionally minimal:

```cpp
#include "game.hpp"

int main()
{
    RunGame();
    return 0;
}
```

The main gameplay loop currently lives in `include/game.hpp` inside `RunGame()`.

---

# Core Architecture

## `world.hpp`

`world.hpp` is now only a central include file.

It includes:

- `world_types.hpp`
- `terrain.hpp`
- `vegetation.hpp`
- `water.hpp`
- `sky.hpp`
- `alien.hpp`
- `collisions.hpp`

Avoid putting large implementations back into `world.hpp`.

---

## `world_types.hpp`

Contains shared world constants and structs:

- `CHUNK_SIZE`
- `CHUNK_RESOLUTION`
- `VIEW_DISTANCE`
- `WORLD_SCALE`
- `GRASS_PER_CHUNK`
- `FLOWERS_PER_CHUNK`
- `WATER_LEVEL`
- `ChunkKey`
- `Tree`
- `Rock`
- `Flower`
- `Chunk`

Current `Tree` structure includes gameplay state:

```cpp
struct Tree {
    Vector3 position;
    float size;
    float sway;
    int hitCount = 0;
    float shakeTimer = 0.0f;
};
```

`hitCount` is used for tree damage.  
`shakeTimer` is used for tree shake animation after being hit.

---

# Engine Systems

## Terrain System

Files:

- `terrain.hpp`
- `terrain.cpp`

Responsibilities:

- procedural terrain generation
- fractal noise
- smooth noise
- ridge noise
- terrain height calculation
- terrain color calculation
- terrain mesh generation
- chunk generation
- terrain normal recalculation

Important functions:

- `HashNoise()`
- `SmoothNoise()`
- `FractalNoise()`
- `RidgeNoise()`
- `GetTerrainHeight()`
- `GetTerrainColor()`
- `RecalculateTerrainNormals()`
- `GenerateChunk()`

`GenerateChunk()` currently generates:

- terrain mesh
- grass mesh
- flower mesh
- procedural trees
- procedural rocks

---

## Vegetation System

Files:

- `vegetation.hpp`
- `vegetation.cpp`

Responsibilities:

- procedural grass generation
- procedural flower generation
- tree rendering
- rock rendering
- vegetation draw distance
- grass shader source
- tree shake animation

Important functions:

- `GenerateGrassMesh()`
- `GenerateFlowerMesh()`
- `DrawNature()`
- `DrawFlowerModels()`
- `RandomGrassColor()`
- `RandomFlowerColor()`

Grass is generated as GPU mesh per chunk.

Trees are currently drawn procedurally using raylib primitives.

Tree shake is controlled by:

```cpp
tree.shakeTimer
```

When the alien hits a tree, `shakeTimer` is set to `0.25f`.

---

## Water System

Files:

- `water.hpp`
- `water.cpp`

Responsibilities:

- water shader source
- animated water rendering
- water wave animation
- water rendered around player

Important functions:

- `DrawWaterAroundPlayer()`

Important shader globals:

- `waterVertexShader`
- `waterFragmentShader`

Water level comes from:

```cpp
WATER_LEVEL
```

inside `world_types.hpp`.

---

## Sky System

Files:

- `sky.hpp`
- `sky.cpp`

Responsibilities:

- procedural cloud rendering
- cloud movement
- atmospheric background support

Important function:

- `DrawClouds(float time, Vector3 playerPos)`

---

## Alien System

Files:

- `alien.hpp`
- `alien.cpp`

Responsibilities:

- procedural alien rendering
- walking animation
- sprinting animation
- falling animation
- antenna rendering
- backpack rendering
- right-arm attack animation

Current function:

```cpp
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
```

The alien now has a cute brown backpack drawn on its back.

Backpack is rendered using `rlPushMatrix()`, `rlTranslatef()`, and `rlRotatef()` so it rotates with the alien.

Attack animation:

- controlled by `attackTimer`
- right arm moves forward during a punch/hit
- used when left mouse button hits a tree

---

## Collision System

Files:

- `collisions.hpp`
- `collisions.cpp`

Responsibilities:

- tree collision
- rock collision
- player collision against nature
- smart ground detection
- rock top detection

Important functions:

- `IsInsideCylinderXZ()`
- `CheckTreeCollision()`
- `GetRockTopYAtPoint()`
- `CheckRockSideCollision()`
- `CheckNatureCollision()`
- `GetSmartGroundY()`

---

## Map System

Files:

- `map.hpp`
- `map.cpp`

Responsibilities:

- real-time terrain map overlay
- terrain sampling around player
- showing water, grass, hills, rocks, mountains, snow/high terrain
- player marker in center

Important function:

```cpp
void DrawWorldMapOverlay(
    const std::map<ChunkKey, Chunk>& chunks,
    Vector3 playerPos,
    bool visible
);
```

Map is toggled with:

```txt
M
```

The map uses `GetTerrainHeight()` to sample actual world terrain in real time.

Terrain map colors:

- blue = water
- green = meadow/grass
- darker green = hills
- brown/gray = rocks/mountains
- light color = high peaks/snow

---

## Inventory System

Files:

- `inventory.hpp`
- `inventory.cpp`

Responsibilities:

- backpack UI overlay
- 10-slot inventory
- mouse-enabled item organization
- click item and move to empty slot
- display collected wood

Current function:

```cpp
void DrawInventoryOverlay(
    bool visible,
    int inventorySlots[10],
    int& selectedSlot
);
```

Inventory is toggled with:

```txt
E
```

When inventory opens:

- cursor is enabled
- player movement is disabled
- inventory slots are clickable

When inventory closes:

- cursor is disabled again
- selected slot should be reset with `selectedSlot = -1`

Current slot logic:

```cpp
int inventorySlots[10] = {0};
int selectedSlot = -1;
```

Current item system:

- `0` means empty slot
- positive number means wood count in that slot

Current supported item:

- Wood

---

# Gameplay Systems

## Player Movement

Controls:

```txt
W A S D      = move
SHIFT        = sprint
SPACE        = jump
Double SPACE = toggle fly mode
CTRL         = descend in fly mode
Mouse        = camera
ESC          = pause
M            = map
E            = backpack/inventory
Left Mouse   = hit tree
```

Movement is disabled when:

- pause menu is open
- inventory is open

---

## Tree Hitting / Wood Gathering

The alien can hit trees with left mouse button.

Logic:

- left mouse checks nearest tree in front of alien
- tree must be within hit range
- tree must be generally in front of player using dot product
- every valid hit increments `tree.hitCount`
- every hit triggers `tree.shakeTimer`
- alien attack animation starts via `attackTimer`
- after 3 hits, tree disappears
- boom cloud effect appears
- wood is added to inventory

Tree hit values:

```cpp
const float hitRange = 4.2f;
const float minDot = 0.45f;
```

Tree break threshold:

```cpp
tree.hitCount >= 3
```

Wood reward:

```cpp
+3 wood
```

Inventory add logic:

- first tries to add wood to an existing wood stack
- if no stack exists, adds wood to slot 0

Current code logic:

```cpp
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
```

---

## Attack Animation

State variables in `RunGame()`:

```cpp
float attackTimer = 0.0f;
float boomTimer = 0.0f;
Vector3 boomPos = {0, 0, 0};
```

Every frame:

```cpp
if (attackTimer > 0.0f) attackTimer -= dt;
if (boomTimer > 0.0f) boomTimer -= dt;
```

Attack starts when tree is hit:

```cpp
attackTimer = 0.22f;
```

Alien arm animation is handled in `alien.cpp`.

---

## Tree Shake Animation

Every frame, tree shake timer is reduced:

```cpp
for (auto& item : chunks) {
    for (Tree& tree : item.second.trees) {
        if (tree.shakeTimer > 0.0f) {
            tree.shakeTimer -= dt;
        }
    }
}
```

In `vegetation.cpp`, tree rendering should add extra wind/shake offset when:

```cpp
t.shakeTimer > 0.0f
```

---

## Boom Effect

When a tree breaks:

```cpp
boomPos = tree.position;
boomPos.y += 1.6f;
boomTimer = 0.45f;
```

During rendering, if `boomTimer > 0.0f`, draw a few fading spheres as a small cartoon cloud.

This effect is drawn before `EndMode3D()`.

---

# Rendering Order

Current 3D rendering order:

1. clear background
2. `BeginMode3D(camera)`
3. clouds
4. sun glow
5. terrain chunks
6. grass chunks
7. water
8. flowers
9. nature objects
10. alien
11. boom effect
12. `EndMode3D()`
13. map overlay
14. inventory overlay
15. HUD
16. pause menu

---

# UI Systems

## HUD

HUD currently shows:

- project title
- movement controls
- map/inventory controls
- chunk count
- grass per chunk
- FPS
- fly/ground mode state

Recommended HUD line:

```txt
Mouse = camera | M = map | E = backpack | ESC = pause
```

---

## Pause Menu

ESC toggles pause.

When paused:

- cursor is enabled
- pause overlay is shown
- exit button is clickable

---

## Inventory UI

Inventory currently has:

- 10 slots
- 5 columns x 2 rows
- hover effect
- selected slot highlight
- click-to-move item
- wood display
- empty slot plus symbol

Inventory should remain simple and clean.

---

# Build System

Uses:

- CMake
- C++17
- raylib
- macOS bundle support

Current `CMakeLists.txt` should include:

```cmake
add_executable(EndlessMeadow
    MACOSX_BUNDLE

    src/main.cpp
    src/world.cpp
    src/alien.cpp
    src/sky.cpp
    src/water.cpp
    src/terrain.cpp
    src/vegetation.cpp
    src/collisions.cpp
    src/map.cpp
    src/inventory.cpp

    assets/icons/meadow.icns
)
```

Build:

```bash
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run on macOS:

```bash
open build/EndlessMeadow.app
```

---

# Current Implemented Features

Implemented:

- procedural infinite terrain
- chunk streaming
- procedural grass
- procedural flowers
- procedural trees
- procedural rocks
- animated water shader
- procedural cloud system
- animated alien
- alien backpack
- live terrain minimap
- 10-slot inventory
- inventory item movement
- tree hitting
- tree shake on hit
- tree breaking after 3 hits
- boom cloud effect
- wood collection
- wood displayed in inventory
- fly mode
- pause menu
- loading screen
- macOS app bundle

---

# Current Technical Notes

## Current Architecture State

The project is partially modular.

Most engine systems are now split into separate `.hpp/.cpp` modules.

However, `RunGame()` and the main game loop are still inside:

```txt
include/game.hpp
```

Future improvement:

```txt
include/game.hpp  -> declarations only
src/game.cpp      -> RunGame implementation
```

This would make the architecture cleaner and reduce compile times.

---

## Current Inventory Limitation

Inventory currently stores only wood counts as integers.

Future better item model:

```cpp
enum class ItemType {
    None,
    Wood,
    Stone,
    Flower,
    Mushroom
};

struct InventorySlot {
    ItemType type;
    int count;
};
```

This would allow multiple item types.

---

## Current Tree Limitation

Trees are stored directly inside chunks:

```cpp
std::vector<Tree> trees;
```

When a tree is destroyed, it is erased from the chunk vector.

This works, but future improvements could include:

- dropped item entities
- tree stump
- regrowth timer
- falling tree animation
- wood pieces on ground
- axe/tool requirement

---

# Recommended Future Improvements

High-value next systems:

1. Move `RunGame()` implementation from `game.hpp` into `game.cpp`
2. Create real `InventorySlot` type
3. Add item pickup entities on the ground
4. Add wood pieces falling after tree breaks
5. Add axe/tool item
6. Add crafting system
7. Add hotbar
8. Add day/night cycle
9. Add dynamic weather
10. Add biome system
11. Add wildlife
12. Add sound effects
13. Add particle system
14. Add save/load system

---

# AI Modification Rules

When modifying this project:

- preserve modular architecture
- do not put large implementation code back into `world.hpp`
- avoid making `world.hpp` monolithic again
- keep new systems in separate `.hpp/.cpp` files
- keep raylib procedural style
- avoid unnecessary heavy OOP
- avoid ECS conversion unless explicitly requested
- preserve clean visual style
- preserve cinematic feeling
- preserve procedural generation style
- keep code copy-paste ready
- when changing function signatures, update both `.hpp` and `.cpp`
- when adding `.cpp` files, update `CMakeLists.txt`
- when adding shared gameplay state, document it here

---

# Debugging Notes

Common build issues:

## Function signature mismatch

If `.hpp` says:

```cpp
void DrawInventoryOverlay(bool visible, int inventorySlots[10], int& selectedSlot);
```

then `.cpp` and call sites must match exactly.

## Missing struct field

If error says:

```txt
no member named 'shakeTimer' in 'Tree'
```

then update `world_types.hpp`.

## Missing source file in build

If linker errors appear for a new function, check `CMakeLists.txt`.

## App executable missing

If macOS says app executable is missing, build failed earlier. Fix compile errors first, then run:

```bash
cmake --build build
open build/EndlessMeadow.app
```

---

# Engine Philosophy

Endless Meadow should remain:

- lightweight
- procedural
- cinematic
- atmospheric
- playful
- stylized
- readable
- modular

The project should feel like a mix of:

- procedural art project
- graphics programming playground
- cozy survival prototype
- mini-engine architecture showcase

The best future direction is to gradually evolve it into a small cinematic procedural survival sandbox.