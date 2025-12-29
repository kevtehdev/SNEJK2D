#include "../../include/multiplayer/mp_host.h"
#include "../../include/multiplayer/multiplayer.h"
#include <stdio.h>

/* Host operations module - wraps host functions from mp_core.c */
/* TODO: Gradually move implementation from mp_core.c to here */

int MP_Host_Create(MultiplayerContext *_Ctx, const char *_PlayerName)
{
    return Multiplayer_Host(_Ctx, _PlayerName);
}

void MP_Host_Update(MultiplayerContext *_Ctx, unsigned int _CurrentTime)
{
    Multiplayer_HostUpdate(_Ctx, _CurrentTime);
}

void MP_Host_BroadcastState(MultiplayerContext *_Ctx)
{
    Multiplayer_HostBroadcastState(_Ctx);
}
