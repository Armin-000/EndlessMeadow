
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
- procedural resource gathering
- wood physics and item drops
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
├── inventory.hpp
└── item_drops.hpp

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
├── inventory.cpp
└── item_drops.cpp
````

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

The main gameplay loop currently lives in:

```txt
include/game.hpp
```

inside:

```cpp
RunGame()
```

---

# Core Architecture

## `world.hpp`

`world.hpp` is now only a central include file.

It includes:

* `world_types.hpp`
* `terrain.hpp`
* `vegetation.hpp`
* `water.hpp`
* `sky.hpp`
* `alien.hpp`
* `collisions.hpp`

Avoid putting large implementations back into `world.hpp`.

---

## `world_types.hpp`

Contains shared world constants and structs:

* `CHUNK_SIZE`
* `CHUNK_RESOLUTION`
* `VIEW_DISTANCE`
* `WORLD_SCALE`
* `GRASS_PER_CHUNK`
* `FLOWERS_PER_CHUNK`
* `WATER_LEVEL`
* `ChunkKey`
* `Tree`
* `Rock`
* `Flower`
* `Chunk`

Current `Tree` structure:

```cpp
struct Tree {
    Vector3 position;
    float size;
    float sway;
    int hitCount = 0;
    float shakeTimer = 0.0f;
};
```

Gameplay state:

* `hitCount` = tree damage
* `shakeTimer` = tree shake animation

---

# Engine Systems

## Terrain System

Files:

* `terrain.hpp`
* `terrain.cpp`

Responsibilities:

* procedural terrain generation
* fractal noise
* ridge noise
* terrain mesh generation
* chunk generation
* terrain color calculation
* terrain normal recalculation

Important functions:

```cpp
HashNoise()
SmoothNoise()
FractalNoise()
RidgeNoise()
GetTerrainHeight()
GetTerrainColor()
RecalculateTerrainNormals()
GenerateChunk()
```

`GenerateChunk()` generates:

* terrain mesh
* grass mesh
* flower mesh
* trees
* rocks

---

## Vegetation System

Files:

* `vegetation.hpp`
* `vegetation.cpp`

Responsibilities:

* grass generation
* flower generation
* tree rendering
* rock rendering
* tree shaking
* vegetation draw distance

Important functions:

```cpp
GenerateGrassMesh()
GenerateFlowerMesh()
DrawNature()
DrawFlowerModels()
```

Grass is GPU-generated per chunk.

Trees are procedurally rendered using raylib primitives.

Tree shake uses:

```cpp
tree.shakeTimer
```

---

## Water System

Files:

* `water.hpp`
* `water.cpp`

Responsibilities:

* animated water shader
* water rendering
* wave movement
* rendering around player

Important function:

```cpp
DrawWaterAroundPlayer()
```

Important shader globals:

```cpp
waterVertexShader
waterFragmentShader
```

---

## Sky System

Files:

* `sky.hpp`
* `sky.cpp`

Responsibilities:

* cloud rendering
* atmospheric background
* cloud movement

Important function:

```cpp
DrawClouds(float time, Vector3 playerPos)
```

---

## Alien System

Files:

* `alien.hpp`
* `alien.cpp`

Responsibilities:

* procedural alien rendering
* walking animation
* sprinting animation
* falling animation
* antenna animation
* backpack rendering
* attack animation

Main function:

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

---

# Alien Features

## Reactive Antenna System

Alien antennas now use dynamic physics-like motion.

Current antenna behavior includes:

* idle sway
* movement bounce
* sprint bending
* falling whip animation
* organic motion
* speed-based bending

Important variables:

```cpp
speedBend
antennaBounce
sprintVibration
fallingWhip
```

Antennas react to:

* movement speed
* sprinting
* jumping
* falling velocity
* movement state

Sprint causes antennas to bend backward heavily.

Falling causes antennas to whip dynamically.

---

## Backpack System

Alien backpack was upgraded into a cinematic procedural adventure backpack.

Features:

* rounded backpack body
* front pocket
* top flap
* straps
* side pouches
* buckle details

Backpack rotates with alien using:

```cpp
rlPushMatrix()
rlTranslatef()
rlRotatef()
```

---

## Collision System

Files:

* `collisions.hpp`
* `collisions.cpp`

Responsibilities:

* tree collision
* rock collision
* smart ground detection
* nature collision

Important functions:

```cpp
CheckNatureCollision()
GetSmartGroundY()
GetRockTopYAtPoint()
```

---

## Map System

Files:

* `map.hpp`
* `map.cpp`

Responsibilities:

* live terrain minimap
* terrain sampling
* water visualization
* hill/mountain visualization
* player tracking

Important function:

```cpp
DrawWorldMapOverlay()
```

Map toggle:

```txt
M
```

---

## Inventory System

Files:

* `inventory.hpp`
* `inventory.cpp`

Responsibilities:

* backpack overlay
* inventory UI
* slot movement
* click item organization
* displaying wood

Current function:

```cpp
DrawInventoryOverlay(
    bool visible,
    int inventorySlots[10],
    int& selectedSlot
);
```

Inventory toggle:

```txt
E
```

Current inventory state:

```cpp
int inventorySlots[10] = {0};
int selectedSlot = -1;
```

Current item system:

* `0` = empty
* positive value = wood count

---

## Item Drop System

Files:

* `item_drops.hpp`
* `item_drops.cpp`

Responsibilities:

* dropped wood entities
* terrain-aware physics
* gravity simulation
* bounce simulation
* magnetic pickup animation
* spinning pickup animation
* inventory collection

Current structure:

```cpp
struct WoodDrop {
    Vector3 position;
    Vector3 velocity;
    float radius;
    float life;
    bool picked;
    bool magnetized;
    float magnetTimer;
};
```

Important functions:

```cpp
SpawnWoodDrops()
UpdateWoodDrops()
DrawWoodDrops()
```

---

# Wood Drop Features

Implemented:

* procedural wood chunks
* terrain collision
* gravity
* bounce physics
* spinning logs
* magnetic pickup
* inventory collection
* pickup popup text

Wood chunks are rendered as procedural logs using:

```cpp
DrawCylinder()
DrawCylinderWires()
```

Pickup system:

* wood becomes magnetized near player
* wood flies toward alien
* spin speed increases
* item disappears into inventory

Pickup popup:

```txt
+1 Wood
```

---

# Gameplay Systems

## Player Movement

Controls:

```txt
W A S D      = move
SHIFT        = sprint
SPACE        = jump
Double SPACE = fly mode
CTRL         = descend
Mouse        = camera
ESC          = pause
M            = map
E            = inventory
Left Mouse   = hit tree
```

Movement is disabled during:

* pause menu
* inventory

---

## Tree Hitting / Wood Gathering

Alien can hit trees using left mouse button.

Logic:

* nearest valid tree detection
* hit range check
* front-facing dot product check
* hit counter
* tree shake
* attack animation

Tree hit values:

```cpp
const float hitRange = 4.2f;
const float minDot = 0.45f;
```

Tree break condition:

```cpp
tree.hitCount >= 3
```

Tree destruction now spawns:

```cpp
SpawnWoodDrops(woodDrops, tree.position, 3);
```

Tree destruction includes:

* boom cloud effect
* procedural wood chunks
* terrain bounce physics
* magnetic pickup system

---

## Attack Animation

State variables:

```cpp
float attackTimer = 0.0f;
float boomTimer = 0.0f;
Vector3 boomPos = {0, 0, 0};
```

Attack triggers:

```cpp
attackTimer = 0.22f;
```

Alien arm animation is handled in:

```txt
alien.cpp
```

---

## Boom Effect

When tree breaks:

```cpp
boomPos = tree.position;
boomPos.y += 1.6f;
boomTimer = 0.45f;
```

Small cartoon smoke cloud is rendered before:

```cpp
EndMode3D()
```

---

# Rendering Order

Current render order:

1. clear background
2. `BeginMode3D(camera)`
3. clouds
4. sun glow
5. terrain
6. grass
7. water
8. flowers
9. nature objects
10. wood drops
11. alien
12. boom effect
13. `EndMode3D()`
14. map overlay
15. inventory overlay
16. HUD
17. pause menu

---

# UI Systems

## HUD

HUD currently shows:

* controls
* chunk count
* grass count
* FPS
* fly mode state

---

## Pickup Popup

When collecting wood:

```txt
+1 Wood
```

Popup appears above alien.

---

## Pause Menu

ESC toggles pause.

When paused:

* cursor enabled
* overlay shown
* exit button clickable

---

## Inventory UI

Features:

* 10 slots
* 5x2 layout
* hover effect
* selected slot highlight
* click-to-move item system
* wood display

---

# Build System

Uses:

* CMake
* C++17
* raylib
* macOS bundle support

Current `CMakeLists.txt` includes:

```cmake
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
src/item_drops.cpp
```

Build:

```bash
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run:

```bash
open build/EndlessMeadow.app
```

---

# Current Implemented Features

Implemented:

* infinite procedural terrain
* chunk streaming
* procedural grass
* procedural flowers
* procedural trees
* procedural rocks
* animated water
* procedural clouds
* animated alien
* reactive alien antennas
* cinematic alien backpack
* terrain minimap
* 10-slot inventory
* inventory organization
* tree hitting
* tree shaking
* tree destruction
* procedural wood drops
* terrain-aware wood physics
* magnetic pickup animation
* spinning pickup animation
* pickup popup text
* boom cloud effect
* fly mode
* loading screen
* pause menu
* macOS app bundle

---

# Current Architecture State

Project is partially modular.

Main systems are separated into `.hpp/.cpp` modules.

Main gameplay loop still exists in:

```txt
include/game.hpp
```

Future improvement:

```txt
include/game.hpp -> declarations only
src/game.cpp -> implementation
```

---

# Current Inventory Limitation

Inventory still uses integer-only wood storage.

Future better model:

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

---

# Current Tree System

Trees currently support:

* damage
* shaking
* destruction
* procedural wood drops
* terrain bounce physics
* magnetic pickup
* inventory collection

Trees are still stored directly inside chunk vectors:

```cpp
std::vector<Tree> trees;
```

---

# Recommended Future Improvements

High-value next systems:

1. Move `RunGame()` into `game.cpp`
2. Create real `InventorySlot`
3. Add pickup sound effects
4. Add falling tree animation
5. Add tree stump system
6. Add axe/tool item
7. Add crafting system
8. Add hotbar
9. Add day/night cycle
10. Add dynamic weather
11. Add biome system
12. Add wildlife
13. Add particle system
14. Add save/load system

---

# AI Modification Rules

When modifying project:

* preserve modular architecture
* avoid giant `world.hpp`
* use separate `.hpp/.cpp` systems
* preserve procedural style
* preserve cinematic atmosphere
* avoid unnecessary ECS conversion
* keep systems lightweight
* update `CMakeLists.txt` when adding files
* keep code copy-paste ready
* update both `.hpp` and `.cpp` when changing signatures

---

# Debugging Notes

## Linker Errors

If linker errors appear:

* check `CMakeLists.txt`
* verify new `.cpp` file is added

## Signature Mismatch

If function mismatch appears:

* verify `.hpp`
* verify `.cpp`
* verify call site

## Missing Struct Fields

If missing member errors appear:

* update `world_types.hpp`

## App Missing

If macOS app missing:

* build failed earlier
* fix compile errors first

---

# Engine Philosophy

Endless Meadow should remain:

* lightweight
* procedural
* cinematic
* atmospheric
* playful
* stylized
* modular
* readable

The project should feel like a mix of:

* procedural art project
* graphics playground
* cozy survival prototype
* mini-engine showcase

Long-term direction:

A cinematic procedural survival sandbox with stylized graphics and modular engine architecture.