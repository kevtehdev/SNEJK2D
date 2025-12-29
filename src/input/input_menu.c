#include "../../include/input/input_menu.h"
#include "../../include/input.h"  // For g_main_menu_selection and g_score_saved
#include "../../include/audio/audio.h"
#include "../../include/utils/settings.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>

void Input_HandleMainMenuInput(Game *_Game, SDL_Event *_Event, MultiplayerContext **_MpCtx, void *_Audio)
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
                // Singleplayer - go to mode selection first
                _Game->state = GAME_MODE_SELECT;
            }
            else if (g_main_menu_selection == 1)
            {
                _Game->state = GAME_MULTIPLAYER;
                if (!*_MpCtx)
                {
                    *_MpCtx = Multiplayer_Create();
                    if (*_MpCtx)
                    {
                        (*_MpCtx)->audio = _Audio;
                    }
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
            _Game->state = GAME_MODE_SELECT; // Go back to mode select
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

void Input_HandleModeSelectInput(Game *_Game, SDL_Event *_Event)
{
    if (_Event->type == SDL_KEYDOWN)
    {
        switch (_Event->key.keysym.sym)
        {
        case SDLK_UP:
        case SDLK_w:
        case SDLK_LEFT:
            _Game->modeSelection = 0; // Classic
            break;
        case SDLK_DOWN:
        case SDLK_s:
        case SDLK_RIGHT:
            _Game->modeSelection = 1; // Power-Up
            break;
        case SDLK_RETURN:
        case SDLK_SPACE:
            // Set game mode and go to name input
            _Game->gameMode = (_Game->modeSelection == 0) ? MODE_CLASSIC : MODE_POWERUP;
            _Game->state = GAME_NAME_INPUT;
            break;
        case SDLK_ESCAPE:
            _Game->state = GAME_MAIN_MENU;
            break;
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

void Input_HandlePauseMenuInput(Game *_Game, SDL_Event *_Event)
{
    if (_Event->type == SDL_KEYDOWN)
    {
        switch (_Event->key.keysym.sym)
        {
        case SDLK_UP:
        case SDLK_w:
            _Game->pauseMenuSelection = 0; // Resume Game
            break;
        case SDLK_DOWN:
        case SDLK_s:
            _Game->pauseMenuSelection = 1; // Exit to Main Menu
            break;
        case SDLK_RETURN:
        case SDLK_SPACE:
            if (_Game->pauseMenuSelection == 0)
            {
                // Resume game
                _Game->state = GAME_PLAYING;
            }
            else
            {
                // Exit to main menu
                _Game->state = GAME_MAIN_MENU;
                _Game->pauseMenuSelection = 0;
            }
            break;
        case SDLK_ESCAPE:
            // ESC also resumes the game
            _Game->state = GAME_PLAYING;
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
            _Game->optionsSelection = (_Game->optionsSelection - 1 + 6) % 6;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            _Game->optionsSelection = (_Game->optionsSelection + 1) % 6;
            break;
        case SDLK_LEFT:
        case SDLK_a:
        case SDLK_RIGHT:
        case SDLK_d:
            // Toggle or adjust value
            if (_Game->optionsSelection == 0) // Menu Music
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
            else if (_Game->optionsSelection == 1) // Game Music
            {
                _Game->settings.gameMusicEnabled = !_Game->settings.gameMusicEnabled;
            }
            else if (_Game->optionsSelection == 2) // Grid Alpha
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
            else if (_Game->optionsSelection == 3) // Brightness
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
            else if (_Game->optionsSelection == 4) // Combo Effects
            {
                _Game->settings.comboEffects = !_Game->settings.comboEffects;
            }
            break;
        case SDLK_RETURN:
        case SDLK_SPACE:
            if (_Game->optionsSelection == 5) // Reset to defaults
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

    (void)_Audio; // Reserved for audio volume updates
}

