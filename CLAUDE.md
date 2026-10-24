# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```bash
cd build
cmake ..
make
```

Requires GLEW. On macOS: `brew install glew`. On Linux: `apt install libglew-dev`.

Resources must contain `config.json`, `block.json`, shaders, and textures. The executable checks the current directory, nearby `res/` directories, and macOS bundle Resources/. Running from `res/` is also supported:

```bash
cd res && ../build/mineworld
```

### macOS packaging for GitHub Releases

```bash
cmake --build build --target package
```

This creates `build/MineWorld.app` with the binary, resources, and any required GLEW dylib. Zip it for distribution:

```bash
cd build && zip -r mineworld-macos.zip MineWorld.app
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

1. Load and validate config, then create the GLFW window (`glfwStart()`)
2. Resource lookup accepts the current directory, nearby `res/` directories, and macOS bundle Resources/
3. Load block definitions from the block file specified in config (`BlockRegister::loadBlock()`)
4. Initialize world generator, shaders (skybox → block → crosshair), player, input handler, terminal, worker threads
5. Main loop: poll events → handle input → render skybox + chunks → render terminal/crosshair → swap buffers

### World structure

The world is procedurally generated using a **chunk → cell → block** hierarchy:

- **Block** (`block.hpp`) — a block type with a 3D model (list of `rect` faces) and hitboxes. Block subclasses: `Air`, `Cube`, `Glass`, `Slab`, `Stairs`, `Rail`, `Grass`. `BlockRegister` loads block definitions from a JSON file and manages the ID→block mapping. Global: `gblockregister`.
- **Cell** (`cell.hpp`) — a 16×16×16 volume of blocks. Stores block IDs in `blockbuffer[4096]` and generated vertex data (`glRectBuffer`). Owns block data and GPU buffers on the main thread. Neighbor borders exist only in immutable mesh-task snapshots (`mesh.hpp`).
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

A thread pool (`workerStart(n)`) offloads mesh (vertex buffer) generation to background threads. Typed tasks own block and border snapshots; results own their vertex arrays. A generation and revision identify each result, so late or stale results cannot overwrite current meshes. Only one task per cell is outstanding. The main thread applies results and performs GPU uploads. `workerStop()` cancels queued work and joins all threads before block definitions or OpenGL resources are released.

### Raycasting

`Hitbox` (`hitbox.hpp`) — axis-aligned box for raycasting against blocks. `Entity::updateLookAt()` casts a ray through the player's look direction to find the targeted block/entity.

## Configuration (`res/config.json`)

Separated into `config` (resource paths, texture sizes, visible/chunk distances) and `setting` (FOV, window size, speeds, username, initial position). Both are parsed into global `Config config` and `Setting setting` structs at startup.

## Regression tests

Configure with `-DBUILD_TESTING=ON`, build, then run `ctest --test-dir build --output-on-failure` (add `-C Release` for a multi-config generator). Tests cover image ownership, JSON and terminal validation, axis-parallel rays, empty meshes, cross-cell culling, stale results, unloading, worker shutdown, and UI upload invalidation without opening a window. `-DMINEWORLD_GL_TESTS=ON` additionally enables a smoke test with a hidden OpenGL 3.3 window; it needs a working desktop graphics context.

GPU resource wrappers cannot be copied. `Game` in main.cpp owns the player and controls cleanup: stop workers, clear cells, release screen/shader/texture resources, then destroy the window and terminate GLFW. SOIL pixel buffers are imported into `Image` vector storage and released using `SOIL_free_image_data`.
### Windows toolchain isolation

Use separate build directories: `build_vs` for Visual Studio and `build_msys2` for MSYS2. On MSVC, CMake explicitly imports the complete official Windows GLEW package from `GLEW_ROOT` (default `deps/glew-2.3.1`) and chooses x64/Win32 libraries to match the compiler. GLEW includes propagate through its target only, never into SOIL. Static GLEW uses the static MSVC runtime consistently across the game and dependencies. Shared GLEW is copied beside built executables. MSYS2 may remain on PATH; it must not supply MSVC's standard headers or libraries.