#include "../../include/multiplayer/multiplayer.h"
#include "../../include/multiplayer/mp_network.h"
#include "../../include/multiplayer/mp_host.h"
#include "../../include/multiplayer/mp_client.h"
#include "../../include/multiplayer/mp_game.h"
#include "../../include/multiplayer/mp_turn_battle.h"
#include "../../include/audio/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// mpapi event callback
void mpapiEventCallback(const char *event, int64_t messageId, const char *clientId, json_t *data, void *context)
{
    MultiplayerContext *ctx = (MultiplayerContext *)context;

    printf("mpapi event: %s from %s\n", event, clientId ? clientId : "NULL");

    if (strcmp(event, "joined") == 0)
    {
        // Player joined
        printf("Player joined: %s\n", clientId ? clientId : "unknown");

        // Find empty slot
        int joinedSlot = -1;
        for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
        {
            if (!ctx->players[i].joined)
            {
                joinedSlot = i;
                ctx->players[i].joined = true;
                if (clientId)
                {
                    strncpy(ctx->players[i].clientId, clientId, sizeof(ctx->players[i].clientId) - 1);
                    ctx->players[i].clientId[sizeof(ctx->players[i].clientId) - 1] = '\0';
                }
                ctx->players[i].isLocal = (clientId && strcmp(clientId, ctx->ourClientId) == 0);

                // If this is our own client, set localPlayerIndex
                if (ctx->players[i].isLocal && ctx->localPlayerIndex < 0)
                {
                    ctx->localPlayerIndex = i;
                    printf("✓ Set localPlayerIndex to %d (our client)\n", i);
                }

                // Get player name from data
                if (data)
                {
                    json_t *nameObj = json_object_get(data, "name");
                    if (nameObj && json_is_string(nameObj))
                    {
                        const char *name = json_string_value(nameObj);
                        strncpy(ctx->players[i].name, name, sizeof(ctx->players[i].name) - 1);
                        ctx->players[i].name[sizeof(ctx->players[i].name) - 1] = '\0';
                        printf("✓ Player %d name: %s\n", i, name);
                    }
                }

                // Initialize snake for the new player
                Snake_Init(&ctx->players[i].snake);
                if (i == 0)
                {
                    // Player 0 (host) starts on left
                    ctx->players[i].snake.segments[0].x = GRID_WIDTH / 4;
                }
                else
                {
                    // Player 1 (joiner) starts on right
                    ctx->players[i].snake.segments[0].x = (3 * GRID_WIDTH) / 4;
                }
                ctx->players[i].snake.segments[0].y = GRID_HEIGHT / 2;
                for (int j = 1; j < ctx->players[i].snake.length; j++)
                {
                    ctx->players[i].snake.segments[j] = ctx->players[i].snake.segments[0];
                }
                ctx->players[i].alive = false;

                break;
            }
        }

        // If we're host, broadcast player list to all clients
        if (ctx->isHost && joinedSlot >= 0)
        {
            printf("✓ Broadcasting player list to all clients\n");
            json_t *syncMsg = json_object();
            json_object_set_new(syncMsg, "type", json_string("player_sync"));

            json_t *playersArray = json_array();
            for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
            {
                if (ctx->players[i].joined)
                {
                    json_t *playerObj = json_object();
                    json_object_set_new(playerObj, "clientId", json_string(ctx->players[i].clientId));
                    json_object_set_new(playerObj, "name", json_string(ctx->players[i].name));
                    json_object_set_new(playerObj, "ready", json_boolean(ctx->players[i].ready));
                    json_array_append_new(playersArray, playerObj);
                }
            }
            json_object_set_new(syncMsg, "players", playersArray);

            mpapi_game(ctx->api, syncMsg, NULL);
            json_decref(syncMsg);
        }
    }
    else if (strcmp(event, "leaved") == 0)
    {
        // Player left
        printf("Player left: %s\n", clientId ? clientId : "unknown");

        for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
        {
            if (clientId && strcmp(ctx->players[i].clientId, clientId) == 0)
            {
                memset(&ctx->players[i], 0, sizeof(MultiplayerPlayer));
                break;
            }
        }
    }
    else if (strcmp(event, "game") == 0 && data)
    {
        // Game message
        const char *msgType = json_string_value(json_object_get(data, "type"));
        if (!msgType)
            return;

        if (strcmp(msgType, "game_state") == 0)
        {
            // State update from host
            json_t *state = json_object_get(data, "state");
            if (state)
            {
                Multiplayer_DeserializeState(ctx, state);
            }
        }
        else if (strcmp(msgType, "input") == 0 && ctx->isHost)
        {
            // Client input (host only)
            const char *fromClient = json_string_value(json_object_get(data, "clientId"));
            int dir = json_integer_value(json_object_get(data, "direction"));

            if (fromClient)
            {
                // Find player and update direction
                for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
                {
                    if (ctx->players[i].joined && strcmp(ctx->players[i].clientId, fromClient) == 0)
                    {
                        Snake_SetDirection(&ctx->players[i].snake, (Direction)dir);
                        break;
                    }
                }
            }
        }
        else if (strcmp(msgType, "player_sync") == 0)
        {
            // Host is broadcasting player list (when someone joins/leaves)
            json_t *playersArray = json_object_get(data, "players");
            if (json_is_array(playersArray))
            {
                printf("✓ Received player sync from host\n");
                size_t numPlayers = json_array_size(playersArray);

                // Clear current players
                for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
                {
                    if (!ctx->players[i].isLocal)
                    {
                        ctx->players[i].joined = false;
                    }
                }

                // Update with server data
                for (size_t i = 0; i < numPlayers && i < MAX_MULTIPLAYER_PLAYERS; i++)
                {
                    json_t *playerObj = json_array_get(playersArray, i);
                    if (json_is_object(playerObj))
                    {
                        const char *cid = json_string_value(json_object_get(playerObj, "clientId"));
                        const char *name = json_string_value(json_object_get(playerObj, "name"));
                        int ready = json_boolean_value(json_object_get(playerObj, "ready"));

                        // Find this player by clientId
                        int slot = -1;
                        for (int j = 0; j < MAX_MULTIPLAYER_PLAYERS; j++)
                        {
                            if (ctx->players[j].joined && strcmp(ctx->players[j].clientId, cid) == 0)
                            {
                                slot = j;
                                break;
                            }
                        }

                        // If not found, find empty slot
                        if (slot < 0)
                        {
                            for (int j = 0; j < MAX_MULTIPLAYER_PLAYERS; j++)
                            {
                                if (!ctx->players[j].joined)
                                {
                                    slot = j;
                                    break;
                                }
                            }
                        }

                        if (slot >= 0 && cid)
                        {
                            ctx->players[slot].joined = true;
                            ctx->players[slot].ready = ready;
                            strncpy(ctx->players[slot].clientId, cid, sizeof(ctx->players[slot].clientId) - 1);
                            ctx->players[slot].clientId[sizeof(ctx->players[slot].clientId) - 1] = '\0';
                            if (name)
                            {
                                strncpy(ctx->players[slot].name, name, sizeof(ctx->players[slot].name) - 1);
                                ctx->players[slot].name[sizeof(ctx->players[slot].name) - 1] = '\0';
                            }

                            // Update isLocal
                            ctx->players[slot].isLocal = (strcmp(cid, ctx->ourClientId) == 0);
                            if (ctx->players[slot].isLocal && ctx->localPlayerIndex < 0)
                            {
                                ctx->localPlayerIndex = slot;
                            }

                            printf("✓ Player %d: %s (ready=%d)\n", slot, ctx->players[slot].name, ready);
                        }
                    }
                }
            }
        }
        else if (strcmp(msgType, "ready") == 0)
        {
            // Player ready status
            int ready = json_boolean_value(json_object_get(data, "ready"));

            if (clientId)
            {
                for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
                {
                    if (ctx->players[i].joined && strcmp(ctx->players[i].clientId, clientId) == 0)
                    {
                        ctx->players[i].ready = ready;
                        break;
                    }
                }
            }
        }
        else if (strcmp(msgType, "mode_selected") == 0)
        {
            // Host selected game mode (clients only receive this)
            json_t *gameModeObj = json_object_get(data, "gameMode");
            if (gameModeObj && json_is_integer(gameModeObj))
            {
                ctx->gameMode = (MultiplayerGameMode)json_integer_value(gameModeObj);
                printf("Host selected game mode: %s\n",
                       ctx->gameMode == MP_MODE_1VS1 ? "1VS1" : "TURN BATTLE");
            }
        }
        else if (strcmp(msgType, "start_game") == 0)
        {
            // Game starting
            json_t *gameModeObj = json_object_get(data, "gameMode");
            if (gameModeObj && json_is_integer(gameModeObj))
            {
                ctx->gameMode = (MultiplayerGameMode)json_integer_value(gameModeObj);
            }

            if (ctx->gameMode == MP_MODE_TURN_BATTLE)
            {
                ctx->state = MP_STATE_READY_UP;
                printf("Entering TURN BATTLE ready-up\n");
            }
            else
            {
                ctx->state = MP_STATE_COUNTDOWN;
                ctx->countdownStart = SDL_GetTicks();
                ctx->gameStartTime = ctx->countdownStart + 3000;
            }
        }
        else if (strcmp(msgType, "restart_game") == 0)
        {
            // Game restarting (client receives this from host)
            ctx->state = MP_STATE_COUNTDOWN;
            ctx->countdownStart = SDL_GetTicks();
            ctx->gameStartTime = ctx->countdownStart + 3000;
        }
        else if (strcmp(msgType, "chat") == 0)
        {
            // Chat message from another player (server doesn't echo back to sender)
            json_t *msgObj = json_object_get(data, "message");
            if (msgObj && json_is_string(msgObj))
            {
                const char *message = json_string_value(msgObj);

                // Shift messages up if at capacity
                if (ctx->chatCount >= 10)
                {
                    for (int i = 0; i < 9; i++)
                    {
                        memmove(ctx->chatMessages[i], ctx->chatMessages[i + 1],
                                sizeof(ctx->chatMessages[0]));
                    }
                    ctx->chatCount = 9;
                }

                // Add new message
                strncpy(ctx->chatMessages[ctx->chatCount], message, sizeof(ctx->chatMessages[0]) - 1);
                ctx->chatMessages[ctx->chatCount][sizeof(ctx->chatMessages[0]) - 1] = '\0';
                ctx->chatCount++;

                // Play chat notification sound
                if (ctx->audio)
                {
                    Audio_PlaySound((AudioSystem *)ctx->audio, SOUND_CHAT);
                }

                printf("✓ Chat message received from other player: %s\n", message);
            }
        }
        else if (strcmp(msgType, "host_disconnect") == 0)
        {
            // Host is disconnecting - all clients should disconnect
            printf("✓ Host disconnected - returning to main menu\n");
            ctx->state = MP_STATE_DISCONNECTED;
            strncpy(ctx->errorMessage, "Host disconnected", sizeof(ctx->errorMessage) - 1);
        }
        else if (strcmp(msgType, "player_left") == 0)
        {
            // A player has left the game (from server)
            const char *leftClientId = json_string_value(json_object_get(data, "clientId"));

            if (leftClientId)
            {
                // Find and remove the player
                for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
                {
                    if (ctx->players[i].joined && strcmp(ctx->players[i].clientId, leftClientId) == 0)
                    {
                        printf("✓ Player left: %s\n", ctx->players[i].name);
                        ctx->players[i].joined = false;
                        ctx->players[i].ready = false;
                        ctx->players[i].turnFinished = false;

                        // If we're in turn battle and waiting for results, check if we can show results now
                        if (ctx->gameMode == MP_MODE_TURN_BATTLE)
                        {
                            if (ctx->state == MP_STATE_TURN_WAITING && ctx->isHost)
                            {
                                // Check if all remaining players are finished
                                if (Multiplayer_AllTurnsFinished(ctx))
                                {
                                    Multiplayer_CalculateTurnWinner(ctx);
                                    ctx->resultPageIndex = 0;
                                    ctx->state = MP_STATE_TURN_RESULTS;

                                    // Broadcast results to remaining players
                                    json_t *resultsMsg = json_object();
                                    json_object_set_new(resultsMsg, "type", json_string("show_results"));
                                    mpapi_game(ctx->api, resultsMsg, NULL);
                                    json_decref(resultsMsg);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
        else if (strcmp(msgType, "player_leaving") == 0)
        {
            // A player is voluntarily leaving (broadcast from player)
            if (clientId)
            {
                // Find and remove the player
                for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
                {
                    if (ctx->players[i].joined && strcmp(ctx->players[i].clientId, clientId) == 0)
                    {
                        printf("✓ Player %s is leaving the game\n", ctx->players[i].name);
                        ctx->players[i].joined = false;
                        ctx->players[i].ready = false;
                        ctx->players[i].turnFinished = false;

                        // If we're in turn battle and waiting for results, check if we can show results now
                        if (ctx->gameMode == MP_MODE_TURN_BATTLE)
                        {
                            if (ctx->state == MP_STATE_TURN_WAITING && ctx->isHost)
                            {
                                // Check if all remaining players are finished
                                if (Multiplayer_AllTurnsFinished(ctx))
                                {
                                    Multiplayer_CalculateTurnWinner(ctx);
                                    ctx->resultPageIndex = 0;
                                    ctx->state = MP_STATE_TURN_RESULTS;

                                    // Broadcast results to remaining players
                                    json_t *resultsMsg = json_object();
                                    json_object_set_new(resultsMsg, "type", json_string("show_results"));
                                    mpapi_game(ctx->api, resultsMsg, NULL);
                                    json_decref(resultsMsg);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
        else if (strcmp(msgType, "nick_change") == 0)
        {
            // Nickname change from another player
            int playerIdx = json_integer_value(json_object_get(data, "player_index"));
            json_t *nameObj = json_object_get(data, "new_name");

            if (nameObj && json_is_string(nameObj))
            {
                const char *newName = json_string_value(nameObj);

                // Find player by clientId if possible, otherwise use index
                if (clientId)
                {
                    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
                    {
                        if (ctx->players[i].joined && strcmp(ctx->players[i].clientId, clientId) == 0)
                        {
                            strncpy(ctx->players[i].name, newName, sizeof(ctx->players[i].name) - 1);
                            ctx->players[i].name[sizeof(ctx->players[i].name) - 1] = '\0';
                            printf("✓ Player %d changed name to: %s\n", i, newName);
                            break;
                        }
                    }
                }
            }
        }
        else if (strcmp(msgType, "turn_battle_mode_changed") == 0)
        {
            // Turn Battle mode changed by host
            int mode = json_integer_value(json_object_get(data, "mode"));
            ctx->turnBattleMode = (GameMode)mode;
            ctx->turnBattleModeSelection = (mode == MODE_CLASSIC) ? 0 : 1;
            printf("✓ Turn Battle mode changed to: %s\n", mode == MODE_CLASSIC ? "CLASSIC" : "POWER-UP");
        }
        else if (strcmp(msgType, "turn_results") == 0)
        {
            // Turn battle results from a player
            int playerIdx = json_integer_value(json_object_get(data, "playerIndex"));
            int bestScore = json_integer_value(json_object_get(data, "bestScore"));
            json_t *attemptsArray = json_object_get(data, "attempts");

            if (playerIdx >= 0 && playerIdx < MAX_MULTIPLAYER_PLAYERS && json_is_array(attemptsArray))
            {
                ctx->players[playerIdx].bestScore = bestScore;
                ctx->players[playerIdx].completedAttempts = json_array_size(attemptsArray);

                // Load attempt details
                for (int i = 0; i < 3 && i < json_array_size(attemptsArray); i++)
                {
                    json_t *attemptObj = json_array_get(attemptsArray, i);
                    ctx->players[playerIdx].attempts[i].score = json_integer_value(json_object_get(attemptObj, "score"));
                    ctx->players[playerIdx].attempts[i].length = json_integer_value(json_object_get(attemptObj, "length"));
                    ctx->players[playerIdx].attempts[i].survivalTime = json_integer_value(json_object_get(attemptObj, "survivalTime"));
                }

                ctx->players[playerIdx].turnFinished = true;
                printf("✓ Received turn results from player %d - Best: %d\n", playerIdx, bestScore);

                // If host and all players finished, move to results
                if (ctx->isHost && Multiplayer_AllTurnsFinished(ctx))
                {
                    Multiplayer_CalculateTurnWinner(ctx);
                    ctx->resultPageIndex = 0; // Start at winner
                    ctx->state = MP_STATE_TURN_RESULTS;

                    // Broadcast results state to all players
                    json_t *resultsMsg = json_object();
                    json_object_set_new(resultsMsg, "type", json_string("show_results"));
                    mpapi_game(ctx->api, resultsMsg, NULL);
                    json_decref(resultsMsg);
                }
            }
        }
        else if (strcmp(msgType, "show_results") == 0)
        {
            // Host is telling us to show results
            ctx->resultPageIndex = 0; // Start at winner
            ctx->state = MP_STATE_TURN_RESULTS;
            Multiplayer_CalculateTurnWinner(ctx);
        }
        else if (strcmp(msgType, "start_turn_attempt") == 0)
        {
            // Host is starting a turn attempt for all players
            int attempt = json_integer_value(json_object_get(data, "attempt"));
            ctx->currentAttempt = attempt;
            Multiplayer_StartTurnAttempt(ctx);
            printf("✓ Starting turn attempt %d/3\n", attempt + 1);
        }
        else if (strcmp(msgType, "return_to_lobby") == 0)
        {
            // Host is returning everyone to lobby after turn battle results
            ctx->state = MP_STATE_LOBBY;

            // Reset turn battle data
            for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
            {
                ctx->players[i].ready = false;
                ctx->players[i].turnFinished = false;
                ctx->players[i].completedAttempts = 0;
                ctx->players[i].bestScore = 0;
            }
            ctx->currentAttempt = 0;
            ctx->resultPageIndex = 0;
            printf("✓ Returned to lobby\n");
        }
        else if (strcmp(msgType, "cancel_ready_up") == 0)
        {
            // Host cancelled ready-up, return to lobby
            ctx->state = MP_STATE_LOBBY;

            // Reset ready states
            for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
            {
                ctx->players[i].ready = false;
            }
            printf("✓ Ready-up cancelled, returned to lobby\n");
        }
    }
}

// Create multiplayer context
MultiplayerContext *Multiplayer_Create(void)
{
    MultiplayerContext *ctx = calloc(1, sizeof(MultiplayerContext));
    if (!ctx)
        return NULL;

    // Initialize mpapi with server settings
    const char *server_host = "kontoret.onvo.se";
    uint16_t server_port = 9001;

    // Generate deterministic UUID based on student number
    // This ensures all instances with same student_number can play together
    // Format: 00000000-0000-0000-0000-0000000000XX where XX is student number
    char identifier[37];
    snprintf(identifier, sizeof(identifier),
             "00000000-0000-0000-0000-0000000000%s", STUDENT_NUMBER);

    printf("🔵 Using team identifier (student %s): %s\n", STUDENT_NUMBER, identifier);

    ctx->api = mpapi_create(server_host, server_port, identifier);
    if (!ctx->api)
    {
        printf("Failed to create mpapi instance\n");
        free(ctx);
        return NULL;
    }

    ctx->state = MP_STATE_MENU;
    ctx->localPlayerIndex = -1;
    ctx->currentSpeed = BASE_SPEED;
    ctx->listenerId = -1;
    ctx->audio = NULL; // Will be set by caller
    ctx->menuSelection = 0;
    ctx->sessionInputLen = 0;
    memset(ctx->sessionInput, 0, sizeof(ctx->sessionInput));
    ctx->roomNameLen = 0;
    memset(ctx->roomName, 0, sizeof(ctx->roomName));
    ctx->browsedGameCount = 0;
    ctx->selectedGameIndex = 0;
    ctx->selectedBackground = 0; // Default to first map (de_cyberpunk)
    ctx->gameOverSelection = 0;  // Default to "Try Again"

    // Initialize chat
    ctx->chatCount = 0;
    ctx->chatInputLen = 0;
    ctx->skipNextChatChar = false;
    memset(ctx->chatInput, 0, sizeof(ctx->chatInput));
    ctx->previousState = MP_STATE_LOBBY;

    // Initialize nick change
    ctx->nickInputLen = 0;
    ctx->skipNextNickChar = false;
    memset(ctx->nickInput, 0, sizeof(ctx->nickInput));

    // Initialize turn battle
    ctx->gameMode = MP_MODE_TURN_BATTLE; // Default to turn battle
    ctx->modeSelection = 0;
    ctx->turnBattleMode = MODE_CLASSIC; // Default to classic mode
    ctx->turnBattleModeSelection = 0; // 0 = Classic
    ctx->currentAttempt = 0;
    ctx->attemptStartTime = 0;
    ctx->resultPageIndex = 0;

    return ctx;
}

// Destroy multiplayer context
void Multiplayer_Destroy(MultiplayerContext *_Ctx)
{
    if (!_Ctx)
        return;

    if (_Ctx->api)
    {
        if (_Ctx->listenerId >= 0)
        {
            mpapi_unlisten(_Ctx->api, _Ctx->listenerId);
        }
        mpapi_destroy(_Ctx->api);
    }

    free(_Ctx);
}

// Host a game
/* NOTE: Implementation moved to mp_host.c */
int Multiplayer_Host(MultiplayerContext *_Ctx, const char *_PlayerName)
{
    return MP_Host_Create(_Ctx, _PlayerName);
}

// Join a game
/* NOTE: Implementation moved to mp_client.c */
int Multiplayer_Join(MultiplayerContext *_Ctx, const char *_SessionId, const char *_PlayerName)
{
    return MP_Client_Join(_Ctx, _SessionId, _PlayerName);
}


// Host update (called every frame when hosting)
/* NOTE: Implementation moved to mp_host.c */
void Multiplayer_HostUpdate(MultiplayerContext *_Ctx, unsigned int _CurrentTime)
{
    MP_Host_Update(_Ctx, _CurrentTime);
}

// Broadcast state to all clients
/* NOTE: Implementation moved to mp_host.c */
void Multiplayer_HostBroadcastState(MultiplayerContext *_Ctx)
{
    MP_Host_BroadcastState(_Ctx);
}

// Client send input
/* NOTE: Implementation moved to mp_client.c */
void Multiplayer_ClientSendInput(MultiplayerContext *_Ctx, Direction _Dir)
{
    MP_Client_SendInput(_Ctx, _Dir);
}


// Send lightweight position update for smooth rendering
void Multiplayer_SendQuickPositionUpdate(MultiplayerContext *_Ctx)
{
    // Disabled - caused jittery gameplay
    // Position updates are now handled by full state broadcasts only
    (void)_Ctx;
}

// Start game
/* NOTE: Implementation moved to mp_game.c */
void Multiplayer_StartGame(MultiplayerContext *_Ctx)
{
    MP_Game_Start(_Ctx);
}


// Restart game (after game over)
/* NOTE: Implementation moved to mp_game.c */
void Multiplayer_RestartGame(MultiplayerContext *_Ctx)
{
    MP_Game_Restart(_Ctx);
}


// Toggle ready
/* NOTE: Implementation moved to mp_client.c */
void Multiplayer_ToggleReady(MultiplayerContext *_Ctx)
{
    MP_Client_ToggleReady(_Ctx);
}


// Check if all players ready
/* NOTE: Implementation moved to mp_game.c */
bool Multiplayer_AllReady(MultiplayerContext *_Ctx)
{
    return MP_Game_AllReady(_Ctx);
}


// Get local player index
/* NOTE: Implementation moved to mp_game.c */
int Multiplayer_GetLocalPlayerIndex(MultiplayerContext *_Ctx)
{
    return MP_Game_GetLocalPlayerIndex(_Ctx);
}


// Update countdown and initialize game when countdown finishes
/* NOTE: Implementation moved to mp_game.c */
void Multiplayer_UpdateCountdown(MultiplayerContext *_Ctx, unsigned int _CurrentTime)
{
    MP_Game_UpdateCountdown(_Ctx, _CurrentTime);
}


// Serialize state to JSON
/* NOTE: Implementation moved to mp_network.c */
json_t *Multiplayer_SerializeState(MultiplayerContext *_Ctx)
{
    return MP_Network_SerializeState(_Ctx);
}

// Deserialize state from JSON
/* NOTE: Implementation moved to mp_network.c */
void Multiplayer_DeserializeState(MultiplayerContext *_Ctx, json_t *_Data)
{
    MP_Network_DeserializeState(_Ctx, _Data);
}

// Browse public games
/* NOTE: Implementation moved to mp_network.c */
int Multiplayer_BrowseGames(MultiplayerContext *_Ctx)
{
    return MP_Network_BrowseGames(_Ctx);
}

// ============================================================================
// Turn Battle Functions
// ============================================================================

/* NOTE: Implementation moved to mp_turn_battle.c */
void Multiplayer_StartTurnAttempt(MultiplayerContext *_Ctx)
{
    MP_TurnBattle_StartAttempt(_Ctx);
}


/* NOTE: Implementation moved to mp_turn_battle.c */
void Multiplayer_FinishTurnAttempt(MultiplayerContext *_Ctx)
{
    MP_TurnBattle_FinishAttempt(_Ctx);
}


/* NOTE: Implementation moved to mp_turn_battle.c */
void Multiplayer_SubmitTurnResults(MultiplayerContext *_Ctx)
{
    MP_TurnBattle_SubmitResults(_Ctx);
}


/* NOTE: Implementation moved to mp_turn_battle.c */
bool Multiplayer_AllTurnsFinished(MultiplayerContext *_Ctx)
{
    return MP_TurnBattle_AllFinished(_Ctx);
}


/* NOTE: Implementation moved to mp_turn_battle.c */
void Multiplayer_CalculateTurnWinner(MultiplayerContext *_Ctx)
{
    MP_TurnBattle_CalculateWinner(_Ctx);
}

