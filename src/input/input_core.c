#include "../../include/input/input_core.h"
#include "../../include/input/input_menu.h"
#include "../../include/input/input_gameplay.h"
#include "../../include/input/input_multiplayer.h"
#include <SDL2/SDL.h>

#include "../include/input.h"
#include "../../include/audio/audio.h"
#include "../../include/multiplayer/multiplayer.h"
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Global state
int g_main_menu_selection = 0;
bool g_score_saved = false;

void Input_HandleInput(Game *_Game, SDL_Event *_Event, MultiplayerContext **_MpCtx, Scoreboard *_Scoreboard, void *_Audio)
{
    MultiplayerContext *mpCtx = _MpCtx ? *_MpCtx : NULL;

    if (_Game->state == GAME_MAIN_MENU)
    {
        Input_HandleMainMenuInput(_Game, _Event, _MpCtx, _Audio);
    }
    else if (_Game->state == GAME_MODE_SELECT)
    {
        Input_HandleModeSelectInput(_Game, _Event);
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
        Input_HandlePlayingInput(_Game, _Event, _Audio);
    }
    else if (_Game->state == GAME_OVER)
    {
        Input_HandleGameOverInput(_Game, _Event);
    }
    else if (_Game->state == GAME_PAUSED)
    {
        Input_HandlePauseMenuInput(_Game, _Event);
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
            Input_HandleMultiplayerInput(mpCtx, _Event, _Game, _Audio);

            // Check if player wants to disconnect and clean up
            if (mpCtx && mpCtx->state == MP_STATE_DISCONNECTED)
            {
                Multiplayer_Destroy(*_MpCtx);
                *_MpCtx = NULL;
                _Game->state = GAME_MAIN_MENU;
            }
        }
    }

    (void)_Scoreboard; // Reserved for future use
}
