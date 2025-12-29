/*
 * mp_game.c - Game state management for multiplayer
 * Part of SNEJK2D multiplayer refactoring (Phase 4)
 *
 * Functions:
 * - Start/restart game
 * - Countdown management
 * - Ready status checking
 * - Player utilities
 */

#include "../../include/multiplayer/multiplayer.h"
#include "../../include/multiplayer/mp_game.h"
#include "../../include/entities/snake.h"
#include <SDL2/SDL.h>
#include <stdio.h>

/*
 * Start the multiplayer game (host only)
 */
void MP_Game_Start(MultiplayerContext *_Ctx)
{
    if (!_Ctx || !_Ctx->isHost)
        return;

    json_t *message = json_object();
    json_object_set_new(message, "type", json_string("start_game"));
    json_object_set_new(message, "gameMode", json_integer(_Ctx->gameMode));

    int rc = mpapi_game(_Ctx->api, message, NULL);
    if (rc != MPAPI_OK)
    {
        printf("Failed to send start command: error %d\n", rc);
    }

    json_decref(message);

    // For turn battle, go to ready-up state instead of countdown
    if (_Ctx->gameMode == MP_MODE_TURN_BATTLE)
    {
        _Ctx->state = MP_STATE_READY_UP;
        printf("Starting TURN BATTLE mode - players ready up\n");
    }
    else
    {
        _Ctx->state = MP_STATE_COUNTDOWN;
        _Ctx->countdownStart = SDL_GetTicks();
        _Ctx->gameStartTime = _Ctx->countdownStart + 3000;
    }
}

/*
 * Restart game after game over (host only)
 */
void MP_Game_Restart(MultiplayerContext *_Ctx)
{
    if (!_Ctx || !_Ctx->isHost)
        return;

    // Reset game state
    _Ctx->currentSpeed = BASE_SPEED;
    _Ctx->gameOverSelection = 0;

    // Reset both players
    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        if (_Ctx->players[i].joined)
        {
            _Ctx->players[i].alive = true;
            _Ctx->players[i].score = 0;
            _Ctx->players[i].comboCount = 0;
            _Ctx->players[i].comboMultiplier = 1.0f;
            _Ctx->players[i].lastFoodTime = 0;

            // Reinitialize snake
            Snake_Init(&_Ctx->players[i].snake);

            // Position players on opposite sides
            if (i == 0)
            {
                _Ctx->players[i].snake.segments[0].x = GRID_WIDTH / 4;
                _Ctx->players[i].snake.segments[0].y = GRID_HEIGHT / 2;
            }
            else
            {
                _Ctx->players[i].snake.segments[0].x = (GRID_WIDTH * 3) / 4;
                _Ctx->players[i].snake.segments[0].y = GRID_HEIGHT / 2;
            }

            for (int j = 1; j < _Ctx->players[i].snake.length; j++)
            {
                _Ctx->players[i].snake.segments[j].x = _Ctx->players[i].snake.segments[0].x - j;
                _Ctx->players[i].snake.segments[j].y = _Ctx->players[i].snake.segments[0].y;
            }
        }
    }

    // Respawn food
    _Ctx->food[0].x = PLAYABLE_MIN_X + 5;
    _Ctx->food[0].y = PLAYABLE_MIN_Y + 5;
    _Ctx->food[1].x = PLAYABLE_MAX_X - 5;
    _Ctx->food[1].y = PLAYABLE_MAX_Y - 5;

    // Send restart command to client
    json_t *message = json_object();
    json_object_set_new(message, "type", json_string("restart_game"));

    int rc = mpapi_game(_Ctx->api, message, NULL);
    if (rc != MPAPI_OK)
    {
        printf("Failed to send restart command: error %d\n", rc);
    }

    json_decref(message);

    // Go to countdown
    _Ctx->state = MP_STATE_COUNTDOWN;
    _Ctx->countdownStart = SDL_GetTicks();
    _Ctx->gameStartTime = _Ctx->countdownStart + 3000;
}

/*
 * Update countdown and initialize game when countdown finishes
 */
void MP_Game_UpdateCountdown(MultiplayerContext *_Ctx, unsigned int _CurrentTime)
{
    if (!_Ctx || _Ctx->state != MP_STATE_COUNTDOWN)
        return;

    // Check if countdown finished
    if (_CurrentTime >= _Ctx->gameStartTime)
    {
        // For turn battle, go to TURN_PLAYING state
        if (_Ctx->gameMode == MP_MODE_TURN_BATTLE)
        {
            _Ctx->state = MP_STATE_TURN_PLAYING;
            printf("Countdown finished - starting turn battle attempt\n");
            return;
        }

        // For 1VS1 mode, go to PLAYING state
        _Ctx->state = MP_STATE_PLAYING;
        _Ctx->lastMoveTime = _CurrentTime;

        // Initialize snakes for both players
        for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
        {
            if (_Ctx->players[i].joined)
            {
                Snake_Init(&_Ctx->players[i].snake);

                // Position snakes on opposite sides
                if (i == 0)
                {
                    _Ctx->players[i].snake.segments[0].x = GRID_WIDTH / 4;
                }
                else
                {
                    _Ctx->players[i].snake.segments[0].x = (GRID_WIDTH * 3) / 4;
                    _Ctx->players[i].snake.direction = DIR_LEFT;
                    _Ctx->players[i].snake.nextDirection = DIR_LEFT;
                }

                _Ctx->players[i].snake.segments[0].y = GRID_HEIGHT / 2;
                for (int j = 1; j < _Ctx->players[i].snake.length; j++)
                {
                    _Ctx->players[i].snake.segments[j] = _Ctx->players[i].snake.segments[0];
                    _Ctx->players[i].snake.segments[j].x += (i == 0 ? -j : j);
                }

                _Ctx->players[i].alive = true;

                // Initialize combo system
                _Ctx->players[i].comboCount = 0;
                _Ctx->players[i].lastFoodTime = 0;
                _Ctx->players[i].comboMultiplier = 1.0f;
                _Ctx->players[i].score = 0;
            }
        }

        // Spawn two food items for more intense gameplay
        _Ctx->food[0].x = PLAYABLE_MIN_X + 5;
        _Ctx->food[0].y = PLAYABLE_MIN_Y + 5;
        _Ctx->food[1].x = PLAYABLE_MAX_X - 5;
        _Ctx->food[1].y = PLAYABLE_MAX_Y - 5;
    }
}

/*
 * Check if all players are ready
 */
bool MP_Game_AllReady(MultiplayerContext *_Ctx)
{
    if (!_Ctx)
        return false;

    int joinedCount = 0;
    int readyCount = 0;

    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        if (_Ctx->players[i].joined)
        {
            joinedCount++;
            if (_Ctx->players[i].ready)
                readyCount++;
        }
    }

    return joinedCount >= 2 && readyCount == joinedCount;
}

/*
 * Get the local player's index
 * Returns: player index (0-3) or -1 if not found
 */
int MP_Game_GetLocalPlayerIndex(MultiplayerContext *_Ctx)
{
    return _Ctx ? _Ctx->localPlayerIndex : -1;
}
