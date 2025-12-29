#ifndef __MP_HOST_H_
#define __MP_HOST_H_

#include "multiplayer.h"

/* Host operations - server-side multiplayer logic */

/* Host a new game session */
int MP_Host_Create(MultiplayerContext *_Ctx, const char *_PlayerName);

/* Update host game state (called every frame) */
void MP_Host_Update(MultiplayerContext *_Ctx, unsigned int _CurrentTime);

/* Broadcast current state to all clients */
void MP_Host_BroadcastState(MultiplayerContext *_Ctx);

#endif
