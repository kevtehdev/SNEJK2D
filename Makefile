# SNEJK2D Makefile
# Supports singleplayer and multiplayer

# Compiler and flags
CC=gcc
OPTIMIZE=-O2 -flto -ffunction-sections -fdata-sections
DEBUG_FLAGS=-g -O0 -Wfatal-errors
CFLAGS_BASE=-Wall -Wextra -Wno-psabi -std=c11

# Build mode: release (default) or debug
MODE ?= release

# Select flags per mode
ifeq ($(MODE),debug)
  CFLAGS=$(CFLAGS_BASE) $(DEBUG_FLAGS)
  LDFLAGS=-fsanitize=address -fno-omit-frame-pointer
else
  CFLAGS=$(CFLAGS_BASE) $(OPTIMIZE)
  LDFLAGS=-flto -Wl,--gc-sections
endif

# Directories
SRC_DIR=src
INCLUDE_DIR=include
BUILD_DIR=build
MPAPI_DIR=$(SRC_DIR)/mpapi/c_client/libs

# Libraries (with multiplayer dependencies)
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -ljansson -pthread -lm

# Include paths
INCLUDES=-I$(INCLUDE_DIR) -I$(MPAPI_DIR)

# Game source files
GAME_SOURCES=$(SRC_DIR)/snake.c \
             $(SRC_DIR)/game.c \
             $(SRC_DIR)/renderer.c \
             $(SRC_DIR)/audio.c \
             $(SRC_DIR)/multiplayer.c \
             $(SRC_DIR)/scoreboard.c \
             $(SRC_DIR)/settings.c \
             $(SRC_DIR)/input.c \
             $(SRC_DIR)/main.c

# mpapi source files
MPAPI_SOURCES=$(MPAPI_DIR)/mpapi.c \
              $(wildcard $(MPAPI_DIR)/jansson/*.c)

# All sources
SOURCES=$(GAME_SOURCES) $(MPAPI_SOURCES)

# Game object files
GAME_OBJECTS=$(BUILD_DIR)/snake.o \
             $(BUILD_DIR)/game.o \
             $(BUILD_DIR)/renderer.o \
             $(BUILD_DIR)/audio.o \
             $(BUILD_DIR)/multiplayer.o \
             $(BUILD_DIR)/scoreboard.o \
             $(BUILD_DIR)/settings.o \
             $(BUILD_DIR)/input.o \
             $(BUILD_DIR)/main.o

# mpapi object files
MPAPI_OBJECTS=$(patsubst $(MPAPI_DIR)/%.c,$(BUILD_DIR)/mpapi_%.o,$(MPAPI_SOURCES))

# All object files
OBJECTS=$(GAME_OBJECTS) $(MPAPI_OBJECTS)

# Executable name
EXECUTABLE=snejk2d

# Default target
all: $(EXECUTABLE)
	@echo ""
	@echo "✓ Build complete ($(MODE))!"
	@echo "Run with: ./$(EXECUTABLE)"
	@echo ""

# Link object files
$(EXECUTABLE): $(OBJECTS)
	@echo "Linking $(EXECUTABLE)..."
	@$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

# Compile game source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile mpapi source files
$(BUILD_DIR)/mpapi_%.o: $(MPAPI_DIR)/%.c
	@echo "Compiling mpapi: $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -D_GNU_SOURCE -I$(MPAPI_DIR) -c $< -o $@

# Debug build
debug:
	@echo "Building with debugging flags..."
	@$(MAKE) MODE=debug --no-print-directory clean $(EXECUTABLE)

# Run the game
run: $(EXECUTABLE)
	@./$(EXECUTABLE)

# Clean build files
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR) $(EXECUTABLE)

# Static analysis
lint:
	@echo "Running static analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=warning,style,performance,portability \
			--suppress=missingIncludeSystem \
			--inline-suppr \
			--quiet \
			$(SRC_DIR)/; \
	else \
		echo "cppcheck not installed. Install with: sudo apt-get install cppcheck"; \
	fi

# Install dependencies (WSL2/Ubuntu)
deps:
	@echo "Installing SDL2 and multiplayer dependencies..."
	@sudo apt-get update
	@sudo apt-get install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libjansson-dev uuid-dev

.PHONY: all clean debug run lint deps
