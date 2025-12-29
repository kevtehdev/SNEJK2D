#ifndef __RENDERER_UI_H_
#define __RENDERER_UI_H_

#include "../renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*
 * UI primitive rendering functions
 *
 * This module contains common UI drawing utilities used across
 * menu, HUD, and multiplayer rendering modules.
 *
 * TODO: Extract common UI patterns (text rendering, button drawing,
 * box drawing, etc.) into this module during code extraction phase.
 */

/* Main rendering dispatcher - handles all game states */
void Renderer_DrawFrame(Renderer *_Renderer, Game *_Game, MultiplayerContext *_MpCtx,
                        Scoreboard *_Scoreboard, unsigned int _CurrentTime,
                        int _MainMenuSelection, void *_Audio);

#endif
