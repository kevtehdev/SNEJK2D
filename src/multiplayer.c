#include "../include/multiplayer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// mpapi event callback
static void mpapiEventCallback(const char *event, int64_t messageId, const char *clientId, json_t *data, void *context)
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
        else if (strcmp(msgType, "start_game") == 0)
        {
            // Game starting
            ctx->state = MP_STATE_COUNTDOWN;
            ctx->countdownStart = SDL_GetTicks();
            ctx->gameStartTime = ctx->countdownStart + 3000;
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
                        strcpy(ctx->chatMessages[i], ctx->chatMessages[i + 1]);
                    }
                    ctx->chatCount = 9;
                }

                // Add new message
                strncpy(ctx->chatMessages[ctx->chatCount], message, sizeof(ctx->chatMessages[0]) - 1);
                ctx->chatMessages[ctx->chatCount][sizeof(ctx->chatMessages[0]) - 1] = '\0';
                ctx->chatCount++;

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
    }
}

// Create multiplayer context
MultiplayerContext* Multiplayer_Create(void)
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
    ctx->menuSelection = 0;
    ctx->sessionInputLen = 0;
    memset(ctx->sessionInput, 0, sizeof(ctx->sessionInput));
    ctx->roomNameLen = 0;
    memset(ctx->roomName, 0, sizeof(ctx->roomName));
    ctx->browsedGameCount = 0;
    ctx->selectedGameIndex = 0;
    ctx->selectedBackground = 0;  // Default to first map (de_cyberpunk)
    ctx->gameOverSelection = 0;  // Default to "Try Again"

    // Initialize chat
    ctx->chatCount = 0;
    ctx->chatInputLen = 0;
    memset(ctx->chatInput, 0, sizeof(ctx->chatInput));
    ctx->previousState = MP_STATE_LOBBY;

    // Initialize nick change
    ctx->nickInputLen = 0;
    memset(ctx->nickInput, 0, sizeof(ctx->nickInput));

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
int Multiplayer_Host(MultiplayerContext *_Ctx, const char *_PlayerName)
{
    if (!_Ctx || !_Ctx->api)
        return 0;

    printf("Hosting game: %s...\n", _Ctx->roomName);

    _Ctx->isHost = true;
    _Ctx->state = MP_STATE_HOSTING;

    // Map names with de_ prefix
    const char *mapNames[] = {"de_cyberpunk", "de_forest", "de_underwater", "de_mountain", "de_country"};

    printf("DEBUG: selectedBackground = %d (%s)\n", _Ctx->selectedBackground,
           mapNames[_Ctx->selectedBackground % 5]);

    // Format room name with student number AND map ID encoded in the name
    // Format: [67] roomname |m2| where 2 is the map ID
    char nameWithId[128];
    snprintf(nameWithId, sizeof(nameWithId), "[%s] %s |m%d|",
             STUDENT_NUMBER, _Ctx->roomName, _Ctx->selectedBackground);

    json_t *hostData = json_object();
    json_object_set_new(hostData, "name", json_string(nameWithId));
    json_object_set_new(hostData, "maxPlayers", json_integer(2));  // 1v1 game
    json_object_set_new(hostData, "map", json_integer(_Ctx->selectedBackground));  // Include map ID

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
    _Ctx->players[0].ready = false;  // Explicitly set ready to false
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
    _Ctx->players[0].alive = false;  // Not alive until game starts

    _Ctx->state = MP_STATE_LOBBY;
    printf("Session created: %s\n", _Ctx->sessionId);
    printf("✓ Waiting for Player 2 to join...\n");
    printf("✓ Press SPACE to toggle ready, ENTER to start when both ready\n");

    return 1;
}

// Join a game
int Multiplayer_Join(MultiplayerContext *_Ctx, const char *_SessionId, const char *_PlayerName)
{
    if (!_Ctx || !_Ctx->api)
        return 0;

    printf("Joining session: %s\n", _SessionId);

    _Ctx->isHost = false;
    _Ctx->state = MP_STATE_JOINING;

    // Register listener BEFORE joining (critical!)
    _Ctx->listenerId = mpapi_listen(_Ctx->api, mpapiEventCallback, _Ctx);
    if (_Ctx->listenerId < 0)
    {
        printf("Failed to register event listener\n");
        snprintf(_Ctx->errorMessage, sizeof(_Ctx->errorMessage), "Failed to register event listener");
        _Ctx->state = MP_STATE_DISCONNECTED;
        return 0;
    }

    // Create join data
    json_t *joinData = json_object();
    json_object_set_new(joinData, "name", json_string(_PlayerName));

    char *returnedSession = NULL;
    char *clientId = NULL;
    json_t *joinResponse = NULL;  // Get full response to parse existing clients

    int rc = mpapi_join(_Ctx->api, _SessionId, joinData, &returnedSession, &clientId, &joinResponse);
    json_decref(joinData);

    if (rc != MPAPI_OK)
    {
        printf("Failed to join: error %d\n", rc);

        // Clean up listener since join failed
        if (_Ctx->listenerId >= 0)
        {
            mpapi_unlisten(_Ctx->api, _Ctx->listenerId);
            _Ctx->listenerId = -1;
        }

        snprintf(_Ctx->errorMessage, sizeof(_Ctx->errorMessage),
                 "Failed to join: %s",
                 rc == MPAPI_ERR_REJECTED ? "Invalid session ID" : "Connection error");
        _Ctx->state = MP_STATE_DISCONNECTED;
        return 0;
    }

    // Store session info
    if (returnedSession)
    {
        strncpy(_Ctx->sessionId, returnedSession, sizeof(_Ctx->sessionId) - 1);
        free(returnedSession);
    }

    if (clientId)
    {
        strncpy(_Ctx->ourClientId, clientId, sizeof(_Ctx->ourClientId) - 1);
        _Ctx->ourClientId[sizeof(_Ctx->ourClientId) - 1] = '\0';
        free(clientId);
    }

    // Parse join response to get existing players (including ourselves)
    // The "clients" array contains all clientIds already in the session
    if (joinResponse && json_is_object(joinResponse))
    {
        printf("✓ Parsing join response for existing players\n");
        json_t *clientsArray = json_object_get(joinResponse, "clients");

        if (json_is_array(clientsArray))
        {
            size_t numClients = json_array_size(clientsArray);
            printf("✓ Found %zu client(s) in session\n", numClients);

            // Add all existing clients as players
            for (size_t i = 0; i < numClients && i < MAX_MULTIPLAYER_PLAYERS; i++)
            {
                json_t *clientIdJson = json_array_get(clientsArray, i);
                if (json_is_string(clientIdJson))
                {
                    const char *cid = json_string_value(clientIdJson);

                    _Ctx->players[i].joined = true;
                    _Ctx->players[i].ready = false;
                    _Ctx->players[i].alive = false;
                    strncpy(_Ctx->players[i].clientId, cid, sizeof(_Ctx->players[i].clientId) - 1);
                    _Ctx->players[i].clientId[sizeof(_Ctx->players[i].clientId) - 1] = '\0';

                    // Check if this is the local player
                    if (strcmp(cid, _Ctx->ourClientId) == 0)
                    {
                        _Ctx->players[i].isLocal = true;
                        _Ctx->localPlayerIndex = i;
                        strncpy(_Ctx->players[i].name, _PlayerName, sizeof(_Ctx->players[i].name) - 1);
                        _Ctx->players[i].name[sizeof(_Ctx->players[i].name) - 1] = '\0';
                        printf("✓ Local player at index %zu\n", i);
                    }
                    else
                    {
                        _Ctx->players[i].isLocal = false;
                        // Name will be updated when we receive game state from host
                        snprintf(_Ctx->players[i].name, sizeof(_Ctx->players[i].name), "Player %zu", i + 1);
                    }

                    // Initialize snake
                    Snake_Init(&_Ctx->players[i].snake);
                    if (i == 0)
                    {
                        // Player 0 (host) starts on left
                        _Ctx->players[i].snake.segments[0].x = GRID_WIDTH / 4;
                    }
                    else
                    {
                        // Player 1+ starts on right
                        _Ctx->players[i].snake.segments[0].x = (3 * GRID_WIDTH) / 4;
                    }
                    _Ctx->players[i].snake.segments[0].y = GRID_HEIGHT / 2;
                    for (int j = 1; j < _Ctx->players[i].snake.length; j++)
                    {
                        _Ctx->players[i].snake.segments[j] = _Ctx->players[i].snake.segments[0];
                    }
                }
            }
        }

        json_decref(joinResponse);
    }

    // Listener already registered before join
    _Ctx->state = MP_STATE_LOBBY;

    printf("Joined session successfully\n");
    printf("✓ Press SPACE to toggle ready, wait for host to start\n");

    return 1;
}

// Host update (called every frame when hosting)
void Multiplayer_HostUpdate(MultiplayerContext *_Ctx, unsigned int _CurrentTime)
{
    if (!_Ctx || !_Ctx->isHost)
        return;

    if (_Ctx->state != MP_STATE_PLAYING)
        return;

    // High-frequency state broadcast (every 16ms for minimal input lag)
    static unsigned int lastBroadcast = 0;
    if (_CurrentTime - lastBroadcast >= 16)
    {
        Multiplayer_HostBroadcastState(_Ctx);
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

                    break;  // Only eat one food per frame
                }
            }

            // Check combo timeout (dynamic based on speed)
            if (_Ctx->players[i].comboCount > 0 && _Ctx->players[i].lastFoodTime > 0)
            {
                unsigned int dynamicTimeout = _Ctx->currentSpeed * 30;  // Base scaling
                if (dynamicTimeout < COMBO_TIMEOUT_MIN)
                    dynamicTimeout = COMBO_TIMEOUT_MIN;  // Never shorter than min
                if (dynamicTimeout > COMBO_TIMEOUT_MAX)
                    dynamicTimeout = COMBO_TIMEOUT_MAX;  // Never longer than max

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
            Multiplayer_HostBroadcastState(_Ctx);
        }
    }
}

// Broadcast state to all clients
void Multiplayer_HostBroadcastState(MultiplayerContext *_Ctx)
{
    if (!_Ctx || !_Ctx->api)
        return;

    json_t *state = Multiplayer_SerializeState(_Ctx);
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

// Client send input
void Multiplayer_ClientSendInput(MultiplayerContext *_Ctx, Direction _Dir)
{
    if (!_Ctx || !_Ctx->api || _Ctx->isHost)
        return;

    int localIdx = Multiplayer_GetLocalPlayerIndex(_Ctx);
    if (localIdx < 0 || !_Ctx->players[localIdx].alive)
        return;

    // Send input to server for validation and broadcast
    json_t *message = json_object();
    json_object_set_new(message, "type", json_string("input"));
    json_object_set_new(message, "clientId", json_string(_Ctx->ourClientId));
    json_object_set_new(message, "direction", json_integer((int)_Dir));

    int rc = mpapi_game(_Ctx->api, message, NULL);
    if (rc != MPAPI_OK)
    {
        printf("Failed to send input: error %d\n", rc);
    }

    json_decref(message);
}

// Send lightweight position update for smooth rendering
void Multiplayer_SendQuickPositionUpdate(MultiplayerContext *_Ctx)
{
    // Disabled - caused jittery gameplay
    // Position updates are now handled by full state broadcasts only
    (void)_Ctx;
}

// Start game
void Multiplayer_StartGame(MultiplayerContext *_Ctx)
{
    if (!_Ctx || !_Ctx->isHost)
        return;

    json_t *message = json_object();
    json_object_set_new(message, "type", json_string("start_game"));

    int rc = mpapi_game(_Ctx->api, message, NULL);
    if (rc != MPAPI_OK)
    {
        printf("Failed to send start command: error %d\n", rc);
    }

    json_decref(message);

    _Ctx->state = MP_STATE_COUNTDOWN;
    _Ctx->countdownStart = SDL_GetTicks();
    _Ctx->gameStartTime = _Ctx->countdownStart + 3000;
}

// Restart game (after game over)
void Multiplayer_RestartGame(MultiplayerContext *_Ctx)
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

// Toggle ready
void Multiplayer_ToggleReady(MultiplayerContext *_Ctx)
{
    if (!_Ctx || _Ctx->localPlayerIndex < 0)
        return;

    _Ctx->players[_Ctx->localPlayerIndex].ready = !_Ctx->players[_Ctx->localPlayerIndex].ready;

    printf("🔵 multiplayer_toggle_ready() called! Player %d ready=%d\n",
           _Ctx->localPlayerIndex, _Ctx->players[_Ctx->localPlayerIndex].ready);

    // Send ready status
    json_t *message = json_object();
    json_object_set_new(message, "type", json_string("ready"));
    json_object_set_new(message, "ready", json_boolean(_Ctx->players[_Ctx->localPlayerIndex].ready));

    int rc = mpapi_game(_Ctx->api, message, NULL);
    if (rc != MPAPI_OK)
    {
        printf("Failed to send ready status: error %d\n", rc);
    }
    else
    {
        printf("✓ Ready status sent: %s\n", _Ctx->players[_Ctx->localPlayerIndex].ready ? "READY" : "NOT READY");
    }

    json_decref(message);
}

// Check if all players ready
bool Multiplayer_AllReady(MultiplayerContext *_Ctx)
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

// Get local player index
int Multiplayer_GetLocalPlayerIndex(MultiplayerContext *_Ctx)
{
    return _Ctx ? _Ctx->localPlayerIndex : -1;
}

// Update countdown and initialize game when countdown finishes
void Multiplayer_UpdateCountdown(MultiplayerContext *_Ctx, unsigned int _CurrentTime)
{
    if (!_Ctx || _Ctx->state != MP_STATE_COUNTDOWN)
        return;

    // Check if countdown finished
    if (_CurrentTime >= _Ctx->gameStartTime)
    {
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

// Serialize state to JSON
json_t* Multiplayer_SerializeState(MultiplayerContext *_Ctx)
{
    if (!_Ctx)
        return NULL;

    json_t *state = json_object();

    // Players
    json_t *playersArray = json_array();
    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        if (!_Ctx->players[i].joined)
            continue;

        json_t *player = json_object();
        json_object_set_new(player, "joined", json_boolean(_Ctx->players[i].joined));
        json_object_set_new(player, "alive", json_boolean(_Ctx->players[i].alive));
        json_object_set_new(player, "ready", json_boolean(_Ctx->players[i].ready));
        json_object_set_new(player, "score", json_integer(_Ctx->players[i].score));
        json_object_set_new(player, "clientId", json_string(_Ctx->players[i].clientId));
        json_object_set_new(player, "name", json_string(_Ctx->players[i].name));

        // Combo system
        json_object_set_new(player, "combo_count", json_integer(_Ctx->players[i].comboCount));
        json_object_set_new(player, "combo_multiplier", json_real(_Ctx->players[i].comboMultiplier));

        // Snake - send all segments for reliable multiplayer sync
        json_t *snake = json_object();
        json_object_set_new(snake, "length", json_integer(_Ctx->players[i].snake.length));
        json_object_set_new(snake, "direction", json_integer((int)_Ctx->players[i].snake.direction));

        // Send all segments
        json_t *segments = json_array();
        for (int j = 0; j < _Ctx->players[i].snake.length; j++)
        {
            json_t *seg = json_object();
            json_object_set_new(seg, "x", json_integer(_Ctx->players[i].snake.segments[j].x));
            json_object_set_new(seg, "y", json_integer(_Ctx->players[i].snake.segments[j].y));
            json_array_append_new(segments, seg);
        }
        json_object_set_new(snake, "segments", segments);

        json_object_set_new(player, "snake", snake);

        json_array_append_new(playersArray, player);
    }
    json_object_set_new(state, "players", playersArray);

    // Food (two items)
    json_t *foodArray = json_array();
    for (int i = 0; i < 2; i++)
    {
        json_t *foodItem = json_object();
        json_object_set_new(foodItem, "x", json_integer(_Ctx->food[i].x));
        json_object_set_new(foodItem, "y", json_integer(_Ctx->food[i].y));
        json_array_append_new(foodArray, foodItem);
    }
    json_object_set_new(state, "food", foodArray);

    // Game state
    json_object_set_new(state, "mpState", json_integer((int)_Ctx->state));
    json_object_set_new(state, "current_speed", json_integer(_Ctx->currentSpeed));
    json_object_set_new(state, "selected_background", json_integer(_Ctx->selectedBackground));

    return state;
}

// Deserialize state from JSON
void Multiplayer_DeserializeState(MultiplayerContext *_Ctx, json_t *_Data)
{
    if (!_Ctx || !_Data)
        return;

    // Players
    json_t *playersArray = json_object_get(_Data, "players");
    if (json_is_array(playersArray))
    {
        size_t index;
        json_t *playerObj;
        int playerIdx = 0;

        json_array_foreach(playersArray, index, playerObj)
        {
            if (playerIdx >= MAX_MULTIPLAYER_PLAYERS)
                break;

            const char *clientId = json_string_value(json_object_get(playerObj, "clientId"));

            // Update ALL players from host's authoritative state
            // Client must accept server _Data for proper synchronization

            _Ctx->players[playerIdx].joined = json_boolean_value(json_object_get(playerObj, "joined"));
            _Ctx->players[playerIdx].alive = json_boolean_value(json_object_get(playerObj, "alive"));
            _Ctx->players[playerIdx].ready = json_boolean_value(json_object_get(playerObj, "ready"));
            _Ctx->players[playerIdx].score = json_integer_value(json_object_get(playerObj, "score"));

            // Combo system
            _Ctx->players[playerIdx].comboCount = json_integer_value(json_object_get(playerObj, "combo_count"));
            _Ctx->players[playerIdx].comboMultiplier = json_real_value(json_object_get(playerObj, "combo_multiplier"));

            if (clientId)
            {
                strncpy(_Ctx->players[playerIdx].clientId, clientId, sizeof(_Ctx->players[playerIdx].clientId) - 1);
                _Ctx->players[playerIdx].clientId[sizeof(_Ctx->players[playerIdx].clientId) - 1] = '\0';

                // Update is_local flag based on clientId
                _Ctx->players[playerIdx].isLocal = (strcmp(clientId, _Ctx->ourClientId) == 0);

                // Update local_player_index if this is us
                if (_Ctx->players[playerIdx].isLocal)
                {
                    _Ctx->localPlayerIndex = playerIdx;
                }
            }

            const char *name = json_string_value(json_object_get(playerObj, "name"));
            if (name)
            {
                strncpy(_Ctx->players[playerIdx].name, name, sizeof(_Ctx->players[playerIdx].name) - 1);
                _Ctx->players[playerIdx].name[sizeof(_Ctx->players[playerIdx].name) - 1] = '\0';
            }

            // Snake - full segments array
            json_t *snakeObj = json_object_get(playerObj, "snake");
            if (snakeObj)
            {
                int newLength = json_integer_value(json_object_get(snakeObj, "length"));
                Direction newDir = (Direction)json_integer_value(json_object_get(snakeObj, "direction"));

                _Ctx->players[playerIdx].snake.length = newLength;
                _Ctx->players[playerIdx].snake.direction = newDir;

                json_t *segments = json_object_get(snakeObj, "segments");
                if (json_is_array(segments))
                {
                    size_t segIndex;
                    json_t *segObj;
                    int segIdx = 0;

                    json_array_foreach(segments, segIndex, segObj)
                    {
                        if (segIdx >= GRID_WIDTH * GRID_HEIGHT)
                            break;

                        _Ctx->players[playerIdx].snake.segments[segIdx].x = json_integer_value(json_object_get(segObj, "x"));
                        _Ctx->players[playerIdx].snake.segments[segIdx].y = json_integer_value(json_object_get(segObj, "y"));
                        segIdx++;
                    }
                }
            }

            playerIdx++;
        }
    }

    // Food (two items)
    json_t *foodArray = json_object_get(_Data, "food");
    if (json_is_array(foodArray))
    {
        for (int i = 0; i < 2 && i < (int)json_array_size(foodArray); i++)
        {
            json_t *foodItem = json_array_get(foodArray, i);
            if (foodItem)
            {
                _Ctx->food[i].x = json_integer_value(json_object_get(foodItem, "x"));
                _Ctx->food[i].y = json_integer_value(json_object_get(foodItem, "y"));
            }
        }
    }

    // State
    json_t *mpState = json_object_get(_Data, "mpState");
    if (mpState)
    {
        _Ctx->state = (MultiplayerState)json_integer_value(mpState);
    }

    json_t *speed = json_object_get(_Data, "current_speed");
    if (speed)
    {
        _Ctx->currentSpeed = json_integer_value(speed);
    }

    json_t *background = json_object_get(_Data, "selected_background");
    if (background)
    {
        _Ctx->selectedBackground = json_integer_value(background);
    }
}

// Browse public games
int Multiplayer_BrowseGames(MultiplayerContext *_Ctx)
{
    if (!_Ctx || !_Ctx->api)
        return 0;

    printf("Browsing public games...\n");

    json_t *gameList = NULL;
    int rc = mpapi_list(_Ctx->api, &gameList);

    if (rc != MPAPI_OK || !gameList)
    {
        printf("Failed to browse games: error %d\n", rc);
        snprintf(_Ctx->errorMessage, sizeof(_Ctx->errorMessage), "Failed to browse games: error %d", rc);
        return 0;
    }

    // Parse game list
    _Ctx->browsedGameCount = 0;
    if (json_is_array(gameList))
    {
        size_t count = json_array_size(gameList);
        printf("Found %zu public games\n", count);

        for (size_t i = 0; i < count && _Ctx->browsedGameCount < MAX_BROWSEABLE_GAMES; i++)
        {
            json_t *game = json_array_get(gameList, i);
            if (!game)
                continue;

            // Extract game name first (needed for filtering)
            json_t *nameObj = json_object_get(game, "name");
            if (!nameObj || !json_is_string(nameObj))
                continue;

            const char *name = json_string_value(nameObj);

            // Filter by student_number (encoded in name as "[67] GameName")
            char prefix[16];
            snprintf(prefix, sizeof(prefix), "[%s] ", STUDENT_NUMBER);
            if (strncmp(name, prefix, strlen(prefix)) != 0)
            {
                // Skip games that don't match our student number
                continue;
            }

            // Extract session ID (field is "id", not "sessionId")
            json_t *idObj = json_object_get(game, "id");
            if (idObj && json_is_string(idObj))
            {
                const char *session_id = json_string_value(idObj);
                strncpy(_Ctx->browsedGames[_Ctx->browsedGameCount].sessionId,
                       session_id,
                       sizeof(_Ctx->browsedGames[_Ctx->browsedGameCount].sessionId) - 1);
            }
            else
            {
                continue;  // Skip if no ID
            }

            // Parse map ID from name format: "[67] roomname |m2|"
            int mapId = 0;  // Default
            const char *mapMarker = strstr(name, "|m");
            if (mapMarker)
            {
                // Found map marker, extract the number
                int parsedMapId = atoi(mapMarker + 2);  // Skip "|m"
                if (parsedMapId >= 0 && parsedMapId < 5)
                {
                    mapId = parsedMapId;
                    printf("✓ Parsed map ID %d from game name\n", mapId);
                }
            }
            else
            {
                // Try to get from data field as fallback
                json_t *mapObj = json_object_get(game, "map");
                if (mapObj && json_is_integer(mapObj))
                {
                    mapId = json_integer_value(mapObj);
                    printf("✓ Got map ID %d from data field\n", mapId);
                }
                else
                {
                    printf("⚠ No map info found, defaulting to 0\n");
                }
            }
            _Ctx->browsedGames[_Ctx->browsedGameCount].mapId = mapId;

            // Store the game name WITHOUT the map marker for display
            char cleanName[128];
            if (mapMarker)
            {
                // Copy up to the map marker
                size_t len = mapMarker - name;
                if (len > sizeof(cleanName) - 1)
                    len = sizeof(cleanName) - 1;
                strncpy(cleanName, name, len);
                cleanName[len] = '\0';
                // Trim trailing space
                while (len > 0 && cleanName[len-1] == ' ')
                {
                    cleanName[--len] = '\0';
                }
            }
            else
            {
                strncpy(cleanName, name, sizeof(cleanName) - 1);
                cleanName[sizeof(cleanName) - 1] = '\0';
            }

            strncpy(_Ctx->browsedGames[_Ctx->browsedGameCount].name,
                   cleanName,
                   sizeof(_Ctx->browsedGames[_Ctx->browsedGameCount].name) - 1);

            // Extract player count
            json_t *playersObj = json_object_get(game, "clients");
            if (playersObj && json_is_array(playersObj))
            {
                _Ctx->browsedGames[_Ctx->browsedGameCount].playerCount = json_array_size(playersObj);
            }
            else
            {
                _Ctx->browsedGames[_Ctx->browsedGameCount].playerCount = 0;
            }

            _Ctx->browsedGames[_Ctx->browsedGameCount].maxPlayers = MAX_MULTIPLAYER_PLAYERS;

            _Ctx->browsedGameCount++;
        }
    }

    json_decref(gameList);

    printf("Loaded %d games into browse list\n", _Ctx->browsedGameCount);
    _Ctx->selectedGameIndex = 0;

    return 1;
}
