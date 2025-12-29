#include "../../include/multiplayer/mp_game.h"
#include "../../include/multiplayer/multiplayer.h"
#include <stdio.h>

/* Game control module - wraps game control functions from mp_core.c */
/* TODO: Gradually move implementation from mp_core.c to here */

void MP_Game_Start(MultiplayerContext *_Ctx)
{
    Multiplayer_StartGame(_Ctx);
}

void MP_Game_Restart(MultiplayerContext *_Ctx)
{
    Multiplayer_RestartGame(_Ctx);
}

void MP_Game_ToggleReady(MultiplayerContext *_Ctx)
{
    Multiplayer_ToggleReady(_Ctx);
}

bool MP_Game_AllReady(MultiplayerContext *_Ctx)
{
    return Multiplayer_AllReady(_Ctx);
}

int MP_Game_GetLocalPlayerIndex(MultiplayerContext *_Ctx)
{
    return Multiplayer_GetLocalPlayerIndex(_Ctx);
}

void MP_Game_UpdateCountdown(MultiplayerContext *_Ctx, unsigned int _CurrentTime)
{
    Multiplayer_UpdateCountdown(_Ctx, _CurrentTime);
}
