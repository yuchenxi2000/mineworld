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

- CMake 3.12+
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
   mkdir build && cd build
   cmake .. -G "MinGW Makefiles"
   cmake --build .
   ```

4. Run (**must be run from the `res/` directory**):

   ```bash
   cd ../res && ../build/mineworld.exe
   ```

#### Option 2: Visual Studio Build Tools

1. Install [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/) (find "Build Tools for Visual Studio" under "All Downloads"), select "Desktop development with C++".

2. Download [GLEW pre-built binaries](https://glew.sourceforge.net/), extract to `deps/glew-2.3.1/`.

3. Open **PowerShell** (not MSYS2 terminal), build the Release version:

   ```powershell
   mkdir build_vs; cd build_vs
   cmake .. -G "Visual Studio 17 2022" -DGLEW_ROOT="<path-to>/deps/glew-2.3.1"
   cmake --build . --config Release
   ```

   > Replace `-DGLEW_ROOT` with the actual absolute path to the extracted GLEW.

   > **Note:** Make sure MSYS2 paths (e.g. `C:\msys64\ucrt64\bin`) are NOT in PowerShell's PATH, or MSVC will pick up MinGW headers and fail.

4. Run:

   ```powershell
   cd ../res; ..\build_vs\Release\mineworld.exe
   ```

> Adjust `-G` for different VS versions (e.g. `"Visual Studio 18 2026"` for VS 2026).

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

### Important

The executable **must be run from the `res/` directory**, because the program reads `config.json`, textures, shaders, and other resources from the current working directory.

## Custom Resource Packs

The resource pack is borrowed from Minecraft.

When the game launches, it reads settings from `config.json`, then loads block information (the resource pack) from the block file specified in `config.json`.

To create your own resource pack, refer to the included `block.json`.

## License

MIT License. See [LICENSE.txt](LICENSE.txt).
