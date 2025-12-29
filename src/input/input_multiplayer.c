#include "../../include/input/input_multiplayer.h"
#include "../../include/audio/audio.h"
#include <SDL2/SDL.h>
#include <string.h>
#include <ctype.h>

void Input_HandleMultiplayerInput(MultiplayerContext *_MpCtx, SDL_Event *_Event, Game *_Game, void *_Audio)
{
    AudioSystem *audio = (AudioSystem *)_Audio;

    if (!_MpCtx)
        return;

    // Handle text input for chat and nick change
    if (_Event->type == SDL_TEXTINPUT)
    {
        if (_MpCtx->state == MP_STATE_CHATTING && _MpCtx->chatInputLen < 127)
        {
            // Skip 'c' from the key that opened chat
            if (_MpCtx->skipNextChatChar && strcmp(_Event->text.text, "c") == 0)
            {
                _MpCtx->skipNextChatChar = false;
                return;
            }
            _MpCtx->skipNextChatChar = false;

            size_t len = strlen(_Event->text.text);
            if (_MpCtx->chatInputLen + len < 127)
            {
                strncat(_MpCtx->chatInput, _Event->text.text, 127 - _MpCtx->chatInputLen);
                _MpCtx->chatInputLen += len;
            }
        }
        else if (_MpCtx->state == MP_STATE_CHANGING_NICK && _MpCtx->nickInputLen < 31)
        {
            // Skip 'n' from the key that opened nick change
            if (_MpCtx->skipNextNickChar && strcmp(_Event->text.text, "n") == 0)
            {
                _MpCtx->skipNextNickChar = false;
                return;
            }
            _MpCtx->skipNextNickChar = false;

            size_t len = strlen(_Event->text.text);
            if (_MpCtx->nickInputLen + len < 31)
            {
                strncat(_MpCtx->nickInput, _Event->text.text, 31 - _MpCtx->nickInputLen);
                _MpCtx->nickInputLen += len;
            }
        }
        return;
    }

    if (_Event->type == SDL_KEYDOWN)
    {
        if (_MpCtx->state == MP_STATE_MENU)
        {
            switch (_Event->key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_w:
                _MpCtx->menuSelection = (_MpCtx->menuSelection - 1 + 2) % 2;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                _MpCtx->menuSelection = (_MpCtx->menuSelection + 1) % 2;
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                if (_MpCtx->menuSelection == MP_MENU_HOST)
                {
                    _MpCtx->state = MP_STATE_ENTERING_NAME;
                    _MpCtx->roomNameLen = 0;
                    memset(_MpCtx->roomName, 0, sizeof(_MpCtx->roomName));
                }
                else if (_MpCtx->menuSelection == MP_MENU_JOIN)
                {
                    _MpCtx->state = MP_STATE_BROWSING;
                    Multiplayer_BrowseGames(_MpCtx);
                }
                break;
            case SDLK_ESCAPE:
                // Return to main menu
                break;
            }
        }
        else if (_MpCtx->state == MP_STATE_ENTERING_NAME)
        {
            if (_Event->key.keysym.sym == SDLK_ESCAPE)
            {
                _MpCtx->state = MP_STATE_MENU;
            }
            else if (_Event->key.keysym.sym == SDLK_BACKSPACE && _MpCtx->roomNameLen > 0)
            {
                _MpCtx->roomNameLen--;
                _MpCtx->roomName[_MpCtx->roomNameLen] = '\0';
            }
            else if (_Event->key.keysym.sym == SDLK_RETURN && _MpCtx->roomNameLen > 0)
            {
                // Name is confirmed, now allow map selection with LEFT/RIGHT/A/D
                // Second ENTER will create the game
                // Use player name from Game struct, or default to "Player 1"
                const char *playerName = (_Game && _Game->playerNameLen > 0) ? _Game->playerName : "Player 1";
                Multiplayer_Host(_MpCtx, playerName);
            }
            // Only allow LEFT/RIGHT for map selection (not A/D to avoid text input conflict)
            else if (_Event->key.keysym.sym == SDLK_LEFT)
            {
                _MpCtx->selectedBackground = (_MpCtx->selectedBackground - 1 + NUM_BACKGROUNDS) % NUM_BACKGROUNDS;
            }
            else if (_Event->key.keysym.sym == SDLK_RIGHT)
            {
                _MpCtx->selectedBackground = (_MpCtx->selectedBackground + 1) % NUM_BACKGROUNDS;
            }
            // UP/DOWN for game mode selection
            else if (_Event->key.keysym.sym == SDLK_UP)
            {
                _MpCtx->modeSelection = 0; // REALTIME
            }
            else if (_Event->key.keysym.sym == SDLK_DOWN)
            {
                _MpCtx->modeSelection = 1; // TURN BATTLE
            }
            else if (_MpCtx->roomNameLen < 32)
            {
                char c = _Event->key.keysym.sym;
                if (c == ' ' || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'Z') || c == '-' || c == '_')
                {
                    _MpCtx->roomName[_MpCtx->roomNameLen++] = c;
                    _MpCtx->roomName[_MpCtx->roomNameLen] = '\0';
                }
            }
        }
        else if (_MpCtx->state == MP_STATE_BROWSING)
        {
            switch (_Event->key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_w:
                if (_MpCtx->selectedGameIndex > 0)
                    _MpCtx->selectedGameIndex--;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                if (_MpCtx->selectedGameIndex < _MpCtx->browsedGameCount - 1)
                    _MpCtx->selectedGameIndex++;
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                if (_MpCtx->browsedGameCount > 0)
                {
                    // Set the background and game mode to match the game we're joining
                    _MpCtx->selectedBackground = _MpCtx->browsedGames[_MpCtx->selectedGameIndex].mapId;
                    _MpCtx->gameMode = _MpCtx->browsedGames[_MpCtx->selectedGameIndex].gameMode;
                    // Use player name from Game struct, or default to "Player 2"
                    const char *playerName = (_Game && _Game->playerNameLen > 0) ? _Game->playerName : "Player 2";
                    Multiplayer_Join(_MpCtx, _MpCtx->browsedGames[_MpCtx->selectedGameIndex].sessionId, playerName);
                }
                break;
            case SDLK_ESCAPE:
                _MpCtx->state = MP_STATE_MENU;
                break;
            }
        }
        else if (_MpCtx->state == MP_STATE_LOBBY)
        {
            switch (_Event->key.keysym.sym)
            {
            case SDLK_c:
                // Enter chat mode
                _MpCtx->previousState = MP_STATE_LOBBY;
                _MpCtx->state = MP_STATE_CHATTING;
                _MpCtx->chatInputLen = 0;
                memset(_MpCtx->chatInput, 0, sizeof(_MpCtx->chatInput));
                _MpCtx->skipNextChatChar = true;  // Skip the 'c' from this keypress
                break;
            case SDLK_n:
                // Enter nick change mode
                _MpCtx->previousState = MP_STATE_LOBBY;
                _MpCtx->state = MP_STATE_CHANGING_NICK;
                _MpCtx->nickInputLen = 0;
                memset(_MpCtx->nickInput, 0, sizeof(_MpCtx->nickInput));
                _MpCtx->skipNextNickChar = true;  // Skip the 'n' from this keypress
                // Pre-fill with current name
                int localIdx = Multiplayer_GetLocalPlayerIndex(_MpCtx);
                if (localIdx >= 0)
                {
                    strncpy(_MpCtx->nickInput, _MpCtx->players[localIdx].name, sizeof(_MpCtx->nickInput) - 1);
                    _MpCtx->nickInputLen = strlen(_MpCtx->nickInput);
                }
                break;
            case SDLK_LEFT:
            case SDLK_RIGHT:
                // Host can change turn battle mode (Classic vs Power-Up) in Turn Battle
                if (_MpCtx->isHost && _MpCtx->gameMode == MP_MODE_TURN_BATTLE)
                {
                    if (_Event->key.keysym.sym == SDLK_LEFT)
                    {
                        _MpCtx->turnBattleModeSelection = (_MpCtx->turnBattleModeSelection == 0) ? 1 : 0;
                    }
                    else
                    {
                        _MpCtx->turnBattleModeSelection = (_MpCtx->turnBattleModeSelection == 0) ? 1 : 0;
                    }
                    _MpCtx->turnBattleMode = (_MpCtx->turnBattleModeSelection == 0) ? MODE_CLASSIC : MODE_POWERUP;

                    // Broadcast the mode change to all clients
                    if (_MpCtx->api)
                    {
                        json_t *message = json_object();
                        json_object_set_new(message, "type", json_string("turn_battle_mode_changed"));
                        json_object_set_new(message, "mode", json_integer(_MpCtx->turnBattleMode));
                        mpapi_game(_MpCtx->api, message, NULL);
                        json_decref(message);
                    }
                }
                break;
            case SDLK_SPACE:
                Multiplayer_ToggleReady(_MpCtx);
                break;
            case SDLK_RETURN:
                if (_MpCtx->isHost && Multiplayer_AllReady(_MpCtx))
                {
                    // Start game with already selected mode
                    Multiplayer_StartGame(_MpCtx);
                }
                break;
            case SDLK_ESCAPE:
                // Broadcast disconnect to clients if host
                if (_MpCtx->isHost && _MpCtx->api)
                {
                    json_t *message = json_object();
                    json_object_set_new(message, "type", json_string("host_disconnect"));
                    mpapi_game(_MpCtx->api, message, NULL);
                    json_decref(message);
                }
                // Mark for disconnection
                _MpCtx->state = MP_STATE_DISCONNECTED;
                break;
            }
        }
        else if (_MpCtx->state == MP_STATE_READY_UP)
        {
            // Turn battle ready-up screen
            switch (_Event->key.keysym.sym)
            {
            case SDLK_SPACE:
                Multiplayer_ToggleReady(_MpCtx);
                break;
            case SDLK_RETURN:
                if (_MpCtx->isHost && Multiplayer_AllReady(_MpCtx))
                {
                    // Broadcast start command to all clients
                    json_t *message = json_object();
                    json_object_set_new(message, "type", json_string("start_turn_attempt"));
                    json_object_set_new(message, "attempt", json_integer(0));
                    mpapi_game(_MpCtx->api, message, NULL);
                    json_decref(message);

                    // Start turn battle for host
                    _MpCtx->currentAttempt = 0;
                    Multiplayer_StartTurnAttempt(_MpCtx);
                }
                break;
            case SDLK_ESCAPE:
                // Host broadcasts cancel to lobby
                if (_MpCtx->isHost && _MpCtx->api)
                {
                    json_t *message = json_object();
                    json_object_set_new(message, "type", json_string("cancel_ready_up"));
                    mpapi_game(_MpCtx->api, message, NULL);
                    json_decref(message);
                }

                // Cancel and return to lobby
                // Reset ready states
                for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
                {
                    _MpCtx->players[i].ready = false;
                }
                _MpCtx->state = MP_STATE_LOBBY;
                break;
            }
        }
        else if (_MpCtx->state == MP_STATE_TURN_WAITING)
        {
            // Waiting for other players to finish
            if (_Event->key.keysym.sym == SDLK_ESCAPE)
            {
                // Broadcast disconnect to clients if host
                if (_MpCtx->isHost && _MpCtx->api)
                {
                    json_t *message = json_object();
                    json_object_set_new(message, "type", json_string("host_disconnect"));
                    mpapi_game(_MpCtx->api, message, NULL);
                    json_decref(message);
                }

                // Disconnect
                _MpCtx->state = MP_STATE_DISCONNECTED;
            }
        }
        else if (_MpCtx->state == MP_STATE_TURN_RESULTS)
        {
            // Results screen
            switch (_Event->key.keysym.sym)
            {
            case SDLK_LEFT:
            case SDLK_a:
                // Navigate to previous player
                if (_MpCtx->resultPageIndex > 0)
                    _MpCtx->resultPageIndex--;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                // Navigate to next player (will be clamped by renderer)
                _MpCtx->resultPageIndex++;
                break;
            case SDLK_RETURN:
                // Host broadcasts return to lobby
                if (_MpCtx->isHost && _MpCtx->api)
                {
                    json_t *message = json_object();
                    json_object_set_new(message, "type", json_string("return_to_lobby"));
                    mpapi_game(_MpCtx->api, message, NULL);
                    json_decref(message);
                }

                // Return to lobby for another round
                _MpCtx->state = MP_STATE_LOBBY;

                // Reset turn battle data
                for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
                {
                    _MpCtx->players[i].ready = false;
                    _MpCtx->players[i].turnFinished = false;
                    _MpCtx->players[i].completedAttempts = 0;
                    _MpCtx->players[i].bestScore = 0;
                }
                _MpCtx->currentAttempt = 0;
                _MpCtx->resultPageIndex = 0;
                break;
            case SDLK_ESCAPE:
                // Broadcast disconnect to clients if host
                if (_MpCtx->isHost && _MpCtx->api)
                {
                    json_t *message = json_object();
                    json_object_set_new(message, "type", json_string("host_disconnect"));
                    mpapi_game(_MpCtx->api, message, NULL);
                    json_decref(message);
                }

                // Return to main menu
                _MpCtx->state = MP_STATE_DISCONNECTED;
                break;
            }
        }
        else if (_MpCtx->state == MP_STATE_CHATTING)
        {
            if (_Event->key.keysym.sym == SDLK_ESCAPE)
            {
                _MpCtx->state = _MpCtx->previousState;
            }
            else if (_Event->key.keysym.sym == SDLK_RETURN && _MpCtx->chatInputLen > 0)
            {
                // Send chat message
                int localIdx = Multiplayer_GetLocalPlayerIndex(_MpCtx);
                if (localIdx >= 0)
                {
                    // Format the message
                    char formattedMsg[160];
                    snprintf(formattedMsg, sizeof(formattedMsg), "%s: %s",
                             _MpCtx->players[localIdx].name, _MpCtx->chatInput);

                    // Add message locally first
                    if (_MpCtx->chatCount >= 10)
                    {
                        for (int i = 0; i < 9; i++)
                        {
                            memmove(_MpCtx->chatMessages[i], _MpCtx->chatMessages[i + 1],
                                    sizeof(_MpCtx->chatMessages[0]));
                        }
                        _MpCtx->chatCount = 9;
                    }
                    strncpy(_MpCtx->chatMessages[_MpCtx->chatCount], formattedMsg,
                            sizeof(_MpCtx->chatMessages[0]) - 1);
                    _MpCtx->chatMessages[_MpCtx->chatCount][sizeof(_MpCtx->chatMessages[0]) - 1] = '\0';
                    _MpCtx->chatCount++;

                    // Broadcast to other players
                    if (_MpCtx->api)
                    {
                        json_t *chatData = json_object();
                        json_object_set_new(chatData, "type", json_string("chat"));
                        json_object_set_new(chatData, "message", json_string(formattedMsg));
                        int rc = mpapi_game(_MpCtx->api, chatData, NULL);
                        json_decref(chatData);

                        if (rc == MPAPI_OK)
                        {
                            printf("✓ Chat message sent: %s\n", formattedMsg);
                        }
                        else
                        {
                            printf("✗ Failed to send chat message: error %d\n", rc);
                        }
                    }
                }

                // Clear input and return to lobby
                _MpCtx->chatInputLen = 0;
                memset(_MpCtx->chatInput, 0, sizeof(_MpCtx->chatInput));
                _MpCtx->state = _MpCtx->previousState;
            }
            else if (_Event->key.keysym.sym == SDLK_BACKSPACE && _MpCtx->chatInputLen > 0)
            {
                _MpCtx->chatInputLen--;
                _MpCtx->chatInput[_MpCtx->chatInputLen] = '\0';
            }
        }
        else if (_MpCtx->state == MP_STATE_CHANGING_NICK)
        {
            if (_Event->key.keysym.sym == SDLK_ESCAPE)
            {
                _MpCtx->state = _MpCtx->previousState;
            }
            else if (_Event->key.keysym.sym == SDLK_RETURN && _MpCtx->nickInputLen > 0)
            {
                // Update local player name
                int localIdx = Multiplayer_GetLocalPlayerIndex(_MpCtx);
                if (localIdx >= 0)
                {
                    strncpy(_MpCtx->players[localIdx].name, _MpCtx->nickInput, sizeof(_MpCtx->players[localIdx].name) - 1);
                    _MpCtx->players[localIdx].name[sizeof(_MpCtx->players[localIdx].name) - 1] = '\0';

                    // Broadcast name change to other players
                    if (_MpCtx->api)
                    {
                        json_t *nickData = json_object();
                        json_object_set_new(nickData, "type", json_string("nick_change"));
                        json_object_set_new(nickData, "player_index", json_integer(localIdx));
                        json_object_set_new(nickData, "new_name", json_string(_MpCtx->players[localIdx].name));
                        mpapi_game(_MpCtx->api, nickData, NULL);
                        json_decref(nickData);
                    }
                }
                // Clear input and return to lobby
                _MpCtx->nickInputLen = 0;
                memset(_MpCtx->nickInput, 0, sizeof(_MpCtx->nickInput));
                _MpCtx->state = _MpCtx->previousState;
            }
            else if (_Event->key.keysym.sym == SDLK_BACKSPACE && _MpCtx->nickInputLen > 0)
            {
                _MpCtx->nickInputLen--;
                _MpCtx->nickInput[_MpCtx->nickInputLen] = '\0';
            }
        }
        else if (_MpCtx->state == MP_STATE_GAME_OVER)
        {
            switch (_Event->key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_w:
                _MpCtx->gameOverSelection = 0;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                _MpCtx->gameOverSelection = 1;
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                if (_MpCtx->gameOverSelection == 0)
                {
                    if (_MpCtx->isHost)
                    {
                        Multiplayer_RestartGame(_MpCtx);
                    }
                }
                else // QUIT GAME selected
                {
                    // Broadcast disconnect to clients if host
                    if (_MpCtx->isHost && _MpCtx->api)
                    {
                        json_t *message = json_object();
                        json_object_set_new(message, "type", json_string("host_disconnect"));
                        mpapi_game(_MpCtx->api, message, NULL);
                        json_decref(message);
                    }
                    _MpCtx->state = MP_STATE_DISCONNECTED;
                }
                break;
            case SDLK_ESCAPE:
                // Broadcast disconnect to clients if host
                if (_MpCtx->isHost && _MpCtx->api)
                {
                    json_t *message = json_object();
                    json_object_set_new(message, "type", json_string("host_disconnect"));
                    mpapi_game(_MpCtx->api, message, NULL);
                    json_decref(message);
                }
                _MpCtx->state = MP_STATE_DISCONNECTED;
                break;
            }
        }
        else if (_MpCtx->state == MP_STATE_TURN_PLAYING)
        {
            // Turn battle - control local game snake
            Direction dir = DIR_UP;
            bool hasInput = false;

            switch (_Event->key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_w:
                dir = DIR_UP;
                hasInput = true;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                dir = DIR_DOWN;
                hasInput = true;
                break;
            case SDLK_LEFT:
            case SDLK_a:
                dir = DIR_LEFT;
                hasInput = true;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                dir = DIR_RIGHT;
                hasInput = true;
                break;
            case SDLK_PLUS:
            case SDLK_KP_PLUS:
            case SDLK_EQUALS:
                if (audio)
                    Audio_IncreaseVolume(audio);
                break;
            case SDLK_MINUS:
            case SDLK_KP_MINUS:
                if (audio)
                    Audio_DecreaseVolume(audio);
                break;
            }

            if (hasInput)
            {
                Snake_SetDirection(&_MpCtx->localGame.snake, dir);
            }
        }
        else if (_MpCtx->state == MP_STATE_PLAYING)
        {
            Direction dir = DIR_UP;
            bool hasInput = false;

            switch (_Event->key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_w:
                dir = DIR_UP;
                hasInput = true;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                dir = DIR_DOWN;
                hasInput = true;
                break;
            case SDLK_LEFT:
            case SDLK_a:
                dir = DIR_LEFT;
                hasInput = true;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                dir = DIR_RIGHT;
                hasInput = true;
                break;
            case SDLK_PLUS:
            case SDLK_KP_PLUS:
            case SDLK_EQUALS:
                if (audio)
                    Audio_IncreaseVolume(audio);
                break;
            case SDLK_MINUS:
            case SDLK_KP_MINUS:
                if (audio)
                    Audio_DecreaseVolume(audio);
                break;
            case SDLK_ESCAPE:
                // Broadcast disconnect to clients if host
                if (_MpCtx->isHost && _MpCtx->api)
                {
                    json_t *message = json_object();
                    json_object_set_new(message, "type", json_string("host_disconnect"));
                    mpapi_game(_MpCtx->api, message, NULL);
                    json_decref(message);
                }
                // Mark for disconnection - this will return both players to main menu
                _MpCtx->state = MP_STATE_DISCONNECTED;
                break;
            }

            if (hasInput)
            {
                if (_MpCtx->isHost)
                {
                    int localIdx = Multiplayer_GetLocalPlayerIndex(_MpCtx);
                    if (localIdx >= 0)
                    {
                        Snake_SetDirection(&_MpCtx->players[localIdx].snake, dir);
                    }
                }
                else
                {
                    Multiplayer_ClientSendInput(_MpCtx, dir);
                }
            }
        }
    }
}

