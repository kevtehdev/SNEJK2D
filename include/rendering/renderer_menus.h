#ifndef __RENDERER_MENUS_H_
#define __RENDERER_MENUS_H_

#include "../renderer.h"
#include "../core/game.h"
#include "../utils/scoreboard.h"

/* Draw background selection menu */
void Renderer_DrawMenu(Renderer *_Renderer, int _SelectedBg);

/* Draw game mode selection screen */
void Renderer_DrawModeSelect(Renderer *_Renderer, Game *_Game);

/* Draw name input screen */
void Renderer_DrawNameInput(Renderer *_Renderer, Game *_Game, unsigned int _Tick);

/* Draw retro loading screen with progress bar */
void Renderer_DrawLoadingScreen(Renderer *_Renderer, float _Progress, unsigned int _CurrentTime);

/* Draw main menu with options */
void Renderer_DrawMainMenu(Renderer *_Renderer, int _Selection);

/* Draw pause menu */
void Renderer_DrawPauseMenu(Renderer *_Renderer, int _Selection);

/* Draw options/settings menu */
void Renderer_DrawOptions(Renderer *_Renderer, Game *_Game);

/* Draw scoreboard with animated medals */
void Renderer_DrawScoreboard(Renderer *_Renderer, Scoreboard *_Scoreboard, unsigned int _Tick);

#endif
