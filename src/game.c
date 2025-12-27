#include "../include/game.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void Game_Init(Game *_Game)
{
    srand(time(NULL));

    _Game->state = GAME_MAIN_MENU;  // Start at main menu
    _Game->gameMode = MODE_CLASSIC;  // Default to classic mode
    _Game->modeSelection = 0;        // 0 = Classic
    _Game->selectedBackground = 0;   // Cyberpunk Street
    _Game->currentSpeed = BASE_SPEED;
    _Game->lastMoveTime = 0;
    _Game->running = true;

    // Initialize combo system
    _Game->comboCount = 0;
    _Game->lastFoodTime = 0;
    _Game->comboMultiplier = 1.0f;

    // Initialize game over menu
    _Game->gameOverSelection = 0;  // Default to "Try Again"

    // Initialize pause menu
    _Game->pauseMenuSelection = 0;  // Default to "Resume Game"

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

    // Initialize power-ups array
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        _Game->powerUps[i].active = false;
        _Game->powerUps[i].type = POWERUP_NONE;
    }
    _Game->powerUpStatus.hasShield = false;
    _Game->powerUpStatus.speedBoostActive = false;
    _Game->powerUpStatus.scoreMultiplierActive = false;
    _Game->powerUpStatus.normalSpeed = BASE_SPEED;

    // Initialize level system
    _Game->level = 1;
    _Game->xp = 0;
    _Game->xpToNextLevel = 100;  // 100 XP for level 2
    _Game->levelUpActive = false;
    _Game->levelUpStartTime = 0;

    Snake_Init(&_Game->snake);
    Game_SpawnFood(_Game);
}

void Game_SpawnFood(Game *_Game)
{
    bool valid = false;
    Position spawnPos;
    int playableWidth = PLAYABLE_MAX_X - PLAYABLE_MIN_X + 1;
    int playableHeight = PLAYABLE_MAX_Y - PLAYABLE_MIN_Y + 1;

    // Find valid position for regular food
    while (!valid)
    {
        spawnPos.x = PLAYABLE_MIN_X + (rand() % playableWidth);
        spawnPos.y = PLAYABLE_MIN_Y + (rand() % playableHeight);

        valid = true;

        // Check if food spawns on snake
        for (int i = 0; i < _Game->snake.length; i++)
        {
            if (spawnPos.x == _Game->snake.segments[i].x &&
                spawnPos.y == _Game->snake.segments[i].y)
            {
                valid = false;
                break;
            }
        }

        // Check if food spawns on existing power-ups
        if (valid && _Game->gameMode == MODE_POWERUP)
        {
            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                if (_Game->powerUps[i].active &&
                    spawnPos.x == _Game->powerUps[i].position.x &&
                    spawnPos.y == _Game->powerUps[i].position.y)
                {
                    valid = false;
                    break;
                }
            }
        }
    }

    // Set food position
    _Game->food.x = spawnPos.x;
    _Game->food.y = spawnPos.y;

    // In Power-Up mode, spawn multiple power-ups
    if (_Game->gameMode == MODE_POWERUP)
    {
        // Create array of power-up types for guaranteed variety
        PowerUpType powerUpPool[4] = {
            POWERUP_GOLDEN_APPLE,
            POWERUP_SHIELD,
            POWERUP_SPEED_BOOST,
            POWERUP_SCORE_MULTIPLIER
        };

        for (int p = 0; p < POWERUP_MODE_COUNT; p++)
        {
            // Find a free slot
            int slot = -1;
            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                if (!_Game->powerUps[i].active)
                {
                    slot = i;
                    break;
                }
            }

            if (slot == -1)
                continue; // No free slot

            // Pick random power-up type from pool (guaranteed valid)
            PowerUpType spawnType = powerUpPool[rand() % 4];

            // Find valid position for power-up
            valid = false;
            int attempts = 0;
            while (!valid && attempts < 50)
            {
                spawnPos.x = PLAYABLE_MIN_X + (rand() % playableWidth);
                spawnPos.y = PLAYABLE_MIN_Y + (rand() % playableHeight);

                valid = true;
                attempts++;

                // Check snake
                for (int i = 0; i < _Game->snake.length; i++)
                {
                    if (spawnPos.x == _Game->snake.segments[i].x &&
                        spawnPos.y == _Game->snake.segments[i].y)
                    {
                        valid = false;
                        break;
                    }
                }

                // Check food
                if (valid && spawnPos.x == _Game->food.x && spawnPos.y == _Game->food.y)
                    valid = false;

                // Check other power-ups
                if (valid)
                {
                    for (int i = 0; i < MAX_POWERUPS; i++)
                    {
                        if (_Game->powerUps[i].active &&
                            spawnPos.x == _Game->powerUps[i].position.x &&
                            spawnPos.y == _Game->powerUps[i].position.y)
                        {
                            valid = false;
                            break;
                        }
                    }
                }
            }

            if (valid)
            {
                _Game->powerUps[slot].active = true;
                _Game->powerUps[slot].type = spawnType;
                _Game->powerUps[slot].position = spawnPos;
                _Game->powerUps[slot].spawnTime = SDL_GetTicks();
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

    // Check power-up timers
    if (_Game->powerUpStatus.speedBoostActive && currentTime >= _Game->powerUpStatus.speedBoostEndTime)
    {
        // Speed boost expired, restore normal speed
        _Game->powerUpStatus.speedBoostActive = false;
        _Game->currentSpeed = _Game->powerUpStatus.normalSpeed;
    }

    if (_Game->powerUpStatus.scoreMultiplierActive && currentTime >= _Game->powerUpStatus.scoreMultiplierEndTime)
    {
        // Score multiplier expired
        _Game->powerUpStatus.scoreMultiplierActive = false;
    }

    // Check power-up decay (Power-Up mode only)
    if (_Game->gameMode == MODE_POWERUP)
    {
        for (int i = 0; i < MAX_POWERUPS; i++)
        {
            if (_Game->powerUps[i].active)
            {
                unsigned int age = currentTime - _Game->powerUps[i].spawnTime;
                if (age >= POWERUP_DECAY_TIME)
                {
                    // Power-up has decayed, remove it
                    _Game->powerUps[i].active = false;
                }
            }
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

        // If collision detected, check shield
        if (wouldHitWall || wouldHitSelf)
        {
            if (_Game->powerUpStatus.hasShield)
            {
                // Shield absorbs the hit!
                _Game->powerUpStatus.hasShield = false;
                // Don't move into collision, just consume shield and continue
                return;
            }
            else
            {
                _Game->snake.alive = false;
                _Game->state = GAME_OVER;
                return;
            }
        }

        // Safe to move now
        Snake_Move(&_Game->snake);

        // Check food collision
        if (Game_CheckFoodCollision(_Game))
        {
            // Base growth and combo
            int growthAmount = 1;
            int basePoints = 10;

            // Increase combo
            _Game->comboCount++;
            _Game->lastFoodTime = currentTime;
            updateComboMultiplier(_Game);

            // Award points with multipliers
            float totalMultiplier = _Game->comboMultiplier;
            if (_Game->powerUpStatus.scoreMultiplierActive)
            {
                totalMultiplier *= 10.0f; // 10x score multiplier
            }
            int points = (int)(basePoints * totalMultiplier);
            _Game->snake.score += points;

            // Grow snake
            for (int i = 0; i < growthAmount; i++)
            {
                Snake_Grow(&_Game->snake);
            }

            Game_SpawnFood(_Game);

            // Add XP in Power-Up mode
            if (_Game->gameMode == MODE_POWERUP)
            {
                _Game->xp += 10; // 10 XP per food eaten

                // Check for level up
                while (_Game->xp >= _Game->xpToNextLevel)
                {
                    _Game->level++;
                    _Game->xp -= _Game->xpToNextLevel;
                    _Game->xpToNextLevel = 100 + (_Game->level - 1) * 50; // 100, 150, 200, 250, etc.
                    _Game->levelUpActive = true;
                    _Game->levelUpStartTime = currentTime;
                }
            }

            // Increase speed (with cap at MIN_SPEED) - only if no speed boost active
            if (!_Game->powerUpStatus.speedBoostActive)
            {
                _Game->currentSpeed -= SPEED_INCREASE;
                if (_Game->currentSpeed < MIN_SPEED)
                {
                    _Game->currentSpeed = MIN_SPEED; // Speed cap!
                }
            }
        }

        // Check power-up collisions (Power-Up mode only)
        if (_Game->gameMode == MODE_POWERUP)
        {
            Position head = _Game->snake.segments[0];

            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                if (!_Game->powerUps[i].active)
                    continue;

                if (head.x == _Game->powerUps[i].position.x &&
                    head.y == _Game->powerUps[i].position.y)
                {
                    // Power-up collected!
                    PowerUpType type = _Game->powerUps[i].type;
                    _Game->powerUps[i].active = false; // Consume power-up

                    // All power-ups refresh combo timer and give combo boost
                    _Game->lastFoodTime = currentTime;

                    switch (type)
                    {
                    case POWERUP_GOLDEN_APPLE:
                        // +3 length, +100 points × combo, +5 combo
                        for (int g = 0; g < 3; g++)
                            Snake_Grow(&_Game->snake);

                        float totalMult = _Game->comboMultiplier;
                        if (_Game->powerUpStatus.scoreMultiplierActive)
                            totalMult *= 10.0f;

                        _Game->snake.score += (int)(100 * totalMult);
                        _Game->comboCount += 5;
                        updateComboMultiplier(_Game);
                        break;

                    case POWERUP_SHIELD:
                        // +30 points × combo, +2 combo
                        totalMult = _Game->comboMultiplier;
                        if (_Game->powerUpStatus.scoreMultiplierActive)
                            totalMult *= 10.0f;

                        _Game->powerUpStatus.hasShield = true;
                        _Game->snake.score += (int)(30 * totalMult);
                        _Game->comboCount += 2;
                        updateComboMultiplier(_Game);
                        break;

                    case POWERUP_SCORE_MULTIPLIER:
                        // +50 points × combo, +3 combo, activate 10x multiplier
                        totalMult = _Game->comboMultiplier;
                        // Don't apply to itself, just combo

                        _Game->powerUpStatus.scoreMultiplierActive = true;
                        _Game->powerUpStatus.scoreMultiplierEndTime = currentTime + POWERUP_SCORE_MULT_DURATION;
                        _Game->snake.score += (int)(50 * totalMult);
                        _Game->comboCount += 3;
                        updateComboMultiplier(_Game);
                        break;

                    case POWERUP_SPEED_BOOST:
                        // +20 points × combo, +1 combo, temporary speed boost
                        totalMult = _Game->comboMultiplier;
                        if (_Game->powerUpStatus.scoreMultiplierActive)
                            totalMult *= 10.0f;

                        if (!_Game->powerUpStatus.speedBoostActive)
                        {
                            _Game->powerUpStatus.normalSpeed = _Game->currentSpeed;
                        }
                        _Game->powerUpStatus.speedBoostActive = true;
                        _Game->powerUpStatus.speedBoostEndTime = currentTime + POWERUP_SPEED_BOOST_DURATION;

                        // More moderate speed boost - 1.5x instead of 2x
                        _Game->currentSpeed = (_Game->currentSpeed * 2) / 3;
                        if (_Game->currentSpeed < 40)
                            _Game->currentSpeed = 40; // More reasonable minimum

                        _Game->snake.score += (int)(20 * totalMult);
                        _Game->comboCount += 1;
                        updateComboMultiplier(_Game);
                        break;

                    default:
                        break;
                    }
                }
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

    // Reset power-ups
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        _Game->powerUps[i].active = false;
        _Game->powerUps[i].type = POWERUP_NONE;
    }
    _Game->powerUpStatus.hasShield = false;
    _Game->powerUpStatus.speedBoostActive = false;
    _Game->powerUpStatus.scoreMultiplierActive = false;
    _Game->powerUpStatus.normalSpeed = BASE_SPEED;

    // Reset level system
    _Game->level = 1;
    _Game->xp = 0;
    _Game->xpToNextLevel = 100;
    _Game->levelUpActive = false;
    _Game->levelUpStartTime = 0;

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
