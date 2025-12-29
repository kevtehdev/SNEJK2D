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

# Core source files
CORE_SOURCES=$(SRC_DIR)/core/main.c \
             $(SRC_DIR)/core/game.c \
             $(SRC_DIR)/core/state_machine.c \
             $(SRC_DIR)/core/event_system.c \
             $(SRC_DIR)/core/runtime_config.c

# Entity source files
ENTITY_SOURCES=$(SRC_DIR)/entities/snake.c

# Audio source files
AUDIO_SOURCES=$(SRC_DIR)/audio/audio_system.c

# Multiplayer source files
MULTIPLAYER_SOURCES=$(SRC_DIR)/multiplayer/mp_core.c \
                    $(SRC_DIR)/multiplayer/mp_network.c \
                    $(SRC_DIR)/multiplayer/mp_host.c \
                    $(SRC_DIR)/multiplayer/mp_client.c \
                    $(SRC_DIR)/multiplayer/mp_game.c \
                    $(SRC_DIR)/multiplayer/mp_turn_battle.c \
                    $(SRC_DIR)/multiplayer/mp_message_queue.c

# Utility source files
UTILS_SOURCES=$(SRC_DIR)/utils/scoreboard.c \
              $(SRC_DIR)/utils/settings.c \
              $(SRC_DIR)/utils/logger.c \
              $(SRC_DIR)/utils/debug_draw.c

# UI source files
UI_SOURCES=$(SRC_DIR)/ui/notifications.c

# Rendering module source files
RENDERING_SOURCES=$(SRC_DIR)/rendering/renderer_core.c \
                  $(SRC_DIR)/rendering/renderer_background.c \
                  $(SRC_DIR)/rendering/renderer_hud.c \
                  $(SRC_DIR)/rendering/renderer_game.c \
                  $(SRC_DIR)/rendering/renderer_effects.c \
                  $(SRC_DIR)/rendering/renderer_menus.c \
                  $(SRC_DIR)/rendering/renderer_multiplayer.c \
                  $(SRC_DIR)/rendering/renderer_ui.c

# Input module source files
INPUT_SOURCES=$(SRC_DIR)/input/input_core.c \
              $(SRC_DIR)/input/input_menu.c \
              $(SRC_DIR)/input/input_gameplay.c \
              $(SRC_DIR)/input/input_multiplayer.c

# State module source files
STATES_SOURCES=$(SRC_DIR)/states/state_loading.c \
               $(SRC_DIR)/states/state_main_menu.c \
               $(SRC_DIR)/states/state_mode_select.c \
               $(SRC_DIR)/states/state_name_input.c \
               $(SRC_DIR)/states/state_menu.c \
               $(SRC_DIR)/states/state_playing.c \
               $(SRC_DIR)/states/state_paused.c \
               $(SRC_DIR)/states/state_game_over.c \
               $(SRC_DIR)/states/state_scoreboard.c \
               $(SRC_DIR)/states/state_options.c \
               $(SRC_DIR)/states/state_multiplayer.c

# mpapi source files
MPAPI_SOURCES=$(MPAPI_DIR)/mpapi.c \
              $(wildcard $(MPAPI_DIR)/jansson/*.c)

# All sources
SOURCES=$(CORE_SOURCES) $(ENTITY_SOURCES) $(AUDIO_SOURCES) $(MULTIPLAYER_SOURCES) $(UTILS_SOURCES) \
        $(UI_SOURCES) $(RENDERING_SOURCES) $(INPUT_SOURCES) $(STATES_SOURCES) $(MPAPI_SOURCES)

# Core object files
CORE_OBJECTS=$(BUILD_DIR)/main.o \
             $(BUILD_DIR)/game.o \
             $(BUILD_DIR)/state_machine.o \
             $(BUILD_DIR)/event_system.o \
             $(BUILD_DIR)/runtime_config.o

# Entity object files
ENTITY_OBJECTS=$(BUILD_DIR)/snake.o

# Audio object files
AUDIO_OBJECTS=$(BUILD_DIR)/audio_system.o

# Multiplayer object files
MULTIPLAYER_OBJECTS=$(BUILD_DIR)/mp_core.o \
                    $(BUILD_DIR)/mp_network.o \
                    $(BUILD_DIR)/mp_host.o \
                    $(BUILD_DIR)/mp_client.o \
                    $(BUILD_DIR)/mp_game.o \
                    $(BUILD_DIR)/mp_turn_battle.o \
                    $(BUILD_DIR)/mp_message_queue.o

# Utility object files
UTILS_OBJECTS=$(BUILD_DIR)/scoreboard.o \
              $(BUILD_DIR)/settings.o \
              $(BUILD_DIR)/logger.o \
              $(BUILD_DIR)/debug_draw.o

# UI object files
UI_OBJECTS=$(BUILD_DIR)/notifications.o

# Rendering module object files
RENDERING_OBJECTS=$(BUILD_DIR)/renderer_core.o \
                  $(BUILD_DIR)/renderer_background.o \
                  $(BUILD_DIR)/renderer_hud.o \
                  $(BUILD_DIR)/renderer_game.o \
                  $(BUILD_DIR)/renderer_effects.o \
                  $(BUILD_DIR)/renderer_menus.o \
                  $(BUILD_DIR)/renderer_multiplayer.o \
                  $(BUILD_DIR)/renderer_ui.o

# Input module object files
INPUT_OBJECTS=$(BUILD_DIR)/input_core.o \
              $(BUILD_DIR)/input_menu.o \
              $(BUILD_DIR)/input_gameplay.o \
              $(BUILD_DIR)/input_multiplayer.o

# State module object files
STATES_OBJECTS=$(BUILD_DIR)/state_loading.o \
               $(BUILD_DIR)/state_main_menu.o \
               $(BUILD_DIR)/state_mode_select.o \
               $(BUILD_DIR)/state_name_input.o \
               $(BUILD_DIR)/state_menu.o \
               $(BUILD_DIR)/state_playing.o \
               $(BUILD_DIR)/state_paused.o \
               $(BUILD_DIR)/state_game_over.o \
               $(BUILD_DIR)/state_scoreboard.o \
               $(BUILD_DIR)/state_options.o \
               $(BUILD_DIR)/state_multiplayer.o

# mpapi object files
MPAPI_OBJECTS=$(patsubst $(MPAPI_DIR)/%.c,$(BUILD_DIR)/mpapi_%.o,$(MPAPI_SOURCES))

# All object files
OBJECTS=$(CORE_OBJECTS) $(ENTITY_OBJECTS) $(AUDIO_OBJECTS) $(MULTIPLAYER_OBJECTS) $(UTILS_OBJECTS) \
        $(UI_OBJECTS) $(RENDERING_OBJECTS) $(INPUT_OBJECTS) $(STATES_OBJECTS) $(MPAPI_OBJECTS)

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

# Compile core source files
$(BUILD_DIR)/main.o: $(SRC_DIR)/core/main.c
	@echo "Compiling core: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/game.o: $(SRC_DIR)/core/game.c
	@echo "Compiling core: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/state_machine.o: $(SRC_DIR)/core/state_machine.c
	@echo "Compiling core: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/event_system.o: $(SRC_DIR)/core/event_system.c
	@echo "Compiling core: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/runtime_config.o: $(SRC_DIR)/core/runtime_config.c
	@echo "Compiling core: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile entity source files
$(BUILD_DIR)/snake.o: $(SRC_DIR)/entities/snake.c
	@echo "Compiling entity: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile audio source files
$(BUILD_DIR)/audio_system.o: $(SRC_DIR)/audio/audio_system.c
	@echo "Compiling audio: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile multiplayer source files
$(BUILD_DIR)/mp_core.o: $(SRC_DIR)/multiplayer/mp_core.c
	@echo "Compiling multiplayer: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/mp_network.o: $(SRC_DIR)/multiplayer/mp_network.c
	@echo "Compiling multiplayer: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/mp_host.o: $(SRC_DIR)/multiplayer/mp_host.c
	@echo "Compiling multiplayer: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/mp_client.o: $(SRC_DIR)/multiplayer/mp_client.c
	@echo "Compiling multiplayer: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/mp_game.o: $(SRC_DIR)/multiplayer/mp_game.c
	@echo "Compiling multiplayer: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/mp_turn_battle.o: $(SRC_DIR)/multiplayer/mp_turn_battle.c
	@echo "Compiling multiplayer: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/mp_message_queue.o: $(SRC_DIR)/multiplayer/mp_message_queue.c
	@echo "Compiling multiplayer: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile utility source files
$(BUILD_DIR)/scoreboard.o: $(SRC_DIR)/utils/scoreboard.c
	@echo "Compiling utils: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/settings.o: $(SRC_DIR)/utils/settings.c
	@echo "Compiling utils: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/logger.o: $(SRC_DIR)/utils/logger.c
	@echo "Compiling utils: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/debug_draw.o: $(SRC_DIR)/utils/debug_draw.c
	@echo "Compiling utils: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile UI source files
$(BUILD_DIR)/notifications.o: $(SRC_DIR)/ui/notifications.c
	@echo "Compiling UI: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile rendering module source files
$(BUILD_DIR)/renderer_%.o: $(SRC_DIR)/rendering/renderer_%.c
	@echo "Compiling rendering module: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile input module source files
$(BUILD_DIR)/input_%.o: $(SRC_DIR)/input/input_%.c
	@echo "Compiling input module: $<..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile state module source files
$(BUILD_DIR)/state_%.o: $(SRC_DIR)/states/state_%.c
	@echo "Compiling state module: $<..."
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
