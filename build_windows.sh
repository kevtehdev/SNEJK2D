#!/bin/bash
# Build SNEJK2D for Windows using MinGW cross-compiler

set -e

echo "Building SNEJK2D for Windows..."

# Check if mingw is installed
if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo "ERROR: MinGW cross-compiler not found"
    echo "Install with: sudo apt-get install mingw-w64"
    exit 1
fi

# Setup Windows SDL2 libraries if needed
if [ ! -d "/tmp/win-libs" ]; then
    echo "Setting up Windows SDL2 libraries..."
    mkdir -p /tmp/win-libs

    # Download and extract SDL2 libraries (if not already present)
    # Note: You'll need to manually download these once
    echo "Please ensure SDL2 Windows libraries are in /tmp/win-libs/"
    echo "Required: SDL2, SDL2_image, SDL2_mixer, SDL2_ttf"
fi

# Build using Makefile.win
echo "Building with Makefile.win..."
make -f Makefile.win clean
make -f Makefile.win

if [ -f "snejk2d.exe" ]; then
    echo "✓ Build complete: snejk2d.exe"

    # Copy DLLs
    echo "Copying required DLLs..."
    if [ -d "win64_deps/dlls" ]; then
        cp win64_deps/dlls/*.dll .
        echo "✓ DLLs copied"
    else
        echo "Warning: win64_deps/dlls/ not found"
        echo "You'll need to manually copy SDL2 DLLs"
    fi

    echo ""
    echo "✓ Windows build complete!"
    echo ""
    echo "Files created:"
    ls -lh snejk2d.exe 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}'
    ls -lh *.dll 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}' | head -5
else
    echo "✗ Build failed!"
    exit 1
fi
