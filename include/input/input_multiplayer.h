#ifndef __INPUT_MULTIPLAYER_H_
#define __INPUT_MULTIPLAYER_H_

#include "../core/game.h"
#include "../multiplayer/multiplayer.h"
#include <SDL2/SDL.h>

/* Handle all multiplayer-related inputs (lobby, chat, gameplay, etc.) */
void Input_HandleMultiplayerInput(MultiplayerContext *_MpCtx, SDL_Event *_Event,
                                  Game *_Game, void *_Audio);

#endif
