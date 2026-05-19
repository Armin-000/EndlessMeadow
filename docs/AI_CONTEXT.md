
# AI_CONTEXT.md

# Endless Meadow

Cinematic procedural meadow prototype / experimental mini-engine.

Stack:
- C++17
- raylib
- OpenGL
- CMake

Core philosophy:
- lightweight
- procedural
- atmospheric
- modular
- readable
- stylized
- no unnecessary ECS

---

# Architecture

```txt
include/ = declarations
src/     = implementations
````

Current structure:

```txt
include/
├── game.hpp
├── preload.hpp
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
├── game.cpp
├── preload.cpp
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
```

---

# Main Entry

`main.cpp`

```cpp
#include "game.hpp"

int main()
{
    RunGame();
    return 0;
}
```

`game.hpp`

```cpp
#pragma once

void RunGame();
```

Main gameplay loop:

* `src/game.cpp`

---

# Systems

## game.cpp

Responsibilities:

* main gameplay loop
* player movement
* camera update
* render order
* chunk streaming
* gameplay state
* pause menu
* world update

---

## preload.cpp

Responsibilities:

* cinematic loading screen
* loading progress
* fullscreen preload rendering
* glow UI
* animated particles
* debug replay system

Debug key:

```txt
Q = replay preload
```

Main function:

```cpp
DrawLoadingScreen()
```

---

## terrain.cpp

Responsibilities:

* terrain generation
* fractal noise
* ridge noise
* chunk mesh generation
* terrain normals
* terrain colors

Main functions:

```cpp
GenerateChunk()
GetTerrainHeight()
```

---

## vegetation.cpp

Responsibilities:

* grass mesh generation
* flower generation
* procedural trees
* procedural rocks
* vegetation rendering
* tree shaking

Main functions:

```cpp
GenerateGrassMesh()
DrawNature()
```

---

## water.cpp

Responsibilities:

* animated water shader
* water rendering
* wave movement

Main function:

```cpp
DrawWaterAroundPlayer()
```

---

## sky.cpp

Responsibilities:

* procedural clouds
* atmosphere
* cloud movement

Main function:

```cpp
DrawClouds()
```

---

## alien.cpp

Responsibilities:

* procedural alien rendering
* walk animation
* sprint animation
* falling animation
* backpack rendering
* attack animation
* antenna physics

Main function:

```cpp
DrawCuteAlien()
```

Alien features:

* reactive antennas
* cinematic backpack
* procedural body
* stylized movement

---

## collisions.cpp

Responsibilities:

* tree collision
* rock collision
* smart ground detection
* terrain-aware movement

Main functions:

```cpp
CheckNatureCollision()
GetSmartGroundY()
```

---

## map.cpp

Responsibilities:

* minimap rendering
* terrain visualization
* player tracking

Toggle:

```txt
M
```

---

## inventory.cpp

Responsibilities:

* backpack UI
* item movement
* slot rendering
* item organization

Toggle:

```txt
E
```

Current model:

```cpp
int inventorySlots[10]
```

---

## item_drops.cpp

Responsibilities:

* wood physics
* magnetic pickup
* bounce simulation
* inventory collection
* spinning drops

Main functions:

```cpp
SpawnWoodDrops()
UpdateWoodDrops()
DrawWoodDrops()
```

---

# Gameplay

Controls:

```txt
WASD        movement
SHIFT       sprint
SPACE       jump
SPACE x2    fly mode
CTRL        descend
MOUSE       camera
LMB         hit tree
E           inventory
M           minimap
ESC         pause
Q           preload replay
```

---

# Current Features

Implemented:

* infinite terrain
* chunk streaming
* procedural grass
* procedural flowers
* procedural trees
* procedural rocks
* animated water
* procedural clouds
* animated alien
* reactive antennas
* cinematic backpack
* minimap
* inventory system
* inventory organization
* tree destruction
* wood drops
* bounce physics
* magnetic pickup
* popup text
* fly mode
* preload system
* pause menu
* cinematic UI
* macOS bundle

---

# Render Order

```txt
sky
sun
terrain
grass
water
flowers
nature
wood drops
alien
effects
UI
menus
```

---

# Current Technical State

Architecture is modular.

Large gameplay loop moved from:

```txt
game.hpp
```

into:

```txt
game.cpp
```

Loading system separated into:

```txt
preload.cpp
```

Project is transitioning from:

```txt
prototype
```

toward:

```txt
mini-engine architecture
```

---

# AI Rules

When modifying project:

* preserve modular structure
* avoid giant files
* use separate .hpp/.cpp systems
* preserve procedural style
* preserve cinematic atmosphere
* avoid unnecessary complexity
* avoid ECS unless truly needed
* update CMakeLists.txt when adding files
* keep code copy-paste ready
* prefer readability over abstraction

---

# Important Notes

If linker errors appear:

* verify new `.cpp` exists in CMakeLists.txt

If missing symbols appear:

* verify `.hpp`
* verify `.cpp`
* verify signatures

If new gameplay systems become large:

* separate into dedicated module

Recommended future modules:

```txt
camera_controller.cpp
ui.cpp
menus.cpp
chunk_manager.cpp
combat.cpp
audio.cpp
particles.cpp
weather.cpp
```

---

# Long-Term Direction

Endless Meadow should feel like:

* procedural art project
* cozy survival prototype
* graphics playground
* cinematic exploration sandbox
* stylized indie mini-engine

Inspirations:

* Journey
* Sable
* Tiny Glade
* No Man's Sky (stylized side)
* Firewatch atmosphere