#ifndef __INPUT_MENU_H_
#define __INPUT_MENU_H_

#include "../core/game.h"
#include "../multiplayer/multiplayer.h"
#include <SDL2/SDL.h>

/* Main menu input (singleplayer/multiplayer/scoreboard/options/exit) */
void Input_HandleMainMenuInput(Game *_Game, SDL_Event *_Event, MultiplayerContext **_MpCtx, void *_Audio);

/* Name input screen */
void Input_HandleNameInput(Game *_Game, SDL_Event *_Event);

/* Game mode selection screen (Classic/Power-Up) */
void Input_HandleModeSelectInput(Game *_Game, SDL_Event *_Event);

/* In-game menu (background selection) */
void Input_HandleGameMenuInput(Game *_Game, SDL_Event *_Event);

/* Game over screen input */
void Input_HandleGameOverInput(Game *_Game, SDL_Event *_Event);

/* Pause menu input */
void Input_HandlePauseMenuInput(Game *_Game, SDL_Event *_Event);

/* Scoreboard screen input */
void Input_HandleScoreboardInput(Game *_Game, SDL_Event *_Event);

/* Options/settings screen input */
void Input_HandleOptionsInput(Game *_Game, SDL_Event *_Event, void *_Audio);

#endif
