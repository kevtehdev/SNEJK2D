#include "../../include/multiplayer/mp_turn_battle.h"
#include "../../include/multiplayer/multiplayer.h"
#include <stdio.h>

/* Turn battle module - wraps turn battle functions from mp_core.c */
/* TODO: Gradually move implementation from mp_core.c to here */

void MP_TurnBattle_Start(MultiplayerContext *_Ctx)
{
    // Turn battle start logic is in Multiplayer_StartGame
    // This is a placeholder for future refactoring
    (void)_Ctx;
}

void MP_TurnBattle_StartAttempt(MultiplayerContext *_Ctx, int _AttemptNumber)
{
    (void)_AttemptNumber;
    Multiplayer_StartTurnAttempt(_Ctx);
}

void MP_TurnBattle_FinishAttempt(MultiplayerContext *_Ctx)
{
    Multiplayer_FinishTurnAttempt(_Ctx);
}

void MP_TurnBattle_ShowResults(MultiplayerContext *_Ctx)
{
    Multiplayer_CalculateTurnWinner(_Ctx);
}

void MP_TurnBattle_NextResultPage(MultiplayerContext *_Ctx)
{
    if (!_Ctx)
        return;

    // Logic for navigating result pages
    // Currently embedded in input handling
    _Ctx->resultPageIndex++;
    if (_Ctx->resultPageIndex >= MAX_MULTIPLAYER_PLAYERS)
        _Ctx->resultPageIndex = MAX_MULTIPLAYER_PLAYERS - 1;
}

void MP_TurnBattle_PrevResultPage(MultiplayerContext *_Ctx)
{
    if (!_Ctx)
        return;

    _Ctx->resultPageIndex--;
    if (_Ctx->resultPageIndex < 0)
        _Ctx->resultPageIndex = 0;
}
