#ifndef __RENDERER_HUD_H_
#define __RENDERER_HUD_H_

#include "../renderer.h"
#include "../core/game.h"
#include "../multiplayer/multiplayer.h"

/* Draw HUD border with score or attempt counter */
void Renderer_DrawHudBorderWithScore(Renderer *_Renderer, int _Score, int _Attempt, bool _ShowAttempt);

/* Draw HUD border with combo, XP bar, and level */
void Renderer_DrawHudBorderWithCombo(Renderer *_Renderer, int _Score, int _Combo, float _Multiplier, GameMode _GameMode, bool _ComboEffects, int _Level, int _XP, int _XPToNextLevel);

/* Draw in-game HUD (score, combo, XP) */
void Renderer_DrawHud(Renderer *_Renderer, Game *_Game);

/* Draw multiplayer HUD border (for multiplayer games) */
void Renderer_DrawMultiplayerHudBorder(Renderer *_Renderer, MultiplayerContext *_MpCtx);

#endif
