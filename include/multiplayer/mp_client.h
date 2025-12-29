#ifndef __MP_CLIENT_H_
#define __MP_CLIENT_H_

#include "multiplayer.h"
#include "../entities/snake.h"

/* Client operations - client-side multiplayer logic */

/* Join an existing game session */
int MP_Client_Join(MultiplayerContext *_Ctx, const char *_SessionId, const char *_PlayerName);

/* Send input to host */
void MP_Client_SendInput(MultiplayerContext *_Ctx, Direction _Dir);

/* Send quick position update */
void MP_Client_SendPositionUpdate(MultiplayerContext *_Ctx);

#endif
