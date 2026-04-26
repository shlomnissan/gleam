#!/usr/bin/env sh

set -e  # Exit on error

BUILD_DIR=${1:-build}

echo "Generating Doxygen documentation"

# Ensure build directory exists and CMake is configured
if [ ! -f "$BUILD_DIR/Makefile" ] && [ ! -f "$BUILD_DIR/build.ninja" ]; then
    echo "Configuring CMake"
    cmake -S . -B "$BUILD_DIR" \
        -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
        -D VGLX_BUILD_IMGUI=OFF \
        -D VGLX_BUILD_DOCS=ON
fi

# Build the Doxygen target
cmake --build "$BUILD_DIR" --target doxygen

# Generate markdown pages from Doxygen XML
python3 -m tools.doxy_press.main build/docs/xml docs/