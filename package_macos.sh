#!/bin/bash
# Create macOS .app bundle for distribution.
# Usage: ./package_macos.sh <build_dir> <source_dir>

set -euo pipefail

BUILD_DIR="${1:-build}"
SOURCE_DIR="${2:-.}"
BINARY="$BUILD_DIR/mineworld"
APP_DIR="$BUILD_DIR/MineWorld.app"

if [ ! -f "$BINARY" ]; then
    echo "Error: $BINARY not found. Build first: cmake --build $BUILD_DIR"
    exit 1
fi

rm -rf "$APP_DIR"
mkdir -p "$APP_DIR/Contents/MacOS"
mkdir -p "$APP_DIR/Contents/Resources"

# --- Info.plist ---
cat > "$APP_DIR/Contents/Info.plist" << 'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>English</string>
	<key>CFBundleDisplayName</key>
	<string>MineWorld</string>
	<key>CFBundleExecutable</key>
	<string>mineworld</string>
	<key>CFBundleIdentifier</key>
	<string>yuchenxi2000.mineworld</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>mineworld</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
	<string>3.1</string>
	<key>CFBundleVersion</key>
	<string>3.1.0</string>
	<key>NSHumanReadableCopyright</key>
	<string>Copyright (c) 2019-2026 yuchenxi2000. Licensed under MIT license.</string>
	<key>NSHighResolutionCapable</key>
	<true/>
</dict>
</plist>
PLIST

# --- Bundle binary ---
echo "Creating MineWorld.app..."

# copy binary to MacOS/
cp -X "$BINARY" "$APP_DIR/Contents/MacOS/"

# check if GLEW is dynamically linked (Homebrew shared build) or statically linked (universal build)
GLEW_DYLIB=$(otool -L "$BINARY" | grep -oE '/[^ ]*libGLEW[^ ]*\.dylib' || true)

if [ -n "$GLEW_DYLIB" ]; then
    GLEW_NAME=$(basename "$GLEW_DYLIB")
    echo "Bundling GLEW dylib: $GLEW_DYLIB"

    # copy GLEW dylib to MacOS/ (next to the binary)
    cp -X "$GLEW_DYLIB" "$APP_DIR/Contents/MacOS/"

    # fix binary to find GLEW at @executable_path
    install_name_tool -change "$GLEW_DYLIB" "@executable_path/$GLEW_NAME" "$APP_DIR/Contents/MacOS/mineworld"

    # Sign bundled code before sealing the complete app below.
    codesign --force --sign - "$APP_DIR/Contents/MacOS/$GLEW_NAME"
else
    echo "GLEW is statically linked, no dylib bundling needed"
fi

# Copy resources without Finder metadata/resource forks, which codesign rejects.
cp -RX "$SOURCE_DIR/res/" "$APP_DIR/Contents/Resources/"

# Sign after copying resources so the seal describes the finished bundle.
# This also replaces the signature invalidated by install_name_tool.
codesign --force --sign - "$APP_DIR"
codesign --verify --deep --strict --verbose=2 "$APP_DIR"

echo ""
echo "App bundle ready: $APP_DIR"
echo "To create a zip for GitHub Releases:"
echo "  cd $BUILD_DIR && zip -r mineworld-macos.zip MineWorld.app"
