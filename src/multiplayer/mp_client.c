/*
 * mp_client.c - Client-side multiplayer operations
 * Part of SNEJK2D multiplayer refactoring (Phase 4)
 *
 * Functions:
 * - Join game session
 * - Send input to server
 * - Toggle ready status
 */

#include "../../include/multiplayer/multiplayer.h"
#include "../../include/multiplayer/mp_client.h"
#include "../../include/entities/snake.h"
#include <SDL2/SDL.h>
#include <string.h>

/* External references */
extern void mpapiEventCallback(const char *event, int64_t messageId, const char *clientId, json_t *data, void *context);

/*
 * Join an existing multiplayer session
 * Returns: 1 on success, 0 on failure
 */
int MP_Client_Join(MultiplayerContext *_Ctx, const char *_SessionId, const char *_PlayerName)
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
    json_t *joinResponse = NULL; // Get full response to parse existing clients

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

/*
 * Send input direction to server for validation
 * (Client-side only, hosts process input locally)
 */
void MP_Client_SendInput(MultiplayerContext *_Ctx, Direction _Dir)
{
    if (!_Ctx || !_Ctx->api || _Ctx->isHost)
        return;

    int localIdx = _Ctx->localPlayerIndex;
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

/*
 * Toggle local player's ready status
 */
void MP_Client_ToggleReady(MultiplayerContext *_Ctx)
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
