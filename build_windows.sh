#!/bin/bash
# Build SNEJK2D for Windows using MinGW cross-compiler

set -e

echo "Building SNEJK2D for Windows..."

# SDL2 paths in /tmp
SDL2_BASE="/tmp/SDL2-2.28.5/x86_64-w64-mingw32"
SDL2_IMAGE_BASE="/tmp/SDL2_image-2.6.3/x86_64-w64-mingw32"
SDL2_MIXER_BASE="/tmp/SDL2_mixer-2.6.3/x86_64-w64-mingw32"
SDL2_TTF_BASE="/tmp/SDL2_ttf-2.20.2/x86_64-w64-mingw32"

# Compiler
CC=x86_64-w64-mingw32-gcc

# Include paths
INCLUDES="-Iinclude"
INCLUDES="$INCLUDES -I$SDL2_BASE/include -I$SDL2_BASE/include/SDL2"
INCLUDES="$INCLUDES -I$SDL2_IMAGE_BASE/include -I$SDL2_IMAGE_BASE/include/SDL2"
INCLUDES="$INCLUDES -I$SDL2_MIXER_BASE/include -I$SDL2_MIXER_BASE/include/SDL2"
INCLUDES="$INCLUDES -I$SDL2_TTF_BASE/include -I$SDL2_TTF_BASE/include/SDL2"

# Library paths
LIBS="-L$SDL2_BASE/lib"
LIBS="$LIBS -L$SDL2_IMAGE_BASE/lib"
LIBS="$LIBS -L$SDL2_MIXER_BASE/lib"
LIBS="$LIBS -L$SDL2_TTF_BASE/lib"
LIBS="$LIBS -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf"
LIBS="$LIBS -lws2_32 -liphlpapi -lpthread -mwindows"

# Compiler flags
CFLAGS="-O2 -Wall -std=c11"

# Source files
SOURCES="src/main.c src/game.c src/snake.c src/renderer.c src/audio.c"
SOURCES="$SOURCES src/multiplayer.c src/scoreboard.c src/settings.c src/input.c"
SOURCES="$SOURCES src/mpapi/c_client/libs/mpapi.c"
SOURCES="$SOURCES src/mpapi/c_client/libs/jansson/*.c"

# Output
OUTPUT="snejk2d.exe"

echo "Compiling source files..."
$CC $CFLAGS $INCLUDES $SOURCES $LIBS -o $OUTPUT

if [ -f "$OUTPUT" ]; then
    echo "✓ Build complete: $OUTPUT"

    # Copy DLLs
    echo "Copying required DLLs..."
    cp $SDL2_BASE/bin/SDL2.dll .
    cp $SDL2_IMAGE_BASE/bin/*.dll .
    cp $SDL2_MIXER_BASE/bin/*.dll .
    cp $SDL2_TTF_BASE/bin/*.dll .

    echo "✓ Windows build complete!"
    echo ""
    echo "Files created:"
    ls -lh snejk2d.exe *.dll | awk '{print "  " $9 " (" $5 ")"}'
else
    echo "✗ Build failed!"
    exit 1
fi
