#ifndef __MP_TURN_BATTLE_H_
#define __MP_TURN_BATTLE_H_

#include "multiplayer.h"

/* Turn battle mode - specific logic for turn-based multiplayer */

/* Start turn battle mode */
void MP_TurnBattle_Start(MultiplayerContext *_Ctx);

/* Start a new turn attempt */
void MP_TurnBattle_StartAttempt(MultiplayerContext *_Ctx, int _AttemptNumber);

/* Finish current turn attempt */
void MP_TurnBattle_FinishAttempt(MultiplayerContext *_Ctx);

/* Calculate and show turn battle results */
void MP_TurnBattle_ShowResults(MultiplayerContext *_Ctx);

/* Move to next results page */
void MP_TurnBattle_NextResultPage(MultiplayerContext *_Ctx);

/* Move to previous results page */
void MP_TurnBattle_PrevResultPage(MultiplayerContext *_Ctx);

#endif
