#ifndef __INPUT_CORE_H_
#define __INPUT_CORE_H_

#include "../core/game.h"
#include "../multiplayer/multiplayer.h"
#include "../utils/scoreboard.h"
#include <SDL2/SDL.h>

/* Main input dispatcher - routes input events to appropriate handlers */
void Input_HandleInput(Game *_Game, SDL_Event *_Event, MultiplayerContext **_MpCtx,
                       Scoreboard *_Scoreboard, void *_Audio);

#endif
