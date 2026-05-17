# Graphify Workflow — Endless Meadow

## Activate virtual environment

```bash
source .graphify-venv/bin/activate
```

---

# Generate clean architecture graph

## Create temporary clean source folder

```bash
rm -rf clean-graph-src

mkdir clean-graph-src

cp main.cpp clean-graph-src/
cp game.hpp clean-graph-src/
cp world.hpp clean-graph-src/
```

---

## Run Graphify extraction

```bash
OLLAMA_MODEL=qwen2.5:7b graphify extract clean-graph-src \
  --backend ollama \
  --force \
  --out .
```

---

## Generate HTML tree graph

```bash
graphify tree \
  --graph graphify-out/graph.json \
  --output graphify-out/GRAPH_TREE.html \
  --root clean-graph-src \
  --label EndlessMeadow \
  --max-children 10
```

---

## Open graph

```bash
open graphify-out/GRAPH_TREE.html
```

---

# Notes

- Uses Ollama backend
- Uses qwen2.5:7b model
- Only scans:
  - main.cpp
  - game.hpp
  - world.hpp

This prevents Graphify from scanning:
- graphify/
- build/
- .graphify-venv/
- .vscode/

and keeps the architecture graph clean.

---

# Rebuild game

```bash
rm -rf build

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build

open build/EndlessMeadow.app