#include "../../include/rendering/renderer_background.h"
#include "../../include/config.h"
#include <SDL2/SDL.h>

/* Draw parallax background */
void Renderer_DrawBackground(Renderer *_Renderer, unsigned int _Tick)
{
    (void)_Tick; /* Reserved for future time-based animation */

    /* Safety check for background index */
    if (_Renderer->currentBackground >= NUM_BACKGROUNDS)
        return;

    ParallaxBackground *bg = &_Renderer->backgrounds[_Renderer->currentBackground];

    /* Update scroll offset based on time */
    bg->scrollOffset += 0.5f; /* Slow scroll speed */

    for (int i = 0; i < bg->numLayers; i++)
    {
        if (!bg->layers[i])
            continue;

        int origW, origH;
        SDL_QueryTexture(bg->layers[i], NULL, NULL, &origW, &origH);

        /* Safety check */
        if (origH <= 0 || origW <= 0)
            continue;

        /* Scale to window size (maintain aspect ratio height-wise, tile width-wise) */
        float scale = (float)WINDOW_HEIGHT / (float)origH;
        int scaledW = (int)(origW * scale);
        int scaledH = WINDOW_HEIGHT;

        /* Safety check for scaled dimensions */
        if (scaledW <= 0 || scaledH <= 0)
            continue;

        /* Calculate scroll position for this layer */
        float layerScroll = bg->scrollOffset * bg->scrollSpeeds[i];
        int xOffset = -(int)layerScroll % scaledW;

        /* Draw layer multiple times to fill width */
        for (int x = xOffset; x < WINDOW_WIDTH; x += scaledW)
        {
            SDL_Rect dest = {x, 0, scaledW, scaledH};
            SDL_RenderCopy(_Renderer->sdlRenderer, bg->layers[i], NULL, &dest);
        }
    }
}

/* Draw background overlay */
void Renderer_DrawBackgroundOverlay(Renderer *_Renderer)
{
    if (!_Renderer || !_Renderer->sdlRenderer)
        return;

    /* Draw black semi-transparent overlay over entire screen */
    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, BACKGROUND_OVERLAY_ALPHA);

    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &overlay);
}

/* Draw brightness overlay */
void Renderer_DrawBrightnessOverlay(Renderer *_Renderer, float _Brightness)
{
    if (!_Renderer || !_Renderer->sdlRenderer)
        return;

    // Skip if brightness is at default
    if (_Brightness == 1.0f)
        return;

    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);

    if (_Brightness > 1.0f)
    {
        // Brighten with white overlay
        float intensity = (_Brightness - 1.0f) / 1.0f; // 0.0 to 1.0
        int alpha = (int)(intensity * 200);            // Max 200 alpha for white
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 255, 255, 255, alpha);
    }
    else
    {
        // Darken with black overlay
        float intensity = (1.0f - _Brightness) / 0.5f; // 0.0 to 1.0
        int alpha = (int)(intensity * 200);            // Max 200 alpha for black
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, alpha);
    }

    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &overlay);
}
