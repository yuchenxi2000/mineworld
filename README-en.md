# MineWorld

[中文](README.md)

## Introduction

A simple Minecraft clone, written in C++.

## Controls

* W/A/S/D : Move forward / left / backward / right
* Esc : Pause / Resume
* Left mouse button : Break block
* Right mouse button : Place block
* T : Enter command (see **Commands**)
* Left/Right arrow keys : Switch held block

## Commands

* Place block

```
set x y z B  // Place block with ID B at (x, y, z)
```

* Query block ID

```
query x y z  // Return the block ID at (x, y, z)
```

* Look up block ID by name

```
blockid s    // Return the block ID for block named s
```

* Look up block name by ID

```
blockname id // Return the block name for block ID id
```

* List all blocks

```
blocklist    // Print the block list
```

* Teleport

```
goto x y z   // Teleport to (x, y, z)
```

* Say

```
say words    // Print words in the terminal
```

* Adjust font size

```
font S       // Set font size to S pixels
```

* Clear screen

```
clear        // Clear the terminal screen
```

* Exit game

```
exit         // Exit the game
```

## Screenshots

![image](screenshots/example1.jpg)

## Build

### Prerequisites

- CMake 3.15+
- Git (for cloning the repository and submodules)
- A C++17 compiler

```bash
git clone --recursive https://github.com/yuchenxi2000/mineworld.git
cd mineworld
```

### Windows

Two options available.

#### Option 1: MSYS2 UCRT64

1. Install [MSYS2](https://www.msys2.org/), then launch it from the Start menu.

2. Install build tools and GLEW:

   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-glew
   ```

3. Build:

   ```bash
   cmake -S . -B build_msys2 -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
   cmake --build build_msys2 --parallel
   ```

4. Run:

   ```bash
   cd res && ../build_msys2/mineworld.exe
   ```

#### Option 2: Visual Studio Build Tools

1. Install [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/) (find "Build Tools for Visual Studio" under "All Downloads"), select "Desktop development with C++".

2. Download the [official GLEW Windows binary package](https://github.com/nigels-com/glew/releases/tag/glew-2.3.1) and extract the complete `glew-2.3.1` directory into `deps/`. Verify that both files exist:

   ```text
   deps/glew-2.3.1/include/GL/glew.h
   deps/glew-2.3.1/lib/Release/x64/glew32s.lib
   ```

   Extract `include/` as well as `bin/` and `lib/`. MSVC cannot use MSYS2 standard-library headers or `.a` libraries.

3. Open **PowerShell** in the repository root and build with the installed Visual Studio generator:

   ```powershell
   $glewRoot = Join-Path (Get-Location) "deps/glew-2.3.1"
   cmake -S . -B build_vs -G "Visual Studio 18 2026" "-DGLEW_ROOT=$glewRoot"
   cmake --build build_vs --config Release --parallel
   ```

   > Use `"Visual Studio 17 2022"` for VS 2022. Keep MSVC and MSYS2 in separate build directories (`build_vs` and `build_msys2`). Do not reuse one directory with another generator.

   > CMake imports the MSVC GLEW package from `GLEW_ROOT` explicitly. MSYS2 may remain on PATH. Re-run the configuration command to update older generated projects.

   > Static GLEW and the static MSVC runtime are used by default. With `-DGLEW_SHARED=ON`, the GLEW DLL is copied beside built executables automatically.

4. Run:

   ```powershell
   .\build_vs\Release\mineworld.exe
   ```

### macOS

```bash
brew install glew
mkdir build && cd build
cmake ..
make
cd ../res && ../build/mineworld
```

### Linux

```bash
apt install libglew-dev   # or your package manager
mkdir build && cd build
cmake ..
make
cd ../res && ../build/mineworld
```

### Regression tests

Configure with `-DBUILD_TESTING=ON`, build, and run `ctest --test-dir build --output-on-failure`. For multi-config generators, also specify the configuration, for example `-C Release`. Default regression tests do not open a window. Enable `-DMINEWORLD_GL_TESTS=ON` for an additional hidden-window OpenGL smoke test, which requires a working desktop graphics context.

For macOS package and Universal Binary validation commands, see [tests/README.md](tests/README.md).

### Important

The executable checks the current directory, nearby `res/` directories, and macOS bundle `Resources/` for `config.json`. Keep the resource directory and its textures and shaders together.

## Custom Resource Packs

The resource pack is borrowed from Minecraft.

When the game launches, it reads settings from `config.json`, then loads block information (the resource pack) from the block file specified in `config.json`.

To create your own resource pack, refer to the included `block.json`.

## License

MIT License. See [LICENSE.txt](LICENSE.txt).
