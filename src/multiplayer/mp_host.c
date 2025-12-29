#include "../../include/multiplayer/mp_host.h"
#include "../../include/multiplayer/mp_network.h"
#include "../../include/multiplayer/multiplayer.h"
#include "../../include/config.h"
#include "../../include/entities/snake.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Host operations module - server-side multiplayer logic */

/* Forward declaration - defined in mp_core.c */
extern void mpapiEventCallback(const char *event, int64_t messageId, const char *clientId, json_t *data, void *context);

/* Host a new game session */
int MP_Host_Create(MultiplayerContext *_Ctx, const char *_PlayerName)
{
    if (!_Ctx || !_Ctx->api)
        return 0;

    printf("Hosting game: %s...\n", _Ctx->roomName);

    _Ctx->isHost = true;
    _Ctx->state = MP_STATE_HOSTING;

    // Set game mode based on selection
    _Ctx->gameMode = (_Ctx->modeSelection == 0) ? MP_MODE_TURN_BATTLE : MP_MODE_1VS1;

    // Map names with de_ prefix
    const char *mapNames[] = {"de_cyberpunk", "de_forest", "de_underwater", "de_mountain", "de_country", "de_city"};
    const char *modeNames[] = {"TURN BATTLE", "1VS1"};

    printf("DEBUG: selectedBackground = %d (%s), gameMode = %s\n",
           _Ctx->selectedBackground,
           mapNames[_Ctx->selectedBackground % 6],
           modeNames[_Ctx->gameMode]);

    // Format room name with student number, map ID and game mode encoded in the name
    // Format: [76] roomname |m2| |g1V| where 2 is map ID and 1V is game mode (1V=1VS1, TB=TURN BATTLE)
    const char *modeCode = (_Ctx->gameMode == MP_MODE_1VS1) ? "1V" : "TB";
    char nameWithId[128];
    snprintf(nameWithId, sizeof(nameWithId), "[%s] %s |m%d| |g%s|",
             STUDENT_NUMBER, _Ctx->roomName, _Ctx->selectedBackground, modeCode);

    json_t *hostData = json_object();
    json_object_set_new(hostData, "name", json_string(nameWithId));
    json_object_set_new(hostData, "maxPlayers", json_integer(2));                 // 1v1 game
    json_object_set_new(hostData, "map", json_integer(_Ctx->selectedBackground)); // Include map ID

    char *sessionId = NULL;
    char *clientId = NULL;

    int rc = mpapi_host(_Ctx->api, hostData, &sessionId, &clientId, NULL);
    json_decref(hostData);

    if (rc != MPAPI_OK)
    {
        printf("Failed to host: error %d\n", rc);
        snprintf(_Ctx->errorMessage, sizeof(_Ctx->errorMessage), "Failed to host: error %d", rc);
        _Ctx->state = MP_STATE_DISCONNECTED;
        return 0;
    }

    // Store session info
    if (sessionId)
    {
        strncpy(_Ctx->sessionId, sessionId, sizeof(_Ctx->sessionId) - 1);
        free(sessionId);
    }

    if (clientId)
    {
        strncpy(_Ctx->ourClientId, clientId, sizeof(_Ctx->ourClientId) - 1);
        _Ctx->ourClientId[sizeof(_Ctx->ourClientId) - 1] = '\0';
        free(clientId);
    }

    // Register event listener
    _Ctx->listenerId = mpapi_listen(_Ctx->api, mpapiEventCallback, _Ctx);
    if (_Ctx->listenerId < 0)
    {
        printf("Failed to register listener\n");
        return 0;
    }

    // Host auto-joins as player 0
    _Ctx->players[0].joined = true;
    _Ctx->players[0].isLocal = true;
    _Ctx->players[0].ready = false; // Explicitly set ready to false
    strncpy(_Ctx->players[0].clientId, _Ctx->ourClientId, sizeof(_Ctx->players[0].clientId) - 1);
    _Ctx->players[0].clientId[sizeof(_Ctx->players[0].clientId) - 1] = '\0';
    strncpy(_Ctx->players[0].name, _PlayerName, sizeof(_Ctx->players[0].name) - 1);
    _Ctx->players[0].name[sizeof(_Ctx->players[0].name) - 1] = '\0';
    _Ctx->localPlayerIndex = 0;

    // Initialize snake
    Snake_Init(&_Ctx->players[0].snake);
    _Ctx->players[0].snake.segments[0].x = GRID_WIDTH / 4;
    _Ctx->players[0].snake.segments[0].y = GRID_HEIGHT / 2;
    for (int i = 1; i < _Ctx->players[0].snake.length; i++)
    {
        _Ctx->players[0].snake.segments[i].x = _Ctx->players[0].snake.segments[0].x - i;
        _Ctx->players[0].snake.segments[i].y = _Ctx->players[0].snake.segments[0].y;
    }
    _Ctx->players[0].alive = false; // Not alive until game starts

    _Ctx->state = MP_STATE_LOBBY;
    printf("Session created: %s\n", _Ctx->sessionId);
    printf("✓ Waiting for Player 2 to join...\n");
    printf("✓ Press SPACE to toggle ready, ENTER to start when both ready\n");

    return 1;
}

/* Update host game state (called every frame) */
void MP_Host_Update(MultiplayerContext *_Ctx, unsigned int _CurrentTime)
{
    if (!_Ctx || !_Ctx->isHost)
        return;

    if (_Ctx->state != MP_STATE_PLAYING)
        return;

    // High-frequency state broadcast (every 16ms for minimal input lag)
    static unsigned int lastBroadcast = 0;
    if (_CurrentTime - lastBroadcast >= 16)
    {
        MP_Host_BroadcastState(_Ctx);
        lastBroadcast = _CurrentTime;
    }

    // Check if it's time to move
    if (_CurrentTime - _Ctx->lastMoveTime >= _Ctx->currentSpeed)
    {
        _Ctx->lastMoveTime = _CurrentTime;

        // Move all players' snakes
        for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
        {
            if (!_Ctx->players[i].joined || !_Ctx->players[i].alive)
                continue;

            // Calculate where the head would move to (preview next position)
            Position nextHead = _Ctx->players[i].snake.segments[0];

            // Consume direction from buffer if available (same logic as Snake_Move)
            Direction moveDir = _Ctx->players[i].snake.direction;
            if (_Ctx->players[i].snake.inputBuffer.count > 0)
            {
                moveDir = _Ctx->players[i].snake.inputBuffer.buffer[0];
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
            for (int k = 1; k < _Ctx->players[i].snake.length; k++)
            {
                if (nextHead.x == _Ctx->players[i].snake.segments[k].x &&
                    nextHead.y == _Ctx->players[i].snake.segments[k].y)
                {
                    wouldHitSelf = true;
                    break;
                }
            }

            // Check collision with other players
            bool wouldHitOther = false;
            for (int j = 0; j < MAX_MULTIPLAYER_PLAYERS; j++)
            {
                if (i == j || !_Ctx->players[j].joined || !_Ctx->players[j].alive)
                    continue;

                for (int k = 0; k < _Ctx->players[j].snake.length; k++)
                {
                    if (nextHead.x == _Ctx->players[j].snake.segments[k].x &&
                        nextHead.y == _Ctx->players[j].snake.segments[k].y)
                    {
                        wouldHitOther = true;
                        break;
                    }
                }
                if (wouldHitOther)
                    break;
            }

            // If collision detected, don't move - just die
            if (wouldHitWall || wouldHitSelf || wouldHitOther)
            {
                _Ctx->players[i].alive = false;
                continue;
            }

            // Safe to move now
            Snake_Move(&_Ctx->players[i].snake);

            // Check food collision (check both food items)
            Position head = _Ctx->players[i].snake.segments[0];
            for (int foodIdx = 0; foodIdx < 2; foodIdx++)
            {
                if (head.x == _Ctx->food[foodIdx].x && head.y == _Ctx->food[foodIdx].y)
                {
                    // Update combo system
                    _Ctx->players[i].comboCount++;
                    _Ctx->players[i].lastFoodTime = _CurrentTime;

                    // Update multiplier based on combo
                    if (_Ctx->players[i].comboCount >= COMBO_TIER_5)
                        _Ctx->players[i].comboMultiplier = 5.0f;
                    else if (_Ctx->players[i].comboCount >= COMBO_TIER_4)
                        _Ctx->players[i].comboMultiplier = 3.0f;
                    else if (_Ctx->players[i].comboCount >= COMBO_TIER_3)
                        _Ctx->players[i].comboMultiplier = 2.0f;
                    else if (_Ctx->players[i].comboCount >= COMBO_TIER_2)
                        _Ctx->players[i].comboMultiplier = 1.5f;
                    else
                        _Ctx->players[i].comboMultiplier = 1.0f;

                    // Award points with multiplier
                    int basePoints = 10;
                    int points = (int)(basePoints * _Ctx->players[i].comboMultiplier);
                    _Ctx->players[i].score += points;

                    Snake_Grow(&_Ctx->players[i].snake);

                    // Spawn new food within playable area (avoiding HUD border)
                    bool valid = false;
                    while (!valid)
                    {
                        int playableWidth = PLAYABLE_MAX_X - PLAYABLE_MIN_X + 1;
                        int playableHeight = PLAYABLE_MAX_Y - PLAYABLE_MIN_Y + 1;

                        _Ctx->food[foodIdx].x = PLAYABLE_MIN_X + (rand() % playableWidth);
                        _Ctx->food[foodIdx].y = PLAYABLE_MIN_Y + (rand() % playableHeight);

                        valid = true;
                        // Check against all snakes
                        for (int j = 0; j < MAX_MULTIPLAYER_PLAYERS; j++)
                        {
                            if (!_Ctx->players[j].joined)
                                continue;

                            for (int k = 0; k < _Ctx->players[j].snake.length; k++)
                            {
                                if (_Ctx->food[foodIdx].x == _Ctx->players[j].snake.segments[k].x &&
                                    _Ctx->food[foodIdx].y == _Ctx->players[j].snake.segments[k].y)
                                {
                                    valid = false;
                                    break;
                                }
                            }
                            if (!valid)
                                break;
                        }

                        // Check against other food item
                        if (valid)
                        {
                            int otherIdx = 1 - foodIdx;
                            if (_Ctx->food[foodIdx].x == _Ctx->food[otherIdx].x &&
                                _Ctx->food[foodIdx].y == _Ctx->food[otherIdx].y)
                            {
                                valid = false;
                            }
                        }
                    }

                    // Increase speed
                    _Ctx->currentSpeed -= SPEED_INCREASE;
                    if (_Ctx->currentSpeed < MIN_SPEED)
                        _Ctx->currentSpeed = MIN_SPEED;

                    break; // Only eat one food per frame
                }
            }

            // Check combo timeout (dynamic based on speed)
            if (_Ctx->players[i].comboCount > 0 && _Ctx->players[i].lastFoodTime > 0)
            {
                unsigned int dynamicTimeout = _Ctx->currentSpeed * 30; // Base scaling
                if (dynamicTimeout < COMBO_TIMEOUT_MIN)
                    dynamicTimeout = COMBO_TIMEOUT_MIN; // Never shorter than min
                if (dynamicTimeout > COMBO_TIMEOUT_MAX)
                    dynamicTimeout = COMBO_TIMEOUT_MAX; // Never longer than max

                if (_CurrentTime - _Ctx->players[i].lastFoodTime >= dynamicTimeout)
                {
                    // Combo expired!
                    _Ctx->players[i].comboCount = 0;
                    _Ctx->players[i].comboMultiplier = 1.0f;
                }
            }
        }

        // Check game over
        int aliveCount = 0;
        for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
        {
            if (_Ctx->players[i].joined && _Ctx->players[i].alive)
                aliveCount++;
        }

        if (aliveCount <= 1)
        {
            _Ctx->state = MP_STATE_GAME_OVER;
            // Broadcast game over state immediately to all clients
            MP_Host_BroadcastState(_Ctx);
        }
    }
}

/* Broadcast current state to all clients */
void MP_Host_BroadcastState(MultiplayerContext *_Ctx)
{
    if (!_Ctx || !_Ctx->api)
        return;

    json_t *state = MP_Network_SerializeState(_Ctx);
    json_t *message = json_object();
    json_object_set_new(message, "type", json_string("game_state"));
    json_object_set_new(message, "state", state);

    int rc = mpapi_game(_Ctx->api, message, NULL);
    if (rc != MPAPI_OK)
    {
        printf("Failed to broadcast state: error %d\n", rc);
    }

    json_decref(message);
}
