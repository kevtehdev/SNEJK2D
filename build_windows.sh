#!/bin/bash
# Build SNEJK2D for Windows using MinGW cross-compiler

set -e

# Directories
SDL2_DIR="/tmp/SDL2-2.30.0/x86_64-w64-mingw32"
SDL2_TTF_DIR="/tmp/SDL2_ttf-2.22.0/x86_64-w64-mingw32"
SDL2_IMAGE_DIR="/tmp/SDL2_image-2.8.2/x86_64-w64-mingw32"
SDL2_MIXER_DIR="/tmp/SDL2_mixer-2.8.0/x86_64-w64-mingw32"

# Compiler
CC=x86_64-w64-mingw32-gcc

# Flags
CFLAGS="-O2 -Wall -Wextra -std=c11"
CFLAGS="$CFLAGS -I$SDL2_DIR/include/SDL2"
CFLAGS="$CFLAGS -I$SDL2_DIR/include"
CFLAGS="$CFLAGS -I$SDL2_TTF_DIR/include/SDL2"
CFLAGS="$CFLAGS -I$SDL2_TTF_DIR/include"
CFLAGS="$CFLAGS -I$SDL2_IMAGE_DIR/include/SDL2"
CFLAGS="$CFLAGS -I$SDL2_IMAGE_DIR/include"
CFLAGS="$CFLAGS -I$SDL2_MIXER_DIR/include/SDL2"
CFLAGS="$CFLAGS -I$SDL2_MIXER_DIR/include"
CFLAGS="$CFLAGS -Iinclude"
CFLAGS="$CFLAGS -Isrc/mpapi/c_client/libs"
CFLAGS="$CFLAGS -Isrc/mpapi/c_client/libs/jansson"
CFLAGS="$CFLAGS -D_GNU_SOURCE"
CFLAGS="$CFLAGS -DSDL_MAIN_HANDLED"

LDFLAGS="-L$SDL2_DIR/lib"
LDFLAGS="$LDFLAGS -L$SDL2_TTF_DIR/lib"
LDFLAGS="$LDFLAGS -L$SDL2_IMAGE_DIR/lib"
LDFLAGS="$LDFLAGS -L$SDL2_MIXER_DIR/lib"
LDFLAGS="$LDFLAGS -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer"
LDFLAGS="$LDFLAGS -lws2_32 -lwinhttp -lpthread -lm -static-libgcc"
LDFLAGS="$LDFLAGS -mwindows"

echo "Building SNEJK2D for Windows..."

# Create build directory
mkdir -p build_win

# Compile source files
echo "Compiling source files..."
$CC $CFLAGS -c src/snake.c -o build_win/snake.o
$CC $CFLAGS -c src/game.c -o build_win/game.o
$CC $CFLAGS -c src/renderer.c -o build_win/renderer.o
$CC $CFLAGS -c src/audio.c -o build_win/audio.o
$CC $CFLAGS -c src/multiplayer.c -o build_win/multiplayer.o
$CC $CFLAGS -c src/scoreboard.c -o build_win/scoreboard.o
$CC $CFLAGS -c src/settings.c -o build_win/settings.o
$CC $CFLAGS -c src/input.c -o build_win/input.o
$CC $CFLAGS -c src/main.c -o build_win/main.o

# Compile mpapi
echo "Compiling mpapi..."
$CC $CFLAGS -c src/mpapi/c_client/libs/mpapi.c -o build_win/mpapi.o

# Compile jansson
echo "Compiling jansson..."
for file in src/mpapi/c_client/libs/jansson/*.c; do
    basename=$(basename "$file" .c)
    $CC $CFLAGS -c "$file" -o "build_win/jansson_$basename.o"
done

# Link
echo "Linking..."
$CC build_win/*.o -o snejk2d.exe $LDFLAGS

# Copy DLLs
echo "Copying required DLLs..."
cp win64_deps/dlls/*.dll .

echo "✓ Build complete: snejk2d.exe"
echo "✓ DLLs copied"
