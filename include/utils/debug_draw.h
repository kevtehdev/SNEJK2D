#ifndef __DEBUG_DRAW_H_
#define __DEBUG_DRAW_H_

#include "../renderer.h"
#include "../core/game.h"
#include "../core/event_system.h"
#include "../multiplayer/multiplayer.h"
#include <stdbool.h>

/* Debug visualization system - helps with development and debugging */

typedef struct {
    bool showGrid;
    bool showHitboxes;
    bool showFPS;
    bool showPlayerInfo;
    bool showEventStats;
    bool showNetworkStats;
    bool enabled;
} DebugDrawConfig;

/* Initialize debug draw system */
void DebugDraw_Init(DebugDrawConfig *_Config);

/* Toggle debug features */
void DebugDraw_ToggleGrid(DebugDrawConfig *_Config);
void DebugDraw_ToggleHitboxes(DebugDrawConfig *_Config);
void DebugDraw_ToggleFPS(DebugDrawConfig *_Config);
void DebugDraw_ToggleAll(DebugDrawConfig *_Config);

/* Draw debug overlays */
void DebugDraw_Grid(Renderer *_Renderer, DebugDrawConfig *_Config);
void DebugDraw_Hitboxes(Renderer *_Renderer, Game *_Game, DebugDrawConfig *_Config);
void DebugDraw_FPS(Renderer *_Renderer, DebugDrawConfig *_Config, unsigned int _CurrentTime);
void DebugDraw_PlayerInfo(Renderer *_Renderer, Game *_Game, DebugDrawConfig *_Config);
void DebugDraw_EventStats(Renderer *_Renderer, EventDispatcher *_Dispatcher, DebugDrawConfig *_Config);
void DebugDraw_NetworkStats(Renderer *_Renderer, MultiplayerContext *_MpCtx, DebugDrawConfig *_Config);

/* Main debug draw function - draws all enabled overlays */
void DebugDraw_Render(Renderer *_Renderer, Game *_Game, MultiplayerContext *_MpCtx,
                      DebugDrawConfig *_Config, unsigned int _CurrentTime);

#endif
