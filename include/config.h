#ifndef __CONFIG_H_
#define __CONFIG_H_

// Window & Grid Configuration

#define CELL_SIZE 20
#define GRID_WIDTH 32
#define GRID_HEIGHT 24

#define WINDOW_WIDTH (GRID_WIDTH * CELL_SIZE)   // 640
#define WINDOW_HEIGHT (GRID_HEIGHT * CELL_SIZE) // 480

// HUD Border Configuration

#define HUD_BORDER_CELLS 2
#define HUD_BORDER_THICKNESS (HUD_BORDER_CELLS * CELL_SIZE)

// Playable Area (inside HUD walls)

#define PLAYABLE_MIN_X HUD_BORDER_CELLS
#define PLAYABLE_MAX_X (GRID_WIDTH - HUD_BORDER_CELLS - 1)

#define PLAYABLE_MIN_Y HUD_BORDER_CELLS
#define PLAYABLE_MAX_Y (GRID_HEIGHT - HUD_BORDER_CELLS - 1)

// Game Config

#define INITIAL_SNAKE_LENGTH 3
#define BASE_SPEED 100     // ms per move
#define MIN_SPEED 40       
#define SPEED_INCREASE 5

// Combo System

#define COMBO_TIMEOUT_MIN 2500
#define COMBO_TIMEOUT_MAX 6000

#define COMBO_TIER_2 3
#define COMBO_TIER_3 5
#define COMBO_TIER_4 8
#define COMBO_TIER_5 12

// Colors (RGB)

#define COLOR_SNAKE_R 0
#define COLOR_SNAKE_G 255
#define COLOR_SNAKE_B 0

#define COLOR_FOOD_R 255
#define COLOR_FOOD_G 0
#define COLOR_FOOD_B 0

#define COLOR_GRID_R 255
#define COLOR_GRID_G 255
#define COLOR_GRID_B 255
#define COLOR_GRID_A 2

// Background overlay & Background count

#define BACKGROUND_OVERLAY_ALPHA 180
#define NUM_BACKGROUNDS 5

// Multiplayer identification

#define STUDENT_NUMBER "67"

#endif //__CONFIG_H_
