# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```bash
cd build
cmake ..
make
```

Requires GLEW. On macOS: `brew install glew`. On Linux: `apt install libglew-dev`.

The built executable must run from `res/` — that directory must contain `config.json`, `block.json`, shaders, and textures:

```bash
cd res && ../build/mineworld
```

### macOS packaging for GitHub Releases

```bash
cmake --build build --target package
```

This creates `build/mineworld-release/` with the binary, bundled GLEW dylib, and `res/`. Zip it for distribution:

```bash
cd build/mineworld-release && zip -r ../mineworld-macos.zip .
```

## Dependencies (git submodules in `deps/`)

- **GLFW** — window creation, OpenGL context, input
- **GLEW** — OpenGL extension loading (system-installed, not a submodule)
- **GLM** — vector/matrix math (`glm::ivec3`, `glm::vec3`, `glm::mat4`)
- **rapidjson** — JSON parsing for config and block definitions
- **SOIL** (Simple OpenGL Image Library) — texture image loading

All except GLEW are git submodules. Clone with `git clone --recursive` or run `git submodule update --init --recursive` after cloning.

## Architecture

The game is built in the `mineworld` namespace (C++17). Everything lives under `src/`. Resources (shaders, textures, config) are in `res/`.

### Entry point and game loop (`main.cpp`)

1. Create GLFW window (`glfwStart()`)
2. Load config from `config.json` (`importSetting()`)
3. Load block definitions from the block file specified in config (`BlockRegister::loadBlock()`)
4. Initialize world generator, shaders (skybox → block → crosshair), player, input handler, terminal, worker threads
5. Main loop: poll events → handle input → render skybox + chunks → render terminal/crosshair → swap buffers

### World structure

The world is procedurally generated using a **chunk → cell → block** hierarchy:

- **Block** (`block.hpp`) — a block type with a 3D model (list of `rect` faces) and hitboxes. Block subclasses: `Air`, `Cube`, `Glass`, `Slab`, `Stairs`, `Rail`, `Grass`. `BlockRegister` loads block definitions from a JSON file and manages the ID→block mapping. Global: `gblockregister`.
- **Cell** (`cell.hpp`) — a 16×16×16 volume of blocks. Stores block IDs in `blockbuffer[4096]` and generated vertex data (`glRectBuffer`). Tracks neighbor cell data via `nearblock[6][16][16]` for face culling.
- **Chunk** (`cell.hpp`) — manages a set of cells around the player. Loads/unloads cells as the player moves. Only renders cells within `VISIBLE_DISTANCE`. Global: `gchunk`.
- **WorldGenerator** (`world.hpp`) — fills a new cell with block IDs procedurally. Global: `gworldgenerator`.

Coordinate system: `block_loc_t` = chunk position (multiple of 16) + offset (0–15). `entity_pos_t` = chunk position + continuous float offset.

### Rendering

- **Shader** — main block shader with texture atlas + skybox cubemap. Renders cells via `drawCell()`. Global: `gblockshader`.
- **SkyBoxShader** — renders the skybox cube. Global: `gskyboxshader`.
- **CrosshairShader** — renders the crosshair with color inversion. Global: `gcrosshairshader`.
- **TextureManager** (`texture.hpp`) — packs block face textures into a single texture atlas (max size `ATLAS_SIZE_UPPER_BOUND`). Global: `gtexturemanager`.
- **glBuffer\<T\>** (`glbuffer.hpp`) — templated OpenGL VAO/VBO/EBO wrapper for batches of vertices.
- **model.hpp** — helper functions (`rectX`, `rectY`, `rectZ`, `rectP`) for constructing block face geometry with correct UV coordinates from the texture atlas.

### Input handling

`Handler` (`handler.hpp`) — processes keyboard (W/A/S/D movement, Esc pause, T for terminal, arrow keys to switch held block), mouse (look direction, left click to break, right click to place). Manages play state: `PLAYING`, `PAUSE`, `TYPING`. Updates `ViewMatrix` and `ProjectionMatrix` each frame. Global: `handler`.

### In-game terminal

- **Terminal** (`terminal.hpp`) — overlays a text terminal for commands (`set`, `query`, `blockid`, `blockname`, `blocklist`, `goto`, `say`, `font`, `clear`, `exit`). Global: `gterminal`.
- **Board** — single status line showing version + held block. Global: `gboard`.

### Worker threads (`worker.hpp`)

A thread pool (`workerStart(n)`) offloads mesh (vertex buffer) generation to background threads. Tasks are pushed to `task_queue` and results read from `result_queue`, synchronized via mutex + condition variable. The main thread must call GPU uploads on results.

### Raycasting

`Hitbox` (`hitbox.hpp`) — axis-aligned box for raycasting against blocks. `Entity::updateLookAt()` casts a ray through the player's look direction to find the targeted block/entity.

## Configuration (`res/config.json`)

Separated into `config` (resource paths, texture sizes, visible/chunk distances) and `setting` (FOV, window size, speeds, username, initial position). Both are parsed into global `Config config` and `Setting setting` structs at startup.
