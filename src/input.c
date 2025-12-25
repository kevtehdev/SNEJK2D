#include "../include/input.h"
#include "../include/audio.h"
#include "../include/multiplayer.h"
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Global state
int g_main_menu_selection = 0;
bool g_score_saved = false;

void Input_HandleMainMenuInput(Game *_Game, SDL_Event *_Event, MultiplayerContext **_MpCtx)
{
    if (_Event->type == SDL_KEYDOWN)
    {
        switch (_Event->key.keysym.sym)
        {
        case SDLK_UP:
        case SDLK_w:
            g_main_menu_selection = (g_main_menu_selection - 1 + 5) % 5;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            g_main_menu_selection = (g_main_menu_selection + 1) % 5;
            break;
        case SDLK_RETURN:
        case SDLK_SPACE:
            if (g_main_menu_selection == 0)
            {
                // Singleplayer - go to name input first
                _Game->state = GAME_NAME_INPUT;
            }
            else if (g_main_menu_selection == 1)
            {
                _Game->state = GAME_MULTIPLAYER;
                if (!*_MpCtx)
                {
                    *_MpCtx = Multiplayer_Create();
                }
            }
            else if (g_main_menu_selection == 2)
            {
                _Game->state = GAME_SCOREBOARD;
            }
            else if (g_main_menu_selection == 3)
            {
                _Game->state = GAME_OPTIONS;
            }
            else if (g_main_menu_selection == 4)
            {
                // EXIT
                _Game->running = false;
            }
            break;
        case SDLK_ESCAPE:
            _Game->running = false;
            break;
        }
    }
}

void Input_HandleNameInput(Game *_Game, SDL_Event *_Event)
{
    if (_Event->type == SDL_KEYDOWN)
    {
        if (_Event->key.keysym.sym == SDLK_ESCAPE)
        {
            _Game->state = GAME_MAIN_MENU;
        }
        else if (_Event->key.keysym.sym == SDLK_RETURN && _Game->playerNameLen > 0)
        {
            _Game->state = GAME_MENU;
        }
        else if (_Event->key.keysym.sym == SDLK_BACKSPACE && _Game->playerNameLen > 0)
        {
            _Game->playerNameLen--;
            _Game->playerName[_Game->playerNameLen] = '\0';
        }
        else if (_Game->playerNameLen < 31)
        {
            char c = _Event->key.keysym.sym;
            if (c == ' ' || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                (c >= 'A' && c <= 'Z') || c == '-' || c == '_')
            {
                _Game->playerName[_Game->playerNameLen++] = c;
                _Game->playerName[_Game->playerNameLen] = '\0';
            }
        }
    }
}

void Input_HandleGameMenuInput(Game *_Game, SDL_Event *_Event)
{
    if (_Event->type == SDL_KEYDOWN)
    {
        switch (_Event->key.keysym.sym)
        {
        case SDLK_LEFT:
            _Game->selectedBackground = (_Game->selectedBackground - 1 + NUM_BACKGROUNDS) % NUM_BACKGROUNDS;
            break;
        case SDLK_RIGHT:
            _Game->selectedBackground = (_Game->selectedBackground + 1) % NUM_BACKGROUNDS;
            break;
        case SDLK_SPACE:
        case SDLK_RETURN:
            Game_Reset(_Game);
            break;
        case SDLK_ESCAPE:
            _Game->state = GAME_MAIN_MENU;
            break;
        }
    }
}

void Input_HandlePlayingInput(Game *_Game, SDL_Event *_Event)
{
    if (_Event->type == SDL_KEYDOWN)
    {
        Direction dir;
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
        case SDLK_ESCAPE:
            _Game->state = GAME_MENU;
            break;
        }

        if (hasInput)
        {
            Snake_SetDirection(&_Game->snake, dir);
        }
    }
}

void Input_HandleGameOverInput(Game *_Game, SDL_Event *_Event)
{
    if (_Event->type == SDL_KEYDOWN)
    {
        switch (_Event->key.keysym.sym)
        {
        case SDLK_UP:
        case SDLK_w:
            _Game->gameOverSelection = 0;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            _Game->gameOverSelection = 1;
            break;
        case SDLK_RETURN:
        case SDLK_SPACE:
            if (_Game->gameOverSelection == 0)
            {
                g_score_saved = false;
                Game_Reset(_Game);
            }
            else
            {
                g_score_saved = false;
                _Game->state = GAME_MAIN_MENU;
                _Game->gameOverSelection = 0;
            }
            break;
        case SDLK_ESCAPE:
            g_score_saved = false;
            _Game->state = GAME_MAIN_MENU;
            _Game->gameOverSelection = 0;
            break;
        }
    }
}

void Input_HandleScoreboardInput(Game *_Game, SDL_Event *_Event)
{
    if (_Event->type == SDL_KEYDOWN)
    {
        if (_Event->key.keysym.sym == SDLK_ESCAPE)
        {
            _Game->state = GAME_MAIN_MENU;
        }
    }
}

void Input_HandleOptionsInput(Game *_Game, SDL_Event *_Event, void *_Audio)
{
    if (_Event->type == SDL_KEYDOWN)
    {
        switch (_Event->key.keysym.sym)
        {
        case SDLK_UP:
        case SDLK_w:
            _Game->optionsSelection = (_Game->optionsSelection - 1 + 5) % 5;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            _Game->optionsSelection = (_Game->optionsSelection + 1) % 5;
            break;
        case SDLK_LEFT:
        case SDLK_a:
        case SDLK_RIGHT:
        case SDLK_d:
            // Toggle or adjust value
            if (_Game->optionsSelection == 0)  // Menu Music
            {
                _Game->settings.menuMusicEnabled = !_Game->settings.menuMusicEnabled;

                // Apply immediately
                if (_Audio)
                {
                    AudioSystem *audio = (AudioSystem *)_Audio;
                    if (_Game->settings.menuMusicEnabled)
                    {
                        if (!Mix_PlayingMusic() && audio->menuMusic)
                        {
                            Mix_PlayMusic((Mix_Music *)audio->menuMusic, -1);
                        }
                    }
                    else
                    {
                        Mix_HaltMusic();
                    }
                }
            }
            else if (_Game->optionsSelection == 1)  // Game Music
            {
                _Game->settings.gameMusicEnabled = !_Game->settings.gameMusicEnabled;
            }
            else if (_Game->optionsSelection == 2)  // Grid Alpha
            {
                if (_Event->key.keysym.sym == SDLK_LEFT || _Event->key.keysym.sym == SDLK_a)
                {
                    _Game->settings.gridAlpha--;
                    if (_Game->settings.gridAlpha < 0)
                        _Game->settings.gridAlpha = 0;
                }
                else
                {
                    _Game->settings.gridAlpha++;
                    if (_Game->settings.gridAlpha > 10)
                        _Game->settings.gridAlpha = 10;
                }
            }
            else if (_Game->optionsSelection == 3)  // Brightness
            {
                if (_Event->key.keysym.sym == SDLK_LEFT || _Event->key.keysym.sym == SDLK_a)
                {
                    _Game->settings.brightness -= 0.1f;
                    if (_Game->settings.brightness < 0.5f)
                        _Game->settings.brightness = 0.5f;
                }
                else
                {
                    _Game->settings.brightness += 0.1f;
                    if (_Game->settings.brightness > 2.0f)
                        _Game->settings.brightness = 2.0f;
                }
            }
            break;
        case SDLK_RETURN:
        case SDLK_SPACE:
            if (_Game->optionsSelection == 4)  // Reset to defaults
            {
                Settings_Init(&_Game->settings);
                Settings_Save(&_Game->settings);

                // Apply menu music setting immediately
                if (_Audio)
                {
                    AudioSystem *audio = (AudioSystem *)_Audio;
                    if (_Game->settings.menuMusicEnabled)
                    {
                        if (!Mix_PlayingMusic() && audio->menuMusic)
                        {
                            Mix_PlayMusic((Mix_Music *)audio->menuMusic, -1);
                        }
                    }
                    else
                    {
                        Mix_HaltMusic();
                    }
                }
            }
            break;
        case SDLK_ESCAPE:
            Settings_Save(&_Game->settings);
            _Game->state = GAME_MAIN_MENU;
            break;
        }
    }

    (void)_Audio;  // Reserved for audio volume updates
}

void Input_HandleMultiplayerInput(MultiplayerContext *_MpCtx, SDL_Event *_Event, Game *_Game)
{
    if (!_MpCtx)
        return;

    // Handle text input for chat and nick change
    if (_Event->type == SDL_TEXTINPUT)
    {
        if (_MpCtx->state == MP_STATE_CHATTING && _MpCtx->chatInputLen < 127)
        {
            size_t len = strlen(_Event->text.text);
            if (_MpCtx->chatInputLen + len < 127)
            {
                strcat(_MpCtx->chatInput, _Event->text.text);
                _MpCtx->chatInputLen += len;
            }
        }
        else if (_MpCtx->state == MP_STATE_CHANGING_NICK && _MpCtx->nickInputLen < 31)
        {
            size_t len = strlen(_Event->text.text);
            if (_MpCtx->nickInputLen + len < 31)
            {
                strcat(_MpCtx->nickInput, _Event->text.text);
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
                    // Set the background to match the game we're joining
                    _MpCtx->selectedBackground = _MpCtx->browsedGames[_MpCtx->selectedGameIndex].mapId;
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
                break;
            case SDLK_n:
                // Enter nick change mode
                _MpCtx->previousState = MP_STATE_LOBBY;
                _MpCtx->state = MP_STATE_CHANGING_NICK;
                _MpCtx->nickInputLen = 0;
                memset(_MpCtx->nickInput, 0, sizeof(_MpCtx->nickInput));
                // Pre-fill with current name
                int localIdx = Multiplayer_GetLocalPlayerIndex(_MpCtx);
                if (localIdx >= 0)
                {
                    strncpy(_MpCtx->nickInput, _MpCtx->players[localIdx].name, sizeof(_MpCtx->nickInput) - 1);
                    _MpCtx->nickInputLen = strlen(_MpCtx->nickInput);
                }
                break;
            case SDLK_SPACE:
                Multiplayer_ToggleReady(_MpCtx);
                break;
            case SDLK_RETURN:
                if (_MpCtx->isHost && Multiplayer_AllReady(_MpCtx))
                {
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
                            strcpy(_MpCtx->chatMessages[i], _MpCtx->chatMessages[i + 1]);
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
                else  // QUIT GAME selected
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
            case SDLK_ESCAPE:
                // Broadcast disconnect to other player
                if (_MpCtx->api)
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

void Input_HandleInput(Game *_Game, SDL_Event *_Event, MultiplayerContext **_MpCtx, Scoreboard *_Scoreboard, void *_Audio)
{
    MultiplayerContext *mpCtx = _MpCtx ? *_MpCtx : NULL;

    if (_Game->state == GAME_MAIN_MENU)
    {
        Input_HandleMainMenuInput(_Game, _Event, _MpCtx);
    }
    else if (_Game->state == GAME_NAME_INPUT)
    {
        Input_HandleNameInput(_Game, _Event);
    }
    else if (_Game->state == GAME_MENU)
    {
        Input_HandleGameMenuInput(_Game, _Event);
    }
    else if (_Game->state == GAME_PLAYING)
    {
        Input_HandlePlayingInput(_Game, _Event);
    }
    else if (_Game->state == GAME_OVER)
    {
        Input_HandleGameOverInput(_Game, _Event);
    }
    else if (_Game->state == GAME_SCOREBOARD)
    {
        Input_HandleScoreboardInput(_Game, _Event);
    }
    else if (_Game->state == GAME_OPTIONS)
    {
        Input_HandleOptionsInput(_Game, _Event, _Audio);
    }
    else if (_Game->state == GAME_MULTIPLAYER && mpCtx)
    {
        // Handle ESC to return to main menu from multiplayer menu
        if (mpCtx->state == MP_STATE_MENU && _Event->type == SDL_KEYDOWN &&
            _Event->key.keysym.sym == SDLK_ESCAPE)
        {
            _Game->state = GAME_MAIN_MENU;
        }
        // Handle ESC to cancel hosting and return to main menu
        else if ((mpCtx->state == MP_STATE_HOSTING || mpCtx->state == MP_STATE_LOBBY) &&
                 mpCtx->isHost && _Event->type == SDL_KEYDOWN &&
                 _Event->key.keysym.sym == SDLK_ESCAPE)
        {
            // Clean up multiplayer and return to main menu
            if (mpCtx->api)
            {
                // Leave session properly
                Multiplayer_Destroy(*_MpCtx);
                *_MpCtx = NULL;
            }
            _Game->state = GAME_MAIN_MENU;
        }
        else
        {
            Input_HandleMultiplayerInput(mpCtx, _Event, _Game);

            // Check if player wants to disconnect and clean up
            if (mpCtx && mpCtx->state == MP_STATE_DISCONNECTED)
            {
                Multiplayer_Destroy(*_MpCtx);
                *_MpCtx = NULL;
                _Game->state = GAME_MAIN_MENU;
            }
        }
    }

    (void)_Scoreboard;  // Reserved for future use
}
