#!/bin/bash
# Build universal (arm64 + x86_64) macOS .app bundle for distribution.
# Usage: ./package_universal.sh

set -euo pipefail

SOURCE_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SOURCE_DIR/build_universal"
GLEW_UNI_DIR="$SOURCE_DIR/deps/glew-universal"
GLEW_VER="2.2.0"
# macOS 10.13 (High Sierra) is the oldest target this SDK supports.
# x86_64 Macs run up to macOS 15, arm64 Macs start at macOS 11.
MACOSX_DEPLOYMENT_TARGET="10.13"

echo "=== Step 1: Build universal GLEW static library ==="

if [ -f "$GLEW_UNI_DIR/lib/libGLEW.a" ]; then
    echo "Universal GLEW already exists, skipping..."
else
    echo "Downloading GLEW $GLEW_VER..."
    TMPDIR=$(mktemp -d)
    curl -sL "https://github.com/nigels-com/glew/releases/download/glew-${GLEW_VER}/glew-${GLEW_VER}.tgz" | tar xzf - -C "$TMPDIR"

    pushd "$TMPDIR/glew-${GLEW_VER}" > /dev/null
    clang -arch arm64 -arch x86_64 -c src/glew.c -I include -o glew.o -DGLEW_STATIC -DGLEW_BUILD -O2 -mmacosx-version-min="$MACOSX_DEPLOYMENT_TARGET"
    libtool -static -o libGLEW.a glew.o
    popd > /dev/null

    mkdir -p "$GLEW_UNI_DIR/lib"
    cp "$TMPDIR/glew-${GLEW_VER}/libGLEW.a" "$GLEW_UNI_DIR/lib/"
    cp -r "$TMPDIR/glew-${GLEW_VER}/include" "$GLEW_UNI_DIR/"

    rm -rf "$TMPDIR"
    echo "Universal GLEW built: $GLEW_UNI_DIR"
fi

echo ""
echo "=== Step 2: Build universal mineworld ==="

cmake -B "$BUILD_DIR" -S "$SOURCE_DIR" \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="$MACOSX_DEPLOYMENT_TARGET" \
    -DGLEW_CUSTOM_PATH="$GLEW_UNI_DIR" \
    -DCMAKE_BUILD_TYPE=Release

cmake --build "$BUILD_DIR" -j4

echo ""
echo "=== Step 3: Package .app bundle ==="

bash "$SOURCE_DIR/package_macos.sh" "$BUILD_DIR" "$SOURCE_DIR"

echo ""
echo "Universal .app ready: $BUILD_DIR/MineWorld.app"
lipo -info "$BUILD_DIR/MineWorld.app/Contents/MacOS/mineworld"
echo ""
echo "To create a zip for GitHub Releases:"
echo "  cd $BUILD_DIR && zip -r mineworld-macos-universal.zip MineWorld.app"
