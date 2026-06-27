#!/bin/bash
# Copy .so files from lib-android_arm64 to jniLibs
# Usage: ./copy_sos.sh /path/to/lib-android_arm64

set -ex
SRC="${1:-/root/lib-android-fork}"
DEST="app/src/main/jniLibs/arm64-v8a"
mkdir -p "$DEST"
find "$SRC" -name "*.so" -type f -exec cp {} "$DEST/" \;
echo "Copied $(ls "$DEST"/*.so 2>/dev/null | wc -l) .so files to $DEST"
ls -la "$DEST"/
