#ifndef __RENDERER_H_
#define __RENDERER_H_

#include "config.h"
#include "game.h"
#include "multiplayer.h"
#include "scoreboard.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

/* Parallax background layer */
typedef struct {
    SDL_Texture *layers[6];  /* Max 6 layers per background */
    float scrollSpeeds[6];
    int numLayers;
    float scrollOffset;
} ParallaxBackground;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *sdlRenderer;

    /* Backgrounds */
    ParallaxBackground backgrounds[NUM_BACKGROUNDS];
    int currentBackground;

    /* Wall textures (keep from old assets) */
    SDL_Texture *wallBlocks[8];

    /* Explosion frames (keep from old assets) */
    SDL_Texture *explosionFrames[7];

    /* Medal animations (13 frames each) */
    SDL_Texture *medalPlatinum[MEDAL_FRAMES];
    SDL_Texture *medalGold[MEDAL_FRAMES];
    SDL_Texture *medalSilver[MEDAL_FRAMES];
    SDL_Texture *medalBronze[MEDAL_FRAMES];

    /* Fonts */
    TTF_Font *fontLarge;
    TTF_Font *fontMedium;
    TTF_Font *fontSmall;

    bool initialized;
} Renderer;

/* Initialize renderer */
bool Renderer_Init(Renderer *_Renderer);

/* Cleanup renderer */
void Renderer_Cleanup(Renderer *_Renderer);

/* Clear screen */
void Renderer_Clear(Renderer *_Renderer);

/* Present frame */
void Renderer_Present(Renderer *_Renderer);

/* Draw parallax background */
void Renderer_DrawBackground(Renderer *_Renderer, unsigned int _Tick);

/* Draw background overlay */
void Renderer_DrawBackgroundOverlay(Renderer *_Renderer);

/* Draw HUD border with score */
void Renderer_DrawHudBorderWithScore(Renderer *_Renderer, int _Score);

/* Draw HUD border with combo */
void Renderer_DrawHudBorderWithCombo(Renderer *_Renderer, int _Score, int _Combo, float _Multiplier);

/* Draw walls around playfield */
void Renderer_DrawWalls(Renderer *_Renderer);

/* Draw grid overlay */
void Renderer_DrawGrid(Renderer *_Renderer);

/* Draw snake */
void Renderer_DrawSnake(Renderer *_Renderer, Snake *_Snake);

/* Draw snake with custom color */
void Renderer_DrawSnakeColored(Renderer *_Renderer, Snake *_Snake, int _R, int _G, int _B);

/* Draw food */
void Renderer_DrawFood(Renderer *_Renderer, Position *_Food);

/* Draw HUD */
void Renderer_DrawHud(Renderer *_Renderer, Game *_Game);

/* Draw menu */
void Renderer_DrawMenu(Renderer *_Renderer, int _SelectedBg);

/* Draw name input screen */
void Renderer_DrawNameInput(Renderer *_Renderer, Game *_Game, unsigned int _Tick);

/* Draw main menu */
void Renderer_DrawMainMenu(Renderer *_Renderer, int _Selection);

/* Draw explosion at position */
void Renderer_DrawExplosion(Renderer *_Renderer, Position _Pos, int _Frame);

/* Draw scoreboard with animated medals */
void Renderer_DrawScoreboard(Renderer *_Renderer, Scoreboard *_Scoreboard, unsigned int _Tick);

/* Draw multiplayer menu */
void Renderer_DrawMultiplayerMenu(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw multiplayer room name */
void Renderer_DrawMultiplayerRoomName(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw multiplayer input */
void Renderer_DrawMultiplayerInput(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw multiplayer browse */
void Renderer_DrawMultiplayerBrowse(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw multiplayer lobby */
void Renderer_DrawMultiplayerLobby(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw multiplayer game */
void Renderer_DrawMultiplayerGame(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Draw multiplayer HUD border */
void Renderer_DrawMultiplayerHudBorder(Renderer *_Renderer, MultiplayerContext *_MpCtx);

/* Main rendering function - handles all game states */
void Renderer_DrawFrame(Renderer *_Renderer, Game *_Game, MultiplayerContext *_MpCtx, Scoreboard *_Scoreboard, unsigned int _CurrentTime, int _MainMenuSelection);

#endif
