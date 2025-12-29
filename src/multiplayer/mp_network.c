#include "../../include/multiplayer/mp_network.h"
#include "../../include/multiplayer/multiplayer.h"
#include "../../include/config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Network layer module - handles serialization and game browsing */

void MP_Network_Init(MultiplayerContext *_Ctx)
{
    // Network initialization is currently done in Multiplayer_Create
    // This is a placeholder for future refactoring
    (void)_Ctx;
}

void MP_Network_Cleanup(MultiplayerContext *_Ctx)
{
    // Network cleanup is currently done in Multiplayer_Destroy
    // This is a placeholder for future refactoring
    (void)_Ctx;
}

/* Serialize game state to JSON for network transmission */
json_t* MP_Network_SerializeState(MultiplayerContext *_Ctx)
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
    json_object_set_new(state, "turn_battle_mode", json_integer((int)_Ctx->turnBattleMode));

    return state;
}

/* Deserialize state from JSON received over network */
void MP_Network_DeserializeState(MultiplayerContext *_Ctx, json_t *_Data)
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
            // Client must accept server data for proper synchronization

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

    json_t *turnBattleMode = json_object_get(_Data, "turn_battle_mode");
    if (turnBattleMode)
    {
        _Ctx->turnBattleMode = (GameMode)json_integer_value(turnBattleMode);
        _Ctx->turnBattleModeSelection = (_Ctx->turnBattleMode == MODE_CLASSIC) ? 0 : 1;
    }
}

/* Browse public multiplayer games */
int MP_Network_BrowseGames(MultiplayerContext *_Ctx)
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
                continue; // Skip if no ID
            }

            // Parse map ID from name format: "[67] roomname |m2| |gRT|"
            int mapId = 0; // Default
            const char *mapMarker = strstr(name, "|m");
            if (mapMarker)
            {
                // Found map marker, extract the number
                int parsedMapId = atoi(mapMarker + 2); // Skip "|m"
                if (parsedMapId >= 0 && parsedMapId < 6)
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

            // Parse game mode from name format: "|g1V|" or "|gTB|"
            MultiplayerGameMode gameMode = MP_MODE_1VS1; // Default
            const char *modeMarker = strstr(name, "|g");
            if (modeMarker)
            {
                // Check if it's TURN BATTLE (TB) or 1VS1 (1V)
                if (strncmp(modeMarker + 2, "TB", 2) == 0)
                {
                    gameMode = MP_MODE_TURN_BATTLE;
                    printf("✓ Parsed game mode: TURN BATTLE\n");
                }
                else
                {
                    printf("✓ Parsed game mode: 1VS1\n");
                }
            }
            else
            {
                printf("⚠ No game mode info found, defaulting to 1VS1\n");
            }
            _Ctx->browsedGames[_Ctx->browsedGameCount].gameMode = gameMode;

            // Store the game name WITHOUT the map/mode markers for display
            char cleanName[128];
            const char *firstMarker = mapMarker ? mapMarker : modeMarker;
            if (firstMarker)
            {
                // Copy up to the first marker
                size_t len = firstMarker - name;
                if (len > sizeof(cleanName) - 1)
                    len = sizeof(cleanName) - 1;
                strncpy(cleanName, name, len);
                cleanName[len] = '\0';
                // Trim trailing space
                while (len > 0 && cleanName[len - 1] == ' ')
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
