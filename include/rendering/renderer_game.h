#ifndef __RENDERER_GAME_H_
#define __RENDERER_GAME_H_

#include "../renderer.h"
#include "../core/game.h"

/* Draw walls around the playfield */
void Renderer_DrawWalls(Renderer *_Renderer);

/* Draw grid overlay on the playfield */
void Renderer_DrawGrid(Renderer *_Renderer, int _GridAlpha);

/* Draw snake with default color */
void Renderer_DrawSnake(Renderer *_Renderer, Snake *_Snake);

/* Draw snake with custom RGB color */
void Renderer_DrawSnakeColored(Renderer *_Renderer, Snake *_Snake, int _R, int _G, int _B);

/* Draw food (red apple) */
void Renderer_DrawFood(Renderer *_Renderer, Position *_Food);

/* Draw power-up (golden apple, speed boost, multiplier) */
void Renderer_DrawPowerUp(Renderer *_Renderer, PowerUp *_PowerUp);

#endif
