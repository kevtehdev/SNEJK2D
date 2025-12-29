#include "../../include/multiplayer/mp_client.h"
#include "../../include/multiplayer/multiplayer.h"
#include <stdio.h>

/* Client operations module - wraps client functions from mp_core.c */
/* TODO: Gradually move implementation from mp_core.c to here */

int MP_Client_Join(MultiplayerContext *_Ctx, const char *_SessionId, const char *_PlayerName)
{
    return Multiplayer_Join(_Ctx, _SessionId, _PlayerName);
}

void MP_Client_SendInput(MultiplayerContext *_Ctx, Direction _Dir)
{
    Multiplayer_ClientSendInput(_Ctx, _Dir);
}

void MP_Client_SendPositionUpdate(MultiplayerContext *_Ctx)
{
    Multiplayer_SendQuickPositionUpdate(_Ctx);
}
