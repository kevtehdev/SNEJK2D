#include "../include/game.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void Game_Init(Game *_Game)
{
    srand(time(NULL));

    _Game->state = GAME_MAIN_MENU;  // Start at main menu
    _Game->selectedBackground = 0;  // Cyberpunk Street
    _Game->currentSpeed = BASE_SPEED;
    _Game->lastMoveTime = 0;
    _Game->running = true;

    // Initialize combo system
    _Game->comboCount = 0;
    _Game->lastFoodTime = 0;
    _Game->comboMultiplier = 1.0f;

    // Initialize game over menu
    _Game->gameOverSelection = 0;  // Default to "Try Again"

    // Initialize explosion
    _Game->explosion.active = false;
    _Game->explosion.frame = 0;
    _Game->scoreSaved = false;

    // Initialize player name
    strncpy(_Game->playerName, "", sizeof(_Game->playerName) - 1);
    _Game->playerName[sizeof(_Game->playerName) - 1] = '\0';
    _Game->playerNameLen = strlen(_Game->playerName);
    _Game->isHighscore = false;

    // Initialize options
    _Game->optionsSelection = 0;
    Settings_Init(&_Game->settings);
    Settings_Load(&_Game->settings);

    Snake_Init(&_Game->snake);
    Game_SpawnFood(_Game);
}

void Game_SpawnFood(Game *_Game)
{
    bool valid = false;

    while (!valid)
    {
        // Spawn food within playable area (avoiding HUD border)
        int playableWidth = PLAYABLE_MAX_X - PLAYABLE_MIN_X + 1;
        int playableHeight = PLAYABLE_MAX_Y - PLAYABLE_MIN_Y + 1;

        _Game->food.x = PLAYABLE_MIN_X + (rand() % playableWidth);
        _Game->food.y = PLAYABLE_MIN_Y + (rand() % playableHeight);

        valid = true;

        // Check if food spawns on snake
        for (int i = 0; i < _Game->snake.length; i++)
        {
            if (_Game->food.x == _Game->snake.segments[i].x &&
                _Game->food.y == _Game->snake.segments[i].y)
            {
                valid = false;
                break;
            }
        }
    }
}

bool Game_CheckFoodCollision(Game *_Game)
{
    Position head = _Game->snake.segments[0];
    return (head.x == _Game->food.x && head.y == _Game->food.y);
}

/* Update combo multiplier based on combo count */
static void updateComboMultiplier(Game *_Game)
{
    if (_Game->comboCount >= COMBO_TIER_5)
        _Game->comboMultiplier = 5.0f;
    else if (_Game->comboCount >= COMBO_TIER_4)
        _Game->comboMultiplier = 3.0f;
    else if (_Game->comboCount >= COMBO_TIER_3)
        _Game->comboMultiplier = 2.0f;
    else if (_Game->comboCount >= COMBO_TIER_2)
        _Game->comboMultiplier = 1.5f;
    else
        _Game->comboMultiplier = 1.0f;
}

void Game_Update(Game *_Game)
{
    if (_Game->state != GAME_PLAYING)
        return;

    unsigned int currentTime = SDL_GetTicks();

    // Check combo timeout (dynamic based on speed - slower speed = longer timeout)
    if (_Game->comboCount > 0 && _Game->lastFoodTime > 0)
    {
        // Calculate dynamic timeout: scales from MIN to MAX based on current speed
        unsigned int dynamicTimeout = _Game->currentSpeed * 30;  // Base scaling
        if (dynamicTimeout < COMBO_TIMEOUT_MIN)
            dynamicTimeout = COMBO_TIMEOUT_MIN;  // Never shorter than min
        if (dynamicTimeout > COMBO_TIMEOUT_MAX)
            dynamicTimeout = COMBO_TIMEOUT_MAX;  // Never longer than max

        if (currentTime - _Game->lastFoodTime >= dynamicTimeout)
        {
            // Combo expired!
            _Game->comboCount = 0;
            _Game->comboMultiplier = 1.0f;
        }
    }

    // Check if it's time to move
    if (currentTime - _Game->lastMoveTime >= _Game->currentSpeed)
    {
        _Game->lastMoveTime = currentTime;

        // Calculate where the head would move to (preview next position)
        Position nextHead = _Game->snake.segments[0];

        // Consume direction from buffer if available (same logic as snake_move)
        Direction moveDir = _Game->snake.direction;
        if (_Game->snake.inputBuffer.count > 0)
        {
            moveDir = _Game->snake.inputBuffer.buffer[0];
        }

        switch (moveDir)
        {
        case DIR_UP:
            nextHead.y--;
            break;
        case DIR_DOWN:
            nextHead.y++;
            break;
        case DIR_LEFT:
            nextHead.x--;
            break;
        case DIR_RIGHT:
            nextHead.x++;
            break;
        }

        // Check if next position would collide with wall
        bool wouldHitWall = (nextHead.x < PLAYABLE_MIN_X || nextHead.x > PLAYABLE_MAX_X ||
                             nextHead.y < PLAYABLE_MIN_Y || nextHead.y > PLAYABLE_MAX_Y);

        // Check if next position would collide with self
        bool wouldHitSelf = false;
        for (int i = 1; i < _Game->snake.length; i++)
        {
            if (nextHead.x == _Game->snake.segments[i].x &&
                nextHead.y == _Game->snake.segments[i].y)
            {
                wouldHitSelf = true;
                break;
            }
        }

        // If collision detected, don't move - just die
        if (wouldHitWall || wouldHitSelf)
        {
            _Game->snake.alive = false;
            _Game->state = GAME_OVER;
            return;
        }

        // Safe to move now
        Snake_Move(&_Game->snake);

        // Check food collision
        if (Game_CheckFoodCollision(_Game))
        {
            // Increase combo
            _Game->comboCount++;
            _Game->lastFoodTime = currentTime;
            updateComboMultiplier(_Game);

            // Award points with multiplier
            int basePoints = 10;
            int points = (int)(basePoints * _Game->comboMultiplier);
            _Game->snake.score += points;

            Snake_Grow(&_Game->snake);
            Game_SpawnFood(_Game);

            // Increase speed (with cap at MIN_SPEED)
            _Game->currentSpeed -= SPEED_INCREASE;
            if (_Game->currentSpeed < MIN_SPEED)
            {
                _Game->currentSpeed = MIN_SPEED;  // Speed cap!
            }
        }
    }
}

void Game_Reset(Game *_Game)
{
    _Game->state = GAME_PLAYING;
    _Game->currentSpeed = BASE_SPEED;
    _Game->lastMoveTime = SDL_GetTicks();

    // Reset combo system
    _Game->comboCount = 0;
    _Game->lastFoodTime = 0;
    _Game->comboMultiplier = 1.0f;

    // Reset explosion
    _Game->explosion.active = false;
    _Game->scoreSaved = false;
    _Game->isHighscore = false;

    Snake_Init(&_Game->snake);
    Game_SpawnFood(_Game);
}

void Game_UpdateExplosion(Game *_Game, unsigned int _CurrentTime)
{
    if (!_Game->explosion.active)
        return;

    unsigned int elapsed = _CurrentTime - _Game->explosion.startTime;
    _Game->explosion.frame = elapsed / 100;  // 100ms per frame

    if (_Game->explosion.frame >= 7)
    {
        _Game->explosion.active = false;
    }
}

void Game_TriggerExplosion(Game *_Game, Position _Pos, unsigned int _CurrentTime)
{
    _Game->explosion.active = true;
    _Game->explosion.position = _Pos;
    _Game->explosion.frame = 0;
    _Game->explosion.startTime = _CurrentTime;
}
