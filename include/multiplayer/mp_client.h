/*
 * mp_client.h - Client-side multiplayer operations
 * Part of SNEJK2D multiplayer refactoring (Phase 4)
 */

#ifndef MP_CLIENT_H
#define MP_CLIENT_H

#include "multiplayer.h"

/* Join an existing multiplayer session */
int MP_Client_Join(MultiplayerContext *_Ctx, const char *_SessionId, const char *_PlayerName);

/* Send input direction to server */
void MP_Client_SendInput(MultiplayerContext *_Ctx, Direction _Dir);

/* Toggle local player's ready status */
void MP_Client_ToggleReady(MultiplayerContext *_Ctx);

#endif /* MP_CLIENT_H */
