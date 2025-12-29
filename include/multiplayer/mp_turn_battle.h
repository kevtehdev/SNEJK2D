/*
 * mp_turn_battle.h - Turn battle mode implementation
 * Part of SNEJK2D multiplayer refactoring (Phase 4)
 */

#ifndef MP_TURN_BATTLE_H
#define MP_TURN_BATTLE_H

#include "multiplayer.h"
#include <stdbool.h>

/* Start a turn battle attempt (1 of 3) */
void MP_TurnBattle_StartAttempt(MultiplayerContext *_Ctx);

/* Finish current turn attempt and record results */
void MP_TurnBattle_FinishAttempt(MultiplayerContext *_Ctx);

/* Submit turn battle results to host */
void MP_TurnBattle_SubmitResults(MultiplayerContext *_Ctx);

/* Check if all players finished their turns */
bool MP_TurnBattle_AllFinished(MultiplayerContext *_Ctx);

/* Calculate turn battle winner based on best scores */
void MP_TurnBattle_CalculateWinner(MultiplayerContext *_Ctx);

#endif /* MP_TURN_BATTLE_H */
