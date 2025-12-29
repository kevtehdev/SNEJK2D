/*
 * mp_game.h - Game state management for multiplayer
 * Part of SNEJK2D multiplayer refactoring (Phase 4)
 */

#ifndef MP_GAME_H
#define MP_GAME_H

#include "multiplayer.h"
#include <stdbool.h>

/* Start the multiplayer game (host only) */
void MP_Game_Start(MultiplayerContext *_Ctx);

/* Restart game after game over (host only) */
void MP_Game_Restart(MultiplayerContext *_Ctx);

/* Update countdown and initialize game when countdown finishes */
void MP_Game_UpdateCountdown(MultiplayerContext *_Ctx, unsigned int _CurrentTime);

/* Check if all players are ready */
bool MP_Game_AllReady(MultiplayerContext *_Ctx);

/* Get the local player's index */
int MP_Game_GetLocalPlayerIndex(MultiplayerContext *_Ctx);

#endif /* MP_GAME_H */
