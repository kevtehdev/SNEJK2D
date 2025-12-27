#ifndef __GAME_H_
#define __GAME_H_

#include "config.h"
#include "snake.h"
#include "settings.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum {
    GAME_MAIN_MENU,      // Singleplayer / Multiplayer choice
    GAME_MODE_SELECT,    // Classic vs Power-Up mode selection
    GAME_NAME_INPUT,     // Name input before background selection
    GAME_MENU,           // Background selection (singleplayer)
    GAME_PLAYING,        // Singleplayer gameplay
    GAME_PAUSED,
    GAME_OVER,
    GAME_SCOREBOARD,     // Scoreboard view
    GAME_OPTIONS,        // Settings menu
    GAME_MULTIPLAYER     // Multiplayer mode (separate state)
} GameState;

typedef enum {
    MODE_CLASSIC,        // Original game without power-ups
    MODE_POWERUP         // Power-up mode with multiple simultaneous power-ups
} GameMode;

// Explosion effect
typedef struct {
    bool active;
    Position position;
    int frame;
    unsigned int startTime;
} Explosion;

// Power-up types
typedef enum {
    POWERUP_NONE = 0,
    POWERUP_GOLDEN_APPLE,
    POWERUP_SPEED_BOOST,
    POWERUP_SHIELD,
    POWERUP_SCORE_MULTIPLIER
} PowerUpType;

// Power-up item
typedef struct {
    bool active;
    PowerUpType type;
    Position position;
    unsigned int spawnTime;
} PowerUp;

// Active power-up effects
typedef struct {
    bool hasShield;
    bool speedBoostActive;
    bool scoreMultiplierActive;
    unsigned int speedBoostEndTime;
    unsigned int scoreMultiplierEndTime;
    int normalSpeed;  // Store normal speed during speed effects
} PowerUpStatus;

typedef struct {
    Snake snake;
    Position food;
    GameState state;
    GameMode gameMode;           // Classic or Power-Up mode
    int modeSelection;           // 0 = Classic, 1 = Power-Up
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

    // Pause menu
    int pauseMenuSelection;      // 0 = Resume Game, 1 = Exit to Main Menu

    // Explosion effect
    Explosion explosion;
    bool scoreSaved;             // Track if score was saved to prevent duplicates

    // Player name
    char playerName[16];
    int playerNameLen;

    // Highscore flag
    bool isHighscore;            // True if current score is a highscore

    // Options menu
    int optionsSelection;        // 0-4: menu music, game music, grid alpha, brightness, reset
    Settings settings;

    // Power-ups (multiple in Power-Up mode)
    #define MAX_POWERUPS 3  // Reduced from 5 to 3 for better gameplay
    PowerUp powerUps[MAX_POWERUPS];
    PowerUpStatus powerUpStatus;

    // Level system for Power-Up mode
    int level;                   // Current level (starts at 1)
    int xp;                      // Current XP
    int xpToNextLevel;          // XP needed for next level
    bool levelUpActive;         // True when level-up effect is playing
    unsigned int levelUpStartTime; // When level-up effect started
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
