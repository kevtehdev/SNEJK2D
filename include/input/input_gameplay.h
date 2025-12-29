#ifndef __INPUT_GAMEPLAY_H_
#define __INPUT_GAMEPLAY_H_

#include "../core/game.h"
#include <SDL2/SDL.h>

/* Handle input during active gameplay (snake movement, pause, volume) */
void Input_HandlePlayingInput(Game *_Game, SDL_Event *_Event, void *_Audio);

#endif
