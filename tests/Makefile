# Test Makefile for SNEJK2D multiplayer modules

CC=gcc
CFLAGS=-Wall -Wextra -g -I../include -I../src/mpapi/c_client/libs
LDFLAGS=-ljansson -lm

# Source files needed for tests
TEST_SOURCES=test_mp_modules.c \
             ../src/multiplayer/mp_network.c \
             ../src/multiplayer/mp_game.c \
             ../src/multiplayer/mp_turn_battle.c \
             ../src/multiplayer/mp_message_queue.c \
             ../src/entities/snake.c \
             ../src/core/game.c

# Object files
TEST_OBJECTS=$(TEST_SOURCES:.c=.o)

# Executables
TEST_EXEC=test_mp_modules

all: $(TEST_EXEC)

$(TEST_EXEC): test_mp_modules.c
	@echo "Building unit tests..."
	$(CC) $(CFLAGS) -o $@ $^ \
		../src/multiplayer/mp_network.c \
		../src/multiplayer/mp_game.c \
		../src/multiplayer/mp_turn_battle.c \
		../src/multiplayer/mp_message_queue.c \
		../src/entities/snake.c \
		../src/core/game.c \
		../src/mpapi/c_client/libs/jansson/*.c \
		$(LDFLAGS)
	@echo "✓ Tests compiled successfully"

run: $(TEST_EXEC)
	@echo ""
	@echo "Running unit tests..."
	@./$(TEST_EXEC)

clean:
	rm -f $(TEST_EXEC) *.o

.PHONY: all run clean
