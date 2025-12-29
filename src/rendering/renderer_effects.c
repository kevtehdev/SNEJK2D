#include "../../include/rendering/renderer_effects.h"
#include "../../include/config.h"
#include <SDL2/SDL.h>

/* Draw explosion effect at position */
void Renderer_DrawExplosion(Renderer *_Renderer, Position _Pos, int _Frame)
{
    if (_Frame < 0 || _Frame >= 7)
        return;

    /* Simple explosion effect: expanding circle */
    int radius = (_Frame + 1) * 5;
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 255, 200 - _Frame * 20, 0, 200 - _Frame * 25);

    int centerX = _Pos.x * CELL_SIZE + CELL_SIZE / 2;
    int centerY = _Pos.y * CELL_SIZE + CELL_SIZE / 2;

    /* Draw filled circle approximation */
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if (x * x + y * y <= radius * radius)
            {
                SDL_RenderDrawPoint(_Renderer->sdlRenderer, centerX + x, centerY + y);
            }
        }
    }
}
