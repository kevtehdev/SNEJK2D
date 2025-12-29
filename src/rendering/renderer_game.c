#include "../../include/rendering/renderer_game.h"
#include "../../include/config.h"
#include <SDL2/SDL.h>
#include <math.h>

/* Draw walls around playfield */
void Renderer_DrawWalls(Renderer *_Renderer)
{
    if (!_Renderer || !_Renderer->sdlRenderer)
        return;

    /* Draw walls as gray blocks (simple and safe) */
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 100, 100, 100, 255);

    for (int x = -1; x <= GRID_WIDTH; x++)
    {
        for (int y = -1; y <= GRID_HEIGHT; y++)
        {
            /* Only draw on borders */
            if (x != -1 && x != GRID_WIDTH && y != -1 && y != GRID_HEIGHT)
                continue;

            SDL_Rect dest = {
                x * CELL_SIZE,
                y * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE};

            SDL_RenderFillRect(_Renderer->sdlRenderer, &dest);
        }
    }
}

/* Draw grid overlay */
void Renderer_DrawGrid(Renderer *_Renderer, int _GridAlpha)
{
    // Skip if grid is disabled
    if (_GridAlpha == 0)
        return;

    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer,
                           COLOR_GRID_R, COLOR_GRID_G, COLOR_GRID_B, _GridAlpha);

    /* Vertical lines */
    for (int x = 0; x <= GRID_WIDTH; x++)
    {
        int px = x * CELL_SIZE;
        SDL_RenderDrawLine(_Renderer->sdlRenderer, px, 0, px, GRID_HEIGHT * CELL_SIZE);
    }

    /* Horizontal lines */
    for (int y = 0; y <= GRID_HEIGHT; y++)
    {
        int py = y * CELL_SIZE;
        SDL_RenderDrawLine(_Renderer->sdlRenderer, 0, py, GRID_WIDTH * CELL_SIZE, py);
    }
}

/* Draw snake as simple colored squares */
void Renderer_DrawSnake(Renderer *_Renderer, Snake *_Snake)
{
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer,
                           COLOR_SNAKE_R, COLOR_SNAKE_G, COLOR_SNAKE_B, 255);

    for (int i = 0; i < _Snake->length; i++)
    {
        SDL_Rect rect = {
            _Snake->segments[i].x * CELL_SIZE,
            _Snake->segments[i].y * CELL_SIZE,
            CELL_SIZE,
            CELL_SIZE};

        SDL_RenderFillRect(_Renderer->sdlRenderer, &rect);
    }
}

/* Draw snake with custom color */
void Renderer_DrawSnakeColored(Renderer *_Renderer, Snake *_Snake, int _R, int _G, int _B)
{
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, _R, _G, _B, 255);

    for (int i = 0; i < _Snake->length; i++)
    {
        SDL_Rect rect = {
            _Snake->segments[i].x * CELL_SIZE,
            _Snake->segments[i].y * CELL_SIZE,
            CELL_SIZE,
            CELL_SIZE};

        SDL_RenderFillRect(_Renderer->sdlRenderer, &rect);
    }
}

/* Draw food as simple colored square */
void Renderer_DrawFood(Renderer *_Renderer, Position *_Food)
{
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer,
                           COLOR_FOOD_R, COLOR_FOOD_G, COLOR_FOOD_B, 255);

    SDL_Rect rect = {
        _Food->x * CELL_SIZE,
        _Food->y * CELL_SIZE,
        CELL_SIZE,
        CELL_SIZE};

    SDL_RenderFillRect(_Renderer->sdlRenderer, &rect);
}

/* Draw power-up with special colors and effects */
void Renderer_DrawPowerUp(Renderer *_Renderer, PowerUp *_PowerUp)
{
    if (!_PowerUp->active)
        return;

    SDL_Rect rect = {
        _PowerUp->position.x * CELL_SIZE,
        _PowerUp->position.y * CELL_SIZE,
        CELL_SIZE,
        CELL_SIZE};

    unsigned int time = SDL_GetTicks();
    unsigned int age = time - _PowerUp->spawnTime;
    float pulse = (sinf(time * 6.0f / 1000.0f) + 1.0f) * 0.5f; // Pulse effect

    // Check if power-up is about to decay (flash warning)
    bool isDecaying = (age >= (POWERUP_DECAY_TIME - POWERUP_DECAY_WARNING));
    bool flashVisible = true;
    if (isDecaying)
    {
        // Fast flashing in last 3 seconds
        int flashSpeed = 150; // ms per flash
        flashVisible = ((time / flashSpeed) % 2) == 0;
    }

    if (!flashVisible)
        return; // Don't draw during flash-off phase

    switch (_PowerUp->type)
    {
    case POWERUP_GOLDEN_APPLE:
        // Gold/yellow with sparkle
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 255, 215, 0, 255);
        SDL_RenderFillRect(_Renderer->sdlRenderer, &rect);
        // Inner highlight
        SDL_Rect inner = {rect.x + 4, rect.y + 4, rect.w - 8, rect.h - 8};
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 255, 255, 100, (int)(pulse * 200));
        SDL_RenderFillRect(_Renderer->sdlRenderer, &inner);
        break;

    case POWERUP_SPEED_BOOST:
        // Cyan lightning
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 255, 255, 255);
        SDL_RenderFillRect(_Renderer->sdlRenderer, &rect);
        // Flashing white effect
        if ((int)(pulse * 2) % 2 == 0)
        {
            SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 255, 255, 255, 200);
            SDL_RenderFillRect(_Renderer->sdlRenderer, &rect);
        }
        break;

    case POWERUP_SCORE_MULTIPLIER:
        // Purple/magenta gem
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 255, 0, 255, 255);
        SDL_RenderFillRect(_Renderer->sdlRenderer, &rect);
        // Diamond pattern
        SDL_Point points[5] = {
            {rect.x + CELL_SIZE / 2, rect.y + 2},
            {rect.x + CELL_SIZE - 2, rect.y + CELL_SIZE / 2},
            {rect.x + CELL_SIZE / 2, rect.y + CELL_SIZE - 2},
            {rect.x + 2, rect.y + CELL_SIZE / 2},
            {rect.x + CELL_SIZE / 2, rect.y + 2}};
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 255, 100, 255, (int)(pulse * 255));
        SDL_RenderDrawLines(_Renderer->sdlRenderer, points, 5);
        break;

    default:
        break;
    }
}
