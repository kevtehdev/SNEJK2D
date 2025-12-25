#ifndef __INPUT_H_
#define __INPUT_H_

#include "game.h"
#include "multiplayer.h"
#include "scoreboard.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

// Main menu selection
extern int g_main_menu_selection;
extern bool g_score_saved;

// Input handlers
void Input_HandleMainMenuInput(Game *_Game, SDL_Event *_Event, MultiplayerContext **_MpCtx);
void Input_HandleNameInput(Game *_Game, SDL_Event *_Event);
void Input_HandleGameMenuInput(Game *_Game, SDL_Event *_Event);
void Input_HandlePlayingInput(Game *_Game, SDL_Event *_Event);
void Input_HandleGameOverInput(Game *_Game, SDL_Event *_Event);
void Input_HandleScoreboardInput(Game *_Game, SDL_Event *_Event);
void Input_HandleMultiplayerInput(MultiplayerContext *_MpCtx, SDL_Event *_Event);
void Input_HandleInput(Game *_Game, SDL_Event *_Event, MultiplayerContext **_MpCtx, Scoreboard *_Scoreboard);

#endif
