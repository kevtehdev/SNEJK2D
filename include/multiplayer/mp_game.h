#ifndef __MP_GAME_H_
#define __MP_GAME_H_

#include "multiplayer.h"
#include <stdbool.h>

/* Game control - shared game logic for multiplayer */

/* Start the game */
void MP_Game_Start(MultiplayerContext *_Ctx);

/* Restart after game over */
void MP_Game_Restart(MultiplayerContext *_Ctx);

/* Toggle ready status */
void MP_Game_ToggleReady(MultiplayerContext *_Ctx);

/* Check if all players are ready */
bool MP_Game_AllReady(MultiplayerContext *_Ctx);

/* Get local player index */
int MP_Game_GetLocalPlayerIndex(MultiplayerContext *_Ctx);

/* Update countdown timer */
void MP_Game_UpdateCountdown(MultiplayerContext *_Ctx, unsigned int _CurrentTime);

#endif
