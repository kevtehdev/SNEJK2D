#ifndef __RENDERER_MULTIPLAYER_H_
#define __RENDERER_MULTIPLAYER_H_

#include "../renderer.h"
#include "../multiplayer/multiplayer.h"

/* Draw multiplayer main menu (host/join selection) */
void Renderer_DrawMultiplayerMenu(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw multiplayer room name input screen */
void Renderer_DrawMultiplayerRoomName(Renderer *_Renderer, MultiplayerContext *_MpCtx, unsigned int _CurrentTime);

/* Draw multiplayer room browser */
void Renderer_DrawMultiplayerBrowse(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw multiplayer lobby (waiting for players) */
void Renderer_DrawMultiplayerLobby(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw chat overlay */
void Renderer_DrawChatOverlay(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw nickname overlay */
void Renderer_DrawNickOverlay(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw multiplayer game (1v1 or turn battle gameplay) */
void Renderer_DrawMultiplayerGame(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw game mode selection screen (Classic/Power-Up) */
void Renderer_DrawModeSelection(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw ready-up screen for turn battle */
void Renderer_DrawReadyUp(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw turn battle playing screen */
void Renderer_DrawTurnPlaying(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw turn battle waiting screen */
void Renderer_DrawTurnWaiting(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw turn battle results screen */
void Renderer_DrawTurnResults(Renderer *_Renderer, MultiplayerContext *_MpCtx);

#endif
