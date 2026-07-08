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

# --- Bundle GLEW dylib ---
GLEW_DYLIB=$(otool -L "$BINARY" | grep -oE '/[^ ]*libGLEW[^ ]*\.dylib')
GLEW_NAME=$(basename "$GLEW_DYLIB")

echo "GLEW dylib: $GLEW_DYLIB"
echo "Creating MineWorld.app..."

# copy binary to MacOS/
cp "$BINARY" "$APP_DIR/Contents/MacOS/"

# copy GLEW dylib to MacOS/ (next to the binary)
cp "$GLEW_DYLIB" "$APP_DIR/Contents/MacOS/"

# fix binary to find GLEW at @executable_path
install_name_tool -change "$GLEW_DYLIB" "@executable_path/$GLEW_NAME" "$APP_DIR/Contents/MacOS/mineworld"

# re-sign binary (install_name_tool invalidates the linker's ad-hoc signature)
codesign --force --sign - "$APP_DIR/Contents/MacOS/mineworld"

# copy resources to Resources/
cp -r "$SOURCE_DIR/res/" "$APP_DIR/Contents/Resources/"

echo ""
echo "App bundle ready: $APP_DIR"
echo "To create a zip for GitHub Releases:"
echo "  cd $BUILD_DIR && zip -r mineworld-macos.zip MineWorld.app"
