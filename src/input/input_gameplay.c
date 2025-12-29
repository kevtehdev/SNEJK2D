#include "../../include/input/input_gameplay.h"
#include "../../include/audio/audio.h"
#include <SDL2/SDL.h>

void Input_HandlePlayingInput(Game *_Game, SDL_Event *_Event, void *_Audio)
{
    AudioSystem *audio = (AudioSystem *)_Audio;

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
        case SDLK_PLUS:
        case SDLK_KP_PLUS:
        case SDLK_EQUALS:  // On many keyboards, + is Shift+Equals
            if (audio)
                Audio_IncreaseVolume(audio);
            break;
        case SDLK_MINUS:
        case SDLK_KP_MINUS:
            if (audio)
                Audio_DecreaseVolume(audio);
            break;
        case SDLK_ESCAPE:
            _Game->state = GAME_PAUSED;
            _Game->pauseMenuSelection = 0; // Default to "Resume Game"
            break;
        }

        if (hasInput)
        {
            Snake_SetDirection(&_Game->snake, dir);
        }
    }
}

