
<p align="center">
  <img src="./assets/endlessmeadow.png" width="180" alt="Endless Meadow Logo">
</p>

<h1 align="center">Endless Meadow 🌿</h1>

<p align="center">
  A cinematic procedural meadow prototype and experimental mini-engine built in modern C++ using raylib and OpenGL.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue">
  <img src="https://img.shields.io/badge/raylib-5.5-green">
  <img src="https://img.shields.io/badge/OpenGL-Renderer-orange">
  <img src="https://img.shields.io/badge/Procedural-Generation-brightgreen">
</p>

<p align="center">
  <img src="./assets/wallpaper.png" width="100%" alt="Endless Meadow Wallpaper">
</p>

---

![Endless Meadow](./assets/meadoww.png)

---

# Features

- 🌱 Infinite procedural terrain
- 🌊 Animated water system
- ☁️ Atmospheric clouds
- 🌲 Procedural trees and rocks
- 🌸 GPU flower rendering
- 👾 Animated alien character
- 🧠 Reactive alien antenna physics
- 🎒 Cinematic procedural backpack
- 🪵 Tree chopping & destruction
- 📦 Wood drop physics system
- ✨ Magnetic item pickup animation
- 🗺️ Live terrain minimap
- 🎮 Chunk streaming world
- 🎒 Backpack inventory system
- ✨ Pickup popup text
- 🛫 Fly mode
- 🎬 Cinematic exploration

---

# Controls

| Key | Action |
|------|--------|
| W A S D | Move |
| Mouse | Camera |
| SHIFT | Sprint |
| SPACE | Jump |
| Double SPACE | Fly Mode |
| CTRL | Fly Down |
| E | Backpack |
| M | Map |
| Left Click | Hit Tree |
| ESC | Pause |

---

# Project Structure

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
```

---

# Build

## macOS

Install dependencies:

```bash
brew install raylib cmake
```

Clone project:

```bash
git clone https://github.com/Armin-000/EndlessMeadow.git

cd EndlessMeadow
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

# Technologies

- C++17
- raylib
- OpenGL
- GLSL
- CMake
- Procedural Generation

---

# Current Systems

- Infinite procedural terrain
- Runtime chunk streaming
- GPU grass rendering
- Procedural flower rendering
- Animated water shader
- Atmospheric cloud rendering
- Procedural tree & rock generation
- Alien animation system
- Reactive antenna physics
- Cinematic backpack rendering
- Tree collision system
- Tree destruction system
- Procedural wood drop physics
- Terrain-aware bouncing logs
- Magnetic pickup animation
- Inventory UI system
- Real-time terrain minimap
- Fly mode
- Pause menu
- Loading screen

---

# Future Plans

- Pickup sound effects
- Falling tree animation
- Tree stump system
- Crafting system
- Hotbar
- Day/night cycle
- Dynamic weather
- Wildlife
- Biomes
- Better inventory system
- Survival mechanics
- Save/load system

---

# License

This project is intended for:

- learning
- experimentation
- graphics programming
- procedural rendering research
- portfolio showcase

See `LICENSE.md`.