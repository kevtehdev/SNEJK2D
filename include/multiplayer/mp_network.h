#ifndef __MP_NETWORK_H_
#define __MP_NETWORK_H_

#include "multiplayer.h"
#include "../../src/mpapi/c_client/libs/jansson/jansson.h"

/* Network layer - handles all mpapi communication */

/* Initialize network callback */
void MP_Network_Init(MultiplayerContext *_Ctx);

/* Cleanup network resources */
void MP_Network_Cleanup(MultiplayerContext *_Ctx);

/* Serialization */
json_t* MP_Network_SerializeState(MultiplayerContext *_Ctx);
void MP_Network_DeserializeState(MultiplayerContext *_Ctx, json_t *_Data);

/* Game browsing */
int MP_Network_BrowseGames(MultiplayerContext *_Ctx);

#endif
