#include "../../include/rendering/renderer_core.h"
#include "../../include/config.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

/* Helper to load texture */
SDL_Texture *Renderer_LoadTexture(SDL_Renderer *_Renderer, const char *_Path)
{
    SDL_Texture *texture = IMG_LoadTexture(_Renderer, _Path);
    if (!texture)
    {
        fprintf(stderr, "Failed to load texture %s: %s\n", _Path, IMG_GetError());
    }
    return texture;
}

/* Initialize renderer */
bool Renderer_Init(Renderer *_Renderer)
{
    /* Create window */
    _Renderer->window = SDL_CreateWindow(
        "SNEJK2D",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

    if (!_Renderer->window)
    {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return false;
    }

    /* Create renderer */
    _Renderer->sdlRenderer = SDL_CreateRenderer(
        _Renderer->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!_Renderer->sdlRenderer)
    {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return false;
    }

    /* Initialize SDL_image */
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(stderr, "Failed to init SDL_image: %s\n", IMG_GetError());
        return false;
    }

    /* Initialize SDL_ttf */
    if (TTF_Init() < 0)
    {
        fprintf(stderr, "Failed to init SDL_ttf: %s\n", TTF_GetError());
        return false;
    }

    /* Load fonts */
    _Renderer->fontLarge = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", 24);
    _Renderer->fontMedium = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", 16);
    _Renderer->fontSmall = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", 12);

    if (!_Renderer->fontLarge || !_Renderer->fontMedium || !_Renderer->fontSmall)
    {
        fprintf(stderr, "Failed to load fonts: %s\n", TTF_GetError());
        return false;
    }

    /* Load backgrounds (6 total) */
    /* 0: Cyberpunk Street */
    _Renderer->backgrounds[0].numLayers = 3;
    _Renderer->backgrounds[0].layers[0] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/cyberpunk/far-buildings.png");
    _Renderer->backgrounds[0].layers[1] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/cyberpunk/back-buildings.png");
    _Renderer->backgrounds[0].layers[2] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/cyberpunk/foreground.png");
    _Renderer->backgrounds[0].scrollSpeeds[0] = 0.2f;
    _Renderer->backgrounds[0].scrollSpeeds[1] = 0.5f;
    _Renderer->backgrounds[0].scrollSpeeds[2] = 1.0f;
    _Renderer->backgrounds[0].scrollOffset = 0.0f;

    /* 1: Forest */
    _Renderer->backgrounds[1].numLayers = 4;
    _Renderer->backgrounds[1].layers[0] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/forest/parallax-forest-back-trees.png");
    _Renderer->backgrounds[1].layers[1] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/forest/parallax-forest-middle-trees.png");
    _Renderer->backgrounds[1].layers[2] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/forest/parallax-forest-front-trees.png");
    _Renderer->backgrounds[1].layers[3] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/forest/parallax-forest-lights.png");
    _Renderer->backgrounds[1].scrollSpeeds[0] = 0.2f;
    _Renderer->backgrounds[1].scrollSpeeds[1] = 0.5f;
    _Renderer->backgrounds[1].scrollSpeeds[2] = 1.0f;
    _Renderer->backgrounds[1].scrollSpeeds[3] = 0.8f;
    _Renderer->backgrounds[1].scrollOffset = 0.0f;

    /* 2: Underwater */
    _Renderer->backgrounds[2].numLayers = 5;
    _Renderer->backgrounds[2].layers[0] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/far.png");
    _Renderer->backgrounds[2].layers[1] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/sand.png");
    _Renderer->backgrounds[2].layers[2] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/foreground-1.png");
    _Renderer->backgrounds[2].layers[3] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/foreground-2.png");
    _Renderer->backgrounds[2].layers[4] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/foregound-merged.png");
    _Renderer->backgrounds[2].scrollSpeeds[0] = 0.1f;
    _Renderer->backgrounds[2].scrollSpeeds[1] = 0.0f;
    _Renderer->backgrounds[2].scrollSpeeds[2] = 0.5f;
    _Renderer->backgrounds[2].scrollSpeeds[3] = 0.8f;
    _Renderer->backgrounds[2].scrollSpeeds[4] = 1.0f;
    _Renderer->backgrounds[2].scrollOffset = 0.0f;

    /* 3: Mountain Dusk */
    _Renderer->backgrounds[3].numLayers = 6;
    _Renderer->backgrounds[3].layers[0] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/sky.png");
    _Renderer->backgrounds[3].layers[1] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/far-mountains.png");
    _Renderer->backgrounds[3].layers[2] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/mountains.png");
    _Renderer->backgrounds[3].layers[3] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/far-clouds.png");
    _Renderer->backgrounds[3].layers[4] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/near-clouds.png");
    _Renderer->backgrounds[3].layers[5] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/trees.png");
    _Renderer->backgrounds[3].scrollSpeeds[0] = 0.0f;
    _Renderer->backgrounds[3].scrollSpeeds[1] = 0.2f;
    _Renderer->backgrounds[3].scrollSpeeds[2] = 0.4f;
    _Renderer->backgrounds[3].scrollSpeeds[3] = 0.5f;
    _Renderer->backgrounds[3].scrollSpeeds[4] = 0.6f;
    _Renderer->backgrounds[3].scrollSpeeds[5] = 1.0f;
    _Renderer->backgrounds[3].scrollOffset = 0.0f;

    /* 4: Country Platform */
    _Renderer->backgrounds[4].numLayers = 2;
    _Renderer->backgrounds[4].layers[0] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/country/country-platform-back.png");
    _Renderer->backgrounds[4].layers[1] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/country/country-platform-forest.png");
    _Renderer->backgrounds[4].scrollSpeeds[0] = 0.3f;
    _Renderer->backgrounds[4].scrollSpeeds[1] = 1.0f;
    _Renderer->backgrounds[4].scrollOffset = 0.0f;

    /* 5: City Night */
    _Renderer->backgrounds[5].numLayers = 3;
    _Renderer->backgrounds[5].layers[0] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/city/back.png");
    _Renderer->backgrounds[5].layers[1] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/city/middle.png");
    _Renderer->backgrounds[5].layers[2] = Renderer_LoadTexture(_Renderer->sdlRenderer, "assets/backgrounds/city/foreground.png");
    _Renderer->backgrounds[5].scrollSpeeds[0] = 0.2f;
    _Renderer->backgrounds[5].scrollSpeeds[1] = 0.5f;
    _Renderer->backgrounds[5].scrollSpeeds[2] = 1.0f;
    _Renderer->backgrounds[5].scrollOffset = 0.0f;

    printf("✓ Loaded 6 parallax backgrounds\n");

    /* No sprites - using solid colors for walls and effects */
    for (int i = 0; i < 8; i++)
    {
        _Renderer->wallBlocks[i] = NULL;
    }
    for (int i = 0; i < 7; i++)
    {
        _Renderer->explosionFrames[i] = NULL;
    }
    printf("✓ Using solid colors for walls and effects (no sprites)\n");

    /* Load medal animations */
    for (int i = 0; i < MEDAL_FRAMES; i++)
    {
        char path[256];
        snprintf(path, sizeof(path), "assets/medals/Platinum%02d.png", i + 1);
        _Renderer->medalPlatinum[i] = Renderer_LoadTexture(_Renderer->sdlRenderer, path);

        snprintf(path, sizeof(path), "assets/medals/Gold%02d.png", i + 1);
        _Renderer->medalGold[i] = Renderer_LoadTexture(_Renderer->sdlRenderer, path);

        snprintf(path, sizeof(path), "assets/medals/Silver%02d.png", i + 1);
        _Renderer->medalSilver[i] = Renderer_LoadTexture(_Renderer->sdlRenderer, path);

        snprintf(path, sizeof(path), "assets/medals/Bronze%02d.png", i + 1);
        _Renderer->medalBronze[i] = Renderer_LoadTexture(_Renderer->sdlRenderer, path);
    }
    printf("✓ Loaded medal animations\n");

    _Renderer->currentBackground = 0;
    _Renderer->initialized = true;

    printf("✓ Renderer initialized (%dx%d)\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    return true;
}

/* Cleanup renderer */
void Renderer_Cleanup(Renderer *_Renderer)
{
    if (!_Renderer->initialized)
        return;

    /* Cleanup backgrounds */
    for (int i = 0; i < NUM_BACKGROUNDS; i++)
    {
        for (int j = 0; j < _Renderer->backgrounds[i].numLayers; j++)
        {
            if (_Renderer->backgrounds[i].layers[j])
            {
                SDL_DestroyTexture(_Renderer->backgrounds[i].layers[j]);
            }
        }
    }

    /* Cleanup walls */
    for (int i = 0; i < 8; i++)
    {
        if (_Renderer->wallBlocks[i])
        {
            SDL_DestroyTexture(_Renderer->wallBlocks[i]);
        }
    }

    /* Cleanup explosions */
    for (int i = 0; i < 7; i++)
    {
        if (_Renderer->explosionFrames[i])
        {
            SDL_DestroyTexture(_Renderer->explosionFrames[i]);
        }
    }

    /* Cleanup medals */
    for (int i = 0; i < MEDAL_FRAMES; i++)
    {
        if (_Renderer->medalPlatinum[i])
            SDL_DestroyTexture(_Renderer->medalPlatinum[i]);
        if (_Renderer->medalGold[i])
            SDL_DestroyTexture(_Renderer->medalGold[i]);
        if (_Renderer->medalSilver[i])
            SDL_DestroyTexture(_Renderer->medalSilver[i]);
        if (_Renderer->medalBronze[i])
            SDL_DestroyTexture(_Renderer->medalBronze[i]);
    }

    /* Cleanup fonts */
    if (_Renderer->fontLarge)
        TTF_CloseFont(_Renderer->fontLarge);
    if (_Renderer->fontMedium)
        TTF_CloseFont(_Renderer->fontMedium);
    if (_Renderer->fontSmall)
        TTF_CloseFont(_Renderer->fontSmall);

    TTF_Quit();
    IMG_Quit();

    if (_Renderer->sdlRenderer)
    {
        SDL_DestroyRenderer(_Renderer->sdlRenderer);
    }

    if (_Renderer->window)
    {
        SDL_DestroyWindow(_Renderer->window);
    }

    _Renderer->initialized = false;
}

/* Clear screen */
void Renderer_Clear(Renderer *_Renderer)
{
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(_Renderer->sdlRenderer);
}

/* Present frame */
void Renderer_Present(Renderer *_Renderer)
{
    SDL_RenderPresent(_Renderer->sdlRenderer);
}
