#ifndef __GAME_H_
#define __GAME_H_

#include "config.h"
#include "snake.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum {
    GAME_MAIN_MENU,      // Singleplayer / Multiplayer choice
    GAME_NAME_INPUT,     // Name input before background selection
    GAME_MENU,           // Background selection (singleplayer)
    GAME_PLAYING,        // Singleplayer gameplay
    GAME_PAUSED,
    GAME_OVER,
    GAME_SCOREBOARD,     // Scoreboard view
    GAME_MULTIPLAYER     // Multiplayer mode (separate state)
} GameState;

// Explosion effect
typedef struct {
    bool active;
    Position position;
    int frame;
    unsigned int startTime;
} Explosion;

typedef struct {
    Snake snake;
    Position food;
    GameState state;
    int selectedBackground;      
    unsigned int lastMoveTime;
    unsigned int currentSpeed;   // Current speed in ms
    bool running;

    // Combo system
    int comboCount;              // Current combo streak
    unsigned int lastFoodTime;   // Time when last food was eaten
    float comboMultiplier;       // Score multiplier based on combo (1.0, 1.5, 2.0, 3.0, 5.0)

    // Game Over menu
    int gameOverSelection;       // 0 = Try Again, 1 = Main Menu

    // Explosion effect
    Explosion explosion;
    bool scoreSaved;             // Track if score was saved to prevent duplicates

    // Player name
    char playerName[16];
    int playerNameLen;

    // Highscore flag
    bool isHighscore;            // True if current score is a highscore
} Game;

/* Initialize game state and all subsystems */
void Game_Init(Game *_Game);

/* Update game logic (movement, collisions, combos) */
void Game_Update(Game *_Game);

/* Spawn food at random valid position */
void Game_SpawnFood(Game *_Game);

/* Check if snake head collides with food */
bool Game_CheckFoodCollision(Game *_Game);

/* Reset game to initial playing state */
void Game_Reset(Game *_Game);

/* Update explosion animation frame */
void Game_UpdateExplosion(Game *_Game, unsigned int _CurrentTime);

/* Trigger explosion effect at position */
void Game_TriggerExplosion(Game *_Game, Position _Pos, unsigned int _CurrentTime);

#endif
