#include "../include/renderer.h"
#include "../include/audio.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>

/* Helper to load texture */
static SDL_Texture *loadTexture(SDL_Renderer *_Renderer, const char *_Path)
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

    /* Load backgrounds (5 total) */
    /* 0: Cyberpunk Street */
    _Renderer->backgrounds[0].numLayers = 3;
    _Renderer->backgrounds[0].layers[0] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/cyberpunk/far-buildings.png");
    _Renderer->backgrounds[0].layers[1] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/cyberpunk/back-buildings.png");
    _Renderer->backgrounds[0].layers[2] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/cyberpunk/foreground.png");
    _Renderer->backgrounds[0].scrollSpeeds[0] = 0.2f;
    _Renderer->backgrounds[0].scrollSpeeds[1] = 0.5f;
    _Renderer->backgrounds[0].scrollSpeeds[2] = 1.0f;
    _Renderer->backgrounds[0].scrollOffset = 0.0f;

    /* 1: Forest */
    _Renderer->backgrounds[1].numLayers = 4;
    _Renderer->backgrounds[1].layers[0] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/forest/parallax-forest-back-trees.png");
    _Renderer->backgrounds[1].layers[1] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/forest/parallax-forest-middle-trees.png");
    _Renderer->backgrounds[1].layers[2] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/forest/parallax-forest-front-trees.png");
    _Renderer->backgrounds[1].layers[3] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/forest/parallax-forest-lights.png");
    _Renderer->backgrounds[1].scrollSpeeds[0] = 0.2f;
    _Renderer->backgrounds[1].scrollSpeeds[1] = 0.5f;
    _Renderer->backgrounds[1].scrollSpeeds[2] = 1.0f;
    _Renderer->backgrounds[1].scrollSpeeds[3] = 0.8f;
    _Renderer->backgrounds[1].scrollOffset = 0.0f;

    /* 2: Underwater */
    _Renderer->backgrounds[2].numLayers = 5;
    _Renderer->backgrounds[2].layers[0] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/far.png");
    _Renderer->backgrounds[2].layers[1] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/sand.png");
    _Renderer->backgrounds[2].layers[2] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/foreground-1.png");
    _Renderer->backgrounds[2].layers[3] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/foreground-2.png");
    _Renderer->backgrounds[2].layers[4] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/underwater/foregound-merged.png");
    _Renderer->backgrounds[2].scrollSpeeds[0] = 0.1f;
    _Renderer->backgrounds[2].scrollSpeeds[1] = 0.0f;
    _Renderer->backgrounds[2].scrollSpeeds[2] = 0.5f;
    _Renderer->backgrounds[2].scrollSpeeds[3] = 0.8f;
    _Renderer->backgrounds[2].scrollSpeeds[4] = 1.0f;
    _Renderer->backgrounds[2].scrollOffset = 0.0f;

    /* 3: Mountain Dusk */
    _Renderer->backgrounds[3].numLayers = 6;
    _Renderer->backgrounds[3].layers[0] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/sky.png");
    _Renderer->backgrounds[3].layers[1] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/far-mountains.png");
    _Renderer->backgrounds[3].layers[2] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/mountains.png");
    _Renderer->backgrounds[3].layers[3] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/far-clouds.png");
    _Renderer->backgrounds[3].layers[4] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/near-clouds.png");
    _Renderer->backgrounds[3].layers[5] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/mountain/trees.png");
    _Renderer->backgrounds[3].scrollSpeeds[0] = 0.0f;
    _Renderer->backgrounds[3].scrollSpeeds[1] = 0.2f;
    _Renderer->backgrounds[3].scrollSpeeds[2] = 0.4f;
    _Renderer->backgrounds[3].scrollSpeeds[3] = 0.5f;
    _Renderer->backgrounds[3].scrollSpeeds[4] = 0.6f;
    _Renderer->backgrounds[3].scrollSpeeds[5] = 1.0f;
    _Renderer->backgrounds[3].scrollOffset = 0.0f;

    /* 4: Country Platform */
    _Renderer->backgrounds[4].numLayers = 2;
    _Renderer->backgrounds[4].layers[0] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/country/country-platform-back.png");
    _Renderer->backgrounds[4].layers[1] = loadTexture(_Renderer->sdlRenderer, "assets/backgrounds/country/country-platform-forest.png");
    _Renderer->backgrounds[4].scrollSpeeds[0] = 0.3f;
    _Renderer->backgrounds[4].scrollSpeeds[1] = 1.0f;
    _Renderer->backgrounds[4].scrollOffset = 0.0f;

    printf("✓ Loaded 5 parallax backgrounds\n");

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
        _Renderer->medalPlatinum[i] = loadTexture(_Renderer->sdlRenderer, path);

        snprintf(path, sizeof(path), "assets/medals/Gold%02d.png", i + 1);
        _Renderer->medalGold[i] = loadTexture(_Renderer->sdlRenderer, path);

        snprintf(path, sizeof(path), "assets/medals/Silver%02d.png", i + 1);
        _Renderer->medalSilver[i] = loadTexture(_Renderer->sdlRenderer, path);

        snprintf(path, sizeof(path), "assets/medals/Bronze%02d.png", i + 1);
        _Renderer->medalBronze[i] = loadTexture(_Renderer->sdlRenderer, path);
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

/* Helper to draw HUD with combo/pulsing effect */
static void drawHudWithPulse(Renderer *_Renderer, int _ComboCount, float _Multiplier, bool _IsPowerUpMode, bool _ComboEffectsEnabled)
{
    const int borderThickness = HUD_BORDER_THICKNESS;
    unsigned int time = SDL_GetTicks();

    int hudR = 20, hudG = 20, hudB = 20;
    int alpha = 230;

    // If combo effects are disabled, always use static dark border
    if (!_ComboEffectsEnabled)
    {
        hudR = 20;
        hudG = 20;
        hudB = 20;
        alpha = 230;
    }
    // Power-Up mode has more intense effects
    else
    {
        float intensityMult = _IsPowerUpMode ? 1.5f : 1.0f;

        /* Different effects based on combo tier */
        if (_ComboCount == 0)
        {
            /* Tier 1: No effect - static dark border */
            hudR = 20;
            hudG = 20;
            hudB = 20;
            alpha = 230;
        }
        else if (_Multiplier < 2.0f) /* Tier 2: 1.5x (3-4 combo) */
        {
            /* Slow pulse - white glow */
            float pulse = (sinf(time * 2.0f * intensityMult / 1000.0f) + 1.0f) * 0.5f;
            int brightness = 20 + (int)(pulse * 50 * intensityMult);
            hudR = brightness;
            hudG = brightness;
            hudB = brightness;
            alpha = 230 - (int)(pulse * 20 * intensityMult);
        }
        else if (_Multiplier < 3.0f) /* Tier 3: 2.0x (5-7 combo) */
        {
            /* Fast pulse - yellow tint */
            float pulse = (sinf(time * 4.0f * intensityMult / 1000.0f) + 1.0f) * 0.5f;
            hudR = 255;
            hudG = 255 - (int)(pulse * 80);
            hudB = 100 - (int)(pulse * 80);
            alpha = 230 - (int)(pulse * 30 * intensityMult);
        }
        else if (_Multiplier < 5.0f) /* Tier 4: 3.0x (8-11 combo) */
        {
            /* Double pulse - orange glow */
            float pulse1 = (sinf(time * 5.0f * intensityMult / 1000.0f) + 1.0f) * 0.5f;
            float pulse2 = (sinf(time * 10.0f * intensityMult / 1000.0f) + 1.0f) * 0.5f;
            float combined = (pulse1 + pulse2) / 2.0f;
            hudR = 255;
            hudG = 150 - (int)(combined * 80);
            hudB = 50 - (int)(combined * 50);
            alpha = 230 - (int)(combined * 40 * intensityMult);
        }
        else /* Tier 5: 5.0x (12+ combo) */
        {
            /* Rainbow flashing effect - faster in Power-Up mode */
            int colorSpeed = _IsPowerUpMode ? 100 : 150;
            int colorPhase = (time / colorSpeed) % 6;
            float pulse = (sinf(time * 6.0f * intensityMult / 1000.0f) + 1.0f) * 0.5f;

            switch (colorPhase)
            {
            case 0:
                hudR = 255;
                hudG = (int)(pulse * 100);
                hudB = (int)(pulse * 100);
                break; /* Red */
            case 1:
                hudR = 255;
                hudG = 200;
                hudB = 0;
                break; /* Yellow */
            case 2:
                hudR = 0;
                hudG = 255;
                hudB = (int)(pulse * 100);
                break; /* Green */
            case 3:
                hudR = 0;
                hudG = 200;
                hudB = 255;
                break; /* Cyan */
            case 4:
                hudR = (int)(pulse * 150);
                hudG = 0;
                hudB = 255;
                break; /* Blue */
            case 5:
                hudR = 255;
                hudG = 0;
                hudB = 255;
                break; /* Magenta */
            }
            alpha = _IsPowerUpMode ? (200 - (int)(pulse * 70)) : (220 - (int)(pulse * 50));
        }
    }

    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, hudR, hudG, hudB, alpha);

    /* Top border */
    SDL_Rect top = {0, 0, WINDOW_WIDTH, borderThickness};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &top);

    /* Bottom border */
    SDL_Rect bottom = {0, WINDOW_HEIGHT - borderThickness, WINDOW_WIDTH, borderThickness};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &bottom);

    /* Left border */
    SDL_Rect left = {0, 0, borderThickness, WINDOW_HEIGHT};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &left);

    /* Right border */
    SDL_Rect right = {WINDOW_WIDTH - borderThickness, 0, borderThickness, WINDOW_HEIGHT};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &right);
}

void Renderer_DrawHudBorderWithScore(Renderer *_Renderer, int _Score, int _Attempt, bool _ShowAttempt)
{
    if (!_Renderer || !_Renderer->sdlRenderer)
        return;

    /* HUD is exactly 2 cells wide (40px), covering the grid cells */
    const int borderThickness = HUD_BORDER_THICKNESS;

    /* Draw solid HUD border (no pulse for singleplayer) */
    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 20, 20, 20, 230);

    SDL_Rect top = {0, 0, WINDOW_WIDTH, borderThickness};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &top);
    SDL_Rect bottom = {0, WINDOW_HEIGHT - borderThickness, WINDOW_WIDTH, borderThickness};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &bottom);
    SDL_Rect left = {0, 0, borderThickness, WINDOW_HEIGHT};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &left);
    SDL_Rect right = {WINDOW_WIDTH - borderThickness, 0, borderThickness, WINDOW_HEIGHT};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &right);

    /* Draw text in the top HUD border - either ATTEMPT or SCORE */
    char hudText[32];
    if (_ShowAttempt)
    {
        snprintf(hudText, sizeof(hudText), "ATTEMPT %d/3", _Attempt + 1);
    }
    else
    {
        snprintf(hudText, sizeof(hudText), "SCORE: %d", _Score);
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontSmall, hudText, white);

    if (surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
        if (texture)
        {
            SDL_Rect dest = {
                WINDOW_WIDTH / 2 - surface->w / 2,
                (borderThickness - surface->h) / 2,
                surface->w,
                surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

void Renderer_DrawHudBorderWithCombo(Renderer *_Renderer, int _Score, int _Combo, float _Multiplier, GameMode _GameMode, bool _ComboEffects, int _Level, int _XP, int _XPToNextLevel)
{
    if (!_Renderer || !_Renderer->sdlRenderer)
        return;

    const int borderThickness = HUD_BORDER_THICKNESS;

    /* Draw HUD with pulse effect based on combo */
    bool isPowerUpMode = (_GameMode == MODE_POWERUP);
    drawHudWithPulse(_Renderer, _Combo, _Multiplier, isPowerUpMode, _ComboEffects);

    /* Draw SCORE text in the top HUD border */
    char scoreText[64];
    if (isPowerUpMode)
    {
        snprintf(scoreText, sizeof(scoreText), "SCORE: %d   LEVEL %d", _Score, _Level);
    }
    else
    {
        snprintf(scoreText, sizeof(scoreText), "SCORE: %d", _Score);
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontSmall, scoreText, white);

    if (surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
        if (texture)
        {
            SDL_Rect dest = {
                WINDOW_WIDTH / 2 - surface->w / 2,
                (borderThickness - surface->h) / 2,
                surface->w,
                surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }

    /* Draw combo/multiplier info or XP bar at the bottom */
    if (isPowerUpMode)
    {
        // Draw XP bar
        int barWidth = 300;
        int barHeight = 16;
        int barX = WINDOW_WIDTH / 2 - barWidth / 2;
        int barY = WINDOW_HEIGHT - borderThickness + (borderThickness - barHeight) / 2;

        // Background
        SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 40, 40, 40, 200);
        SDL_Rect bgRect = {barX, barY, barWidth, barHeight};
        SDL_RenderFillRect(_Renderer->sdlRenderer, &bgRect);

        // XP fill (cyan/blue gradient)
        float xpPercent = (float)_XP / (float)_XPToNextLevel;
        if (xpPercent > 1.0f) xpPercent = 1.0f;
        int fillWidth = (int)(barWidth * xpPercent);

        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 200, 255, 255);
        SDL_Rect fillRect = {barX, barY, fillWidth, barHeight};
        SDL_RenderFillRect(_Renderer->sdlRenderer, &fillRect);

        // Border
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 100, 220, 255, 255);
        SDL_RenderDrawRect(_Renderer->sdlRenderer, &bgRect);

        // XP text
        char xpText[32];
        snprintf(xpText, sizeof(xpText), "%d / %d XP", _XP, _XPToNextLevel);
        SDL_Color cyanColor = {100, 220, 255, 255};
        SDL_Surface *xpSurface = TTF_RenderText_Solid(_Renderer->fontSmall, xpText, cyanColor);
        if (xpSurface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, xpSurface);
            if (texture)
            {
                SDL_Rect textDest = {
                    WINDOW_WIDTH / 2 - xpSurface->w / 2,
                    barY + (barHeight - xpSurface->h) / 2,
                    xpSurface->w,
                    xpSurface->h
                };
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &textDest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(xpSurface);
        }
    }
    else if (_Combo > 0)
    {
        char comboText[64];
        snprintf(comboText, sizeof(comboText), "COMBO x%d   %.1fx MULTIPLIER", _Combo, _Multiplier);

        SDL_Color comboColor = {255, 200, 50, 255}; /* Gold color for combo */
        SDL_Surface *comboSurface = TTF_RenderText_Solid(_Renderer->fontSmall, comboText, comboColor);
        if (comboSurface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, comboSurface);
            if (texture)
            {
                SDL_Rect dest = {
                    WINDOW_WIDTH / 2 - comboSurface->w / 2,
                    WINDOW_HEIGHT - borderThickness + (borderThickness - comboSurface->h) / 2,
                    comboSurface->w,
                    comboSurface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(comboSurface);
        }
    }
}

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

    case POWERUP_SHIELD:
        // Silver/white shield
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 192, 192, 192, 255);
        SDL_RenderFillRect(_Renderer->sdlRenderer, &rect);
        // Outer border pulsing
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 255, 255, 255, (int)(pulse * 255));
        SDL_RenderDrawRect(_Renderer->sdlRenderer, &rect);
        break;

    case POWERUP_SPEED_BOOST:
        // Yellow lightning
        SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 255, 255, 0, 255);
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

/* Draw HUD (minimal for now) */
void Renderer_DrawHud(Renderer *_Renderer, Game *_Game)
{
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "SCORE: %d", _Game->snake.score);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontSmall, scoreText, white);

    if (surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
        if (texture)
        {
            SDL_Rect dest = {10, 10, surface->w, surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

/* Draw menu */
void Renderer_DrawMenu(Renderer *_Renderer, int _SelectedBg)
{
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {0, 255, 0, 255};

    /* Title */
    SDL_Surface *titleSurface = TTF_RenderText_Solid(_Renderer->fontLarge, "SNEJK2D", white);
    if (titleSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, titleSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(titleSurface);
    }

    /* Background names */
    const char *bgNames[NUM_BACKGROUNDS] = {"cyberpunk", "forest", "underwater", "mountain", "country"};
    char bgText[64];

    /* Safety check for selected_bg index */
    if (_SelectedBg < 0 || _SelectedBg >= NUM_BACKGROUNDS)
        _SelectedBg = 0;

    snprintf(bgText, sizeof(bgText), "de_%s", bgNames[_SelectedBg]);

    SDL_Surface *bgSurface = TTF_RenderText_Solid(_Renderer->fontMedium, bgText, green);
    if (bgSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, bgSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - bgSurface->w / 2, 300, bgSurface->w, bgSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(bgSurface);
    }

    /* Instructions */
    SDL_Surface *instSurface = TTF_RenderText_Solid(_Renderer->fontSmall, "< > to change   SPACE to start", white);
    if (instSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, instSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - instSurface->w / 2, 400, instSurface->w, instSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(instSurface);
    }
}

/* Draw name input screen with blinking prompt */
void Renderer_DrawModeSelect(Renderer *_Renderer, Game *_Game)
{
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 200, 100, 255};
    SDL_Color gray = {150, 150, 150, 255};

    /* Title */
    SDL_Surface *titleSurface = TTF_RenderText_Solid(_Renderer->fontLarge, "SELECT GAME MODE", white);
    if (titleSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, titleSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - titleSurface->w / 2, 80, titleSurface->w, titleSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(titleSurface);
    }

    /* Classic Mode */
    SDL_Color classicColor = (_Game->modeSelection == 0) ? green : gray;
    SDL_Surface *classicSurface = TTF_RenderText_Solid(_Renderer->fontMedium, "CLASSIC MODE", classicColor);
    if (classicSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, classicSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - classicSurface->w / 2, 200, classicSurface->w, classicSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(classicSurface);
    }

    /* Classic description */
    SDL_Surface *classicDescSurface = TTF_RenderText_Solid(_Renderer->fontSmall, "Original snake game", gray);
    if (classicDescSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, classicDescSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - classicDescSurface->w / 2, 235, classicDescSurface->w, classicDescSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(classicDescSurface);
    }

    /* Power-Up Mode */
    SDL_Color powerupColor = (_Game->modeSelection == 1) ? green : gray;
    SDL_Surface *powerupSurface = TTF_RenderText_Solid(_Renderer->fontMedium, "POWER-UP MODE", powerupColor);
    if (powerupSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, powerupSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - powerupSurface->w / 2, 300, powerupSurface->w, powerupSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(powerupSurface);
    }

    /* Power-Up description */
    SDL_Surface *powerupDescSurface = TTF_RenderText_Solid(_Renderer->fontSmall, "Multiple power-ups for extreme gameplay", gray);
    if (powerupDescSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, powerupDescSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - powerupDescSurface->w / 2, 335, powerupDescSurface->w, powerupDescSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(powerupDescSurface);
    }

    /* Instructions */
    SDL_Surface *instrSurface = TTF_RenderText_Solid(_Renderer->fontSmall, "ENTER CONFIRM   ESC BACK", white);
    if (instrSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, instrSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - instrSurface->w / 2, 420, instrSurface->w, instrSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(instrSurface);
    }
}

void Renderer_DrawNameInput(Renderer *_Renderer, Game *_Game, unsigned int _Tick)
{
    /* Blinking colors - cycle through RGB */
    int blinkPhase = (_Tick / 300) % 6; /* Change color every 300ms */
    SDL_Color promptColors[] = {
        {255, 100, 100, 255}, /* Red */
        {255, 200, 100, 255}, /* Orange */
        {255, 255, 100, 255}, /* Yellow */
        {100, 255, 100, 255}, /* Green */
        {100, 200, 255, 255}, /* Cyan */
        {200, 100, 255, 255}  /* Purple */
    };
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 200, 100, 255};

    /* Title */
    SDL_Surface *titleSurface = TTF_RenderText_Solid(_Renderer->fontLarge, "SNEJK2D", white);
    if (titleSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, titleSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(titleSurface);
    }

    /* Blinking "TYPE YOUR NAME" prompt */
    SDL_Surface *promptSurface = TTF_RenderText_Solid(_Renderer->fontSmall, "TYPE YOUR NAME", promptColors[blinkPhase]);
    if (promptSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, promptSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - promptSurface->w / 2, 200, promptSurface->w, promptSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(promptSurface);
    }

    /* Player name input */
    char displayText[64];
    snprintf(displayText, sizeof(displayText), "%s_", _Game->playerName);
    SDL_Surface *nameSurface = TTF_RenderText_Solid(_Renderer->fontMedium, displayText, green);
    if (nameSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, nameSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - nameSurface->w / 2, 260, nameSurface->w, nameSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(nameSurface);
    }

    /* Instructions */
    SDL_Surface *instSurface = TTF_RenderText_Solid(_Renderer->fontSmall, "ENTER CONTINUE   ESC BACK", white);
    if (instSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, instSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - instSurface->w / 2, WINDOW_HEIGHT - 60, instSurface->w, instSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(instSurface);
    }
}

/* Draw explosion at position */
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

// Draw retro loading screen with scanlines and progress bar
void Renderer_DrawLoadingScreen(Renderer *_Renderer, float _Progress, unsigned int _CurrentTime)
{
    // Black background
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(_Renderer->sdlRenderer);

    SDL_Color green = {0, 255, 0, 255};
    SDL_Color white = {255, 255, 255, 255};

    // Retro title "SNEJK2D" with glitch effect
    int glitch = (_CurrentTime % 1000 < 50) ? (rand() % 4 - 2) : 0;  // Random glitch every second
    SDL_Surface *titleSurface = TTF_RenderText_Solid(_Renderer->fontLarge, "SNEJK2D", green);
    if (titleSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, titleSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH/2 - titleSurface->w/2 + glitch, 120, titleSurface->w, titleSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(titleSurface);
    }

    // Rotating loading messages based on progress
    const char *loadingMessages[] = {
        "LOADING SNAKES",
        "LOADING SFX",
        "LOADING TEXTURES",
        "INITIALIZING GRID",
        "SPAWNING POWER-UPS",
        "CALIBRATING WALLS"
    };
    int numMessages = 6;
    int messageIndex = (int)(_Progress * numMessages);
    if (messageIndex >= numMessages) messageIndex = numMessages - 1;

    // Animated dots
    int dotCount = ((_CurrentTime / 300) % 4);  // 0-3 dots animating
    char loadingText[64];
    snprintf(loadingText, sizeof(loadingText), "%s%.*s", loadingMessages[messageIndex], dotCount, "...");

    SDL_Surface *loadingSurface = TTF_RenderText_Solid(_Renderer->fontMedium, loadingText, white);
    if (loadingSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, loadingSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH/2 - loadingSurface->w/2, 220, loadingSurface->w, loadingSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(loadingSurface);
    }

    // Progress bar - retro style
    int barWidth = 400;
    int barHeight = 30;
    int barX = (WINDOW_WIDTH - barWidth) / 2;
    int barY = 280;

    // Progress bar border (green)
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 255, 0, 255);
    SDL_Rect borderRect = {barX - 2, barY - 2, barWidth + 4, barHeight + 4};
    SDL_RenderDrawRect(_Renderer->sdlRenderer, &borderRect);

    // Progress bar background (dark)
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 20, 20, 20, 255);
    SDL_Rect bgRect = {barX, barY, barWidth, barHeight};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &bgRect);

    // Progress bar fill (green gradient effect with segments)
    int fillWidth = (int)(barWidth * _Progress);
    if (fillWidth > 0)
    {
        // Draw segmented progress bar for retro look
        for (int i = 0; i < fillWidth; i += 8)
        {
            int segWidth = (i + 8 < fillWidth) ? 8 : (fillWidth - i);
            // Alternating green shades for segments
            if ((i / 8) % 2 == 0)
                SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 255, 0, 255);
            else
                SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 200, 0, 255);

            SDL_Rect segRect = {barX + i, barY, segWidth - 1, barHeight};
            SDL_RenderFillRect(_Renderer->sdlRenderer, &segRect);
        }
    }

    // Progress percentage
    char percentText[16];
    snprintf(percentText, sizeof(percentText), "%d%%", (int)(_Progress * 100));
    SDL_Surface *percentSurface = TTF_RenderText_Solid(_Renderer->fontSmall, percentText, green);
    if (percentSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, percentSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH/2 - percentSurface->w/2, barY + barHeight + 15, percentSurface->w, percentSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(percentSurface);
    }

    // Retro scanlines effect (every other line is slightly darker)
    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, 30);
    for (int y = 0; y < WINDOW_HEIGHT; y += 2)
    {
        SDL_RenderDrawLine(_Renderer->sdlRenderer, 0, y, WINDOW_WIDTH, y);
    }
}

// Draw main menu (singleplayer / multiplayer choice)
void Renderer_DrawMainMenu(Renderer *_Renderer, int _Selection)
{
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {0, 255, 0, 255};
    SDL_Color gray = {150, 150, 150, 255};

    // Title
    SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontLarge, "SNEJK2D", white);
    if (title_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - title_surface->w / 2, 80, title_surface->w, title_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(title_surface);
    }

    // Menu spacing: 50px between items
    int startY = 180;
    int spacing = 50;

    // Singleplayer option
    const char *sp_text = _Selection == 0 ? "> SINGLEPLAYER " : "  SINGLEPLAYER  ";
    SDL_Color sp_color = _Selection == 0 ? green : gray;
    SDL_Surface *sp_surface = TTF_RenderText_Solid(_Renderer->fontMedium, sp_text, sp_color);
    if (sp_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, sp_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - sp_surface->w / 2, startY, sp_surface->w, sp_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(sp_surface);
    }

    // Multiplayer option
    const char *mp_text = _Selection == 1 ? "> MULTIPLAYER" : "  MULTIPLAYER  ";
    SDL_Color mp_color = _Selection == 1 ? green : gray;
    SDL_Surface *mp_surface = TTF_RenderText_Solid(_Renderer->fontMedium, mp_text, mp_color);
    if (mp_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, mp_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - mp_surface->w / 2, startY + spacing, mp_surface->w, mp_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(mp_surface);
    }

    // Scoreboard option
    const char *sb_text = _Selection == 2 ? "> SCOREBOARD " : "  SCOREBOARD  ";
    SDL_Color sb_color = _Selection == 2 ? green : gray;
    SDL_Surface *sb_surface = TTF_RenderText_Solid(_Renderer->fontMedium, sb_text, sb_color);
    if (sb_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, sb_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - sb_surface->w / 2, startY + spacing * 2, sb_surface->w, sb_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(sb_surface);
    }

    // Options
    const char *opt_text = _Selection == 3 ? "> OPTIONS " : "  OPTIONS  ";
    SDL_Color opt_color = _Selection == 3 ? green : gray;
    SDL_Surface *opt_surface = TTF_RenderText_Solid(_Renderer->fontMedium, opt_text, opt_color);
    if (opt_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, opt_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - opt_surface->w / 2, startY + spacing * 3, opt_surface->w, opt_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(opt_surface);
    }

    // Exit
    const char *exit_text = _Selection == 4 ? "> EXIT " : "  EXIT  ";
    SDL_Color exit_color = _Selection == 4 ? green : gray;
    SDL_Surface *exit_surface = TTF_RenderText_Solid(_Renderer->fontMedium, exit_text, exit_color);
    if (exit_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, exit_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - exit_surface->w / 2, startY + spacing * 4, exit_surface->w, exit_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(exit_surface);
    }
}

void Renderer_DrawPauseMenu(Renderer *_Renderer, int _Selection)
{
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {0, 255, 0, 255};
    SDL_Color gray = {150, 150, 150, 255};

    // Draw semi-transparent overlay
    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, 180); // Dark overlay with transparency
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &overlay);

    // Title "PAUSED"
    SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontLarge, "PAUSED", white);
    if (title_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - title_surface->w / 2, 150, title_surface->w, title_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(title_surface);
    }

    // Menu spacing
    int startY = 250;
    int spacing = 50;

    // Resume Game option
    const char *resume_text = _Selection == 0 ? "> RESUME GAME " : "  RESUME GAME  ";
    SDL_Color resume_color = _Selection == 0 ? green : gray;
    SDL_Surface *resume_surface = TTF_RenderText_Solid(_Renderer->fontMedium, resume_text, resume_color);
    if (resume_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, resume_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - resume_surface->w / 2, startY, resume_surface->w, resume_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(resume_surface);
    }

    // Exit to Main Menu option
    const char *exit_text = _Selection == 1 ? "> EXIT TO MAIN MENU " : "  EXIT TO MAIN MENU  ";
    SDL_Color exit_color = _Selection == 1 ? green : gray;
    SDL_Surface *exit_surface = TTF_RenderText_Solid(_Renderer->fontMedium, exit_text, exit_color);
    if (exit_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, exit_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - exit_surface->w / 2, startY + spacing, exit_surface->w, exit_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(exit_surface);
    }
}

void Renderer_DrawOptions(Renderer *_Renderer, Game *_Game)
{
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {0, 255, 0, 255};
    SDL_Color gray = {150, 150, 150, 255};

    // Title
    SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontLarge, "OPTIONS", white);
    if (title_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - title_surface->w / 2, 50, title_surface->w, title_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(title_surface);
    }

    // Menu Music
    SDL_Color menuMusic_color = _Game->optionsSelection == 0 ? green : gray;
    char menuMusic_text[64];
    snprintf(menuMusic_text, sizeof(menuMusic_text), "%s MENU MUSIC: [%s]",
             _Game->optionsSelection == 0 ? ">" : " ", _Game->settings.menuMusicEnabled ? "ON" : "OFF");
    SDL_Surface *menuMusic_surface = TTF_RenderText_Solid(_Renderer->fontMedium, menuMusic_text, menuMusic_color);
    if (menuMusic_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, menuMusic_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - menuMusic_surface->w / 2, 120, menuMusic_surface->w, menuMusic_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(menuMusic_surface);
    }

    // Game Music
    SDL_Color gameMusic_color = _Game->optionsSelection == 1 ? green : gray;
    char gameMusic_text[64];
    snprintf(gameMusic_text, sizeof(gameMusic_text), "%s GAME MUSIC: [%s]",
             _Game->optionsSelection == 1 ? ">" : " ", _Game->settings.gameMusicEnabled ? "ON" : "OFF");
    SDL_Surface *gameMusic_surface = TTF_RenderText_Solid(_Renderer->fontMedium, gameMusic_text, gameMusic_color);
    if (gameMusic_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, gameMusic_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - gameMusic_surface->w / 2, 170, gameMusic_surface->w, gameMusic_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(gameMusic_surface);
    }

    // Grid Alpha
    SDL_Color grid_color = _Game->optionsSelection == 2 ? green : gray;
    char grid_text[64];
    const char *grid_label = _Game->settings.gridAlpha == 0 ? "OFF" : "";
    if (_Game->settings.gridAlpha == 0)
        snprintf(grid_text, sizeof(grid_text), "%s GRID: [%s]", _Game->optionsSelection == 2 ? ">" : " ", grid_label);
    else
        snprintf(grid_text, sizeof(grid_text), "%s GRID: [%d]", _Game->optionsSelection == 2 ? ">" : " ", _Game->settings.gridAlpha);
    SDL_Surface *grid_surface = TTF_RenderText_Solid(_Renderer->fontMedium, grid_text, grid_color);
    if (grid_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, grid_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - grid_surface->w / 2, 220, grid_surface->w, grid_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(grid_surface);
    }

    // Brightness
    SDL_Color bright_color = _Game->optionsSelection == 3 ? green : gray;
    char bright_text[64];
    snprintf(bright_text, sizeof(bright_text), "%s BRIGHTNESS: [%.0f%%]",
             _Game->optionsSelection == 3 ? ">" : " ", _Game->settings.brightness * 100);
    SDL_Surface *bright_surface = TTF_RenderText_Solid(_Renderer->fontMedium, bright_text, bright_color);
    if (bright_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, bright_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - bright_surface->w / 2, 270, bright_surface->w, bright_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(bright_surface);
    }

    // Combo Effects
    SDL_Color combo_color = _Game->optionsSelection == 4 ? green : gray;
    char combo_text[64];
    snprintf(combo_text, sizeof(combo_text), "%s COMBO HUD EFFECTS: [%s]",
             _Game->optionsSelection == 4 ? ">" : " ", _Game->settings.comboEffects ? "ON" : "OFF");
    SDL_Surface *combo_surface = TTF_RenderText_Solid(_Renderer->fontMedium, combo_text, combo_color);
    if (combo_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, combo_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - combo_surface->w / 2, 320, combo_surface->w, combo_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(combo_surface);
    }

    // Reset to defaults
    const char *reset_text = _Game->optionsSelection == 5 ? "> RESET TO DEFAULTS " : "  RESET TO DEFAULTS  ";
    SDL_Color reset_color = _Game->optionsSelection == 5 ? green : gray;
    SDL_Surface *reset_surface = TTF_RenderText_Solid(_Renderer->fontMedium, reset_text, reset_color);
    if (reset_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, reset_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - reset_surface->w / 2, 370, reset_surface->w, reset_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(reset_surface);
    }

    // Instructions
    SDL_Surface *inst_surface = TTF_RenderText_Solid(_Renderer->fontSmall, "SETTINGS AUTO SAVE   ESC BACK", white);
    if (inst_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, inst_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - inst_surface->w / 2, WINDOW_HEIGHT - 40, inst_surface->w, inst_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(inst_surface);
    }
}

// Multiplayer rendering functions

void Renderer_DrawMultiplayerMenu(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_Renderer || !_MpCtx)
        return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {0, 255, 0, 255}; // Same green as main menu
    SDL_Color gray = {150, 150, 150, 255};

    // Title - SNEJK2D at same height as main menu
    SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontLarge, "SNEJK2D", white);
    if (title_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - title_surface->w / 2, 100, title_surface->w, title_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(title_surface);
    }

    // Menu options - centered like main menu
    const char *options[] = {"HOST GAME", "JOIN GAME"};
    int y_start = 220; // Moved up for better centering

    for (int i = 0; i < 2; i++)
    {
        SDL_Color color = (i == _MpCtx->menuSelection) ? green : gray;
        const char *prefix = (i == _MpCtx->menuSelection) ? "> " : "  ";
        const char *suffix = (i == _MpCtx->menuSelection) ? " " : "  ";

        char line[128];
        snprintf(line, sizeof(line), "%s%s%s", prefix, options[i], suffix);

        SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontMedium, line, color);
        if (surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
            if (texture)
            {
                SDL_Rect dest = {WINDOW_WIDTH / 2 - surface->w / 2, y_start + (i * 60), surface->w, surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }

    // Instructions
    SDL_Surface *inst_surface = TTF_RenderText_Solid(_Renderer->fontSmall,
                                                     "UP DOWN SELECT   ENTER CONFIRM   ESC BACK", white);
    if (inst_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, inst_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - inst_surface->w / 2, WINDOW_HEIGHT - 60,
                             inst_surface->w, inst_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(inst_surface);
    }
}

void Renderer_DrawMultiplayerRoomName(Renderer *_Renderer, MultiplayerContext *_MpCtx, unsigned int _CurrentTime)
{
    if (!_Renderer || !_MpCtx)
        return;

    /* Blinking colors - cycle through RGB */
    int blinkPhase = (_CurrentTime / 300) % 6; /* Change color every 300ms */
    SDL_Color promptColors[] = {
        {255, 100, 100, 255}, /* Red */
        {255, 200, 100, 255}, /* Orange */
        {255, 255, 100, 255}, /* Yellow */
        {100, 255, 100, 255}, /* Green */
        {100, 200, 255, 255}, /* Cyan */
        {200, 100, 255, 255}  /* Purple */
    };

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 200, 100, 255};
    int y = 70; // Moved up for better centering

    // Title
    SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontLarge, "SNEJK2D", white);
    if (title_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - title_surface->w / 2, y, title_surface->w, title_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(title_surface);
    }

    y += 70;

    // Blinking "TYPE GAME NAME" prompt
    SDL_Surface *promptSurface = TTF_RenderText_Solid(_Renderer->fontSmall, "TYPE GAME NAME", promptColors[blinkPhase]);
    if (promptSurface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, promptSurface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - promptSurface->w / 2, y, promptSurface->w, promptSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(promptSurface);
    }

    y += 60;

    // Input box with current text
    char display_text[64];
    snprintf(display_text, sizeof(display_text), "%s_", _MpCtx->roomName);

    SDL_Surface *input_surface = TTF_RenderText_Solid(_Renderer->fontMedium, display_text, green);
    if (input_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, input_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - input_surface->w / 2, y, input_surface->w, input_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(input_surface);
    }

    y += 50;

    // Game mode selection label (moved up before map)
    SDL_Surface *mode_label = TTF_RenderText_Solid(_Renderer->fontSmall, "MODE", white);
    if (mode_label)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, mode_label);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - mode_label->w / 2, y, mode_label->w, mode_label->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(mode_label);
    }

    y += 30;

    // Game mode display
    const char *mode_names[] = {"TURN BATTLE", "1VS1"};
    char mode_display[64];
    snprintf(mode_display, sizeof(mode_display), "< %s >", mode_names[_MpCtx->modeSelection]);

    SDL_Surface *mode_surface = TTF_RenderText_Solid(_Renderer->fontMedium, mode_display, green);
    if (mode_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, mode_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - mode_surface->w / 2, y, mode_surface->w, mode_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(mode_surface);
    }

    y += 50;

    // Map selection label
    SDL_Surface *map_label = TTF_RenderText_Solid(_Renderer->fontSmall, "MAP", white);
    if (map_label)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, map_label);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - map_label->w / 2, y, map_label->w, map_label->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(map_label);
    }

    y += 30;

    // Map name with de_ prefix
    const char *map_names[] = {"de_cyberpunk", "de_forest", "de_underwater", "de_mountain", "de_country"};
    char map_display[64];
    snprintf(map_display, sizeof(map_display), "< %s >", map_names[_MpCtx->selectedBackground]);

    SDL_Surface *map_surface = TTF_RenderText_Solid(_Renderer->fontMedium, map_display, green);
    if (map_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, map_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - map_surface->w / 2, y, map_surface->w, map_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(map_surface);
    }

    // Instructions (shortened to fit screen)
    SDL_Surface *inst_surface = TTF_RenderText_Solid(_Renderer->fontSmall,
                                                     "MODE UP DOWN  MAP LEFT RIGHT  ENTER CREATE", white);
    if (inst_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, inst_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - inst_surface->w / 2, WINDOW_HEIGHT - 60,
                             inst_surface->w, inst_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(inst_surface);
    }
}

void Renderer_DrawMultiplayerBrowse(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_Renderer || !_MpCtx)
        return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color selected = {100, 200, 100, 255};
    SDL_Color gray = {150, 150, 150, 255};
    int y = 100;

    // Title
    SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontLarge, "JOIN GAME", white);
    if (title_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - title_surface->w / 2, y, title_surface->w, title_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(title_surface);
    }

    y += 60;

    // Game list
    if (_MpCtx->browsedGameCount == 0)
    {
        SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontMedium, "NO GAMES", gray);
        if (surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
            if (texture)
            {
                SDL_Rect dest = {WINDOW_WIDTH / 2 - surface->w / 2, y + 40, surface->w, surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
    else
    {
        for (int i = 0; i < _MpCtx->browsedGameCount && i < 7; i++)
        {
            SDL_Color color = (i == _MpCtx->selectedGameIndex) ? selected : white;
            const char *prefix = (i == _MpCtx->selectedGameIndex) ? "> " : "  ";

            // Map names with de_ prefix
            const char *map_names[] = {"de_cyberpunk", "de_forest", "de_underwater", "de_mountain", "de_country"};
            int map_id = _MpCtx->browsedGames[i].mapId;
            if (map_id < 0 || map_id >= 5)
                map_id = 0; // Default to first map if invalid
            const char *map_name = map_names[map_id];

            // Shorten name if needed
            char short_name[32];
            const char *name_to_show = _MpCtx->browsedGames[i].name;
            if (strlen(name_to_show) > 20)
            {
                strncpy(short_name, name_to_show, 17);
                strcpy(short_name + 17, "...");
                name_to_show = short_name;
            }

            char line[128];
            snprintf(line, sizeof(line), "%s%s [%s] (%d/%d)",
                     prefix,
                     name_to_show,
                     map_name,
                     _MpCtx->browsedGames[i].playerCount,
                     _MpCtx->browsedGames[i].maxPlayers);

            SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontSmall, line, color);
            if (surface)
            {
                SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
                if (texture)
                {
                    SDL_Rect dest = {60, y, surface->w, surface->h};
                    SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                    SDL_DestroyTexture(texture);
                }
                SDL_FreeSurface(surface);
            }
            y += 35;
        }
    }

    // Instructions
    const char *instructions = _MpCtx->browsedGameCount > 0 ? "UP DOWN SELECT   ENTER JOIN   R REFRESH   ESC BACK" : "R REFRESH   ESC BACK";

    SDL_Surface *inst_surface = TTF_RenderText_Solid(_Renderer->fontSmall, instructions, white);
    if (inst_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, inst_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - inst_surface->w / 2, WINDOW_HEIGHT - 60,
                             inst_surface->w, inst_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(inst_surface);
    }
}

void Renderer_DrawMultiplayerLobby(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_MpCtx)
        return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 200, 100, 255};
    SDL_Color red = {200, 100, 100, 255};
    SDL_Color gray = {150, 150, 150, 255};

    // Title
    const char *title_text = _MpCtx->isHost ? "LOBBY" : "LOBBY";
    SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontLarge, title_text, white);
    if (title_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - title_surface->w / 2, 90, title_surface->w, title_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(title_surface);
    }

    // Session ID and Game Mode
    if (_MpCtx->isHost)
    {
        char session_text[32];
        snprintf(session_text, sizeof(session_text), "CODE: %s", _MpCtx->sessionId);
        SDL_Surface *session_surface = TTF_RenderText_Solid(_Renderer->fontMedium, session_text, green);
        if (session_surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, session_surface);
            if (texture)
            {
                SDL_Rect dest = {WINDOW_WIDTH / 2 - session_surface->w / 2, 145, session_surface->w, session_surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(session_surface);
        }
    }

    // Game Mode display (for both host and client)
    char mode_text[64];
    if (_MpCtx->gameMode == MP_MODE_TURN_BATTLE)
    {
        const char *tb_mode_names[] = {"CLASSIC", "POWER-UP"};
        if (_MpCtx->isHost)
        {
            // Host can change - show < > arrows
            snprintf(mode_text, sizeof(mode_text), "MODE: TURN BATTLE - < %s >", tb_mode_names[_MpCtx->turnBattleModeSelection]);
        }
        else
        {
            // Client just sees current mode
            snprintf(mode_text, sizeof(mode_text), "MODE: TURN BATTLE - %s", tb_mode_names[_MpCtx->turnBattleModeSelection]);
        }
    }
    else
    {
        // 1VS1 mode
        snprintf(mode_text, sizeof(mode_text), "MODE: 1VS1");
    }

    SDL_Color cyan = {100, 220, 255, 255};
    SDL_Surface *mode_surface = TTF_RenderText_Solid(_Renderer->fontSmall, mode_text, _MpCtx->gameMode == MP_MODE_TURN_BATTLE ? cyan : white);
    if (mode_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, mode_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - mode_surface->w / 2, _MpCtx->isHost ? 180 : 145, mode_surface->w, mode_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(mode_surface);
    }

    // Player list - only show players who have joined
    int y_offset = _MpCtx->isHost ? 245 : 210;
    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        // Skip players who haven't joined
        if (!_MpCtx->players[i].joined)
            continue;

        // Show player
        char player_text[96];
        const char *ready_status = _MpCtx->players[i].ready ? "[READY]" : "[NOT READY]";
        const char *local_marker = _MpCtx->players[i].isLocal ? " (YOU)" : "";
        const char *name = _MpCtx->players[i].name[0] ? _MpCtx->players[i].name : "PLAYER";

        snprintf(player_text, sizeof(player_text), "P%d: %s %s%s",
                 i + 1,
                 name,
                 ready_status,
                 local_marker);

        SDL_Color color = _MpCtx->players[i].ready ? green : red;
        SDL_Surface *player_surface = TTF_RenderText_Solid(_Renderer->fontMedium, player_text, color);
        if (player_surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, player_surface);
            if (texture)
            {
                SDL_Rect dest = {WINDOW_WIDTH / 2 - player_surface->w / 2, y_offset, player_surface->w, player_surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(player_surface);
        }

        y_offset += 50;
    }

    // Chat messages - show last 3 messages
    if (_MpCtx->chatCount > 0)
    {
        int chat_y = 330;
        int start_idx = (_MpCtx->chatCount > 3) ? (_MpCtx->chatCount - 3) : 0;
        int num_to_show = (_MpCtx->chatCount > 3) ? 3 : _MpCtx->chatCount;

        for (int i = 0; i < num_to_show; i++)
        {
            SDL_Color chat_color = {150, 150, 150, 255};
            SDL_Surface *chat_surface = TTF_RenderText_Solid(_Renderer->fontSmall,
                                                             _MpCtx->chatMessages[start_idx + i],
                                                             chat_color);
            if (chat_surface)
            {
                SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, chat_surface);
                if (texture)
                {
                    SDL_Rect dest = {20, chat_y, chat_surface->w, chat_surface->h};
                    SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                    SDL_DestroyTexture(texture);
                }
                SDL_FreeSurface(chat_surface);
            }
            chat_y += 20;
        }
    }

    // Instructions
    const char *inst_text;
    if (_MpCtx->isHost && _MpCtx->gameMode == MP_MODE_TURN_BATTLE)
    {
        inst_text = "ARROWS MODE   SPACE READY   ENTER START   C CHAT   N NICK";
    }
    else if (_MpCtx->isHost)
    {
        inst_text = "SPACE READY   ENTER START   C CHAT   N NICK";
    }
    else
    {
        inst_text = "SPACE READY   C CHAT   N NICK";
    }

    SDL_Surface *inst_surface = TTF_RenderText_Solid(_Renderer->fontSmall, inst_text, white);
    if (inst_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, inst_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - inst_surface->w / 2, WINDOW_HEIGHT - 60,
                             inst_surface->w, inst_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(inst_surface);
    }
}

// Draw chat overlay
void Renderer_DrawChatOverlay(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    // Dark semi-transparent overlay
    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &overlay);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 200, 100, 255};

    // Title
    SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontMedium, "CHAT", white);
    if (title_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - title_surface->w / 2, 150, title_surface->w, title_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(title_surface);
    }

    // Input
    char display_text[130];
    snprintf(display_text, sizeof(display_text), "%s_", _MpCtx->chatInput);
    SDL_Surface *input_surface = TTF_RenderText_Solid(_Renderer->fontSmall, display_text, green);
    if (input_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, input_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - input_surface->w / 2, 210, input_surface->w, input_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(input_surface);
    }

    // Instructions
    SDL_Surface *inst_surface = TTF_RenderText_Solid(_Renderer->fontSmall, "ENTER SEND   ESC CANCEL", white);
    if (inst_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, inst_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - inst_surface->w / 2, WINDOW_HEIGHT - 60, inst_surface->w, inst_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(inst_surface);
    }
}

// Draw nick change overlay
void Renderer_DrawNickOverlay(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    // Dark semi-transparent overlay
    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(_Renderer->sdlRenderer, &overlay);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 200, 100, 255};

    // Title
    SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontMedium, "CHANGE NICKNAME", white);
    if (title_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - title_surface->w / 2, 150, title_surface->w, title_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(title_surface);
    }

    // Input
    char display_text[34];
    snprintf(display_text, sizeof(display_text), "%s_", _MpCtx->nickInput);
    SDL_Surface *input_surface = TTF_RenderText_Solid(_Renderer->fontMedium, display_text, green);
    if (input_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, input_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - input_surface->w / 2, 210, input_surface->w, input_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(input_surface);
    }

    // Instructions
    SDL_Surface *inst_surface = TTF_RenderText_Solid(_Renderer->fontSmall, "ENTER CONFIRM   ESC CANCEL", white);
    if (inst_surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, inst_surface);
        if (texture)
        {
            SDL_Rect dest = {WINDOW_WIDTH / 2 - inst_surface->w / 2, WINDOW_HEIGHT - 60, inst_surface->w, inst_surface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(inst_surface);
    }
}

void Renderer_DrawMultiplayerGame(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_MpCtx)
        return;

    // Draw walls
    Renderer_DrawWalls(_Renderer);

    // Draw grid (use default alpha for multiplayer)
    Renderer_DrawGrid(_Renderer, COLOR_GRID_A);

    // Draw snakes with different colors
    SDL_Color player_colors[MAX_MULTIPLAYER_PLAYERS] = {
        {255, 60, 60, 255}, // Red
        {60, 120, 255, 255} // Blue
    };

    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        if (_MpCtx->players[i].joined && _MpCtx->players[i].alive)
        {
            Renderer_DrawSnakeColored(_Renderer, &_MpCtx->players[i].snake,
                                      player_colors[i].r,
                                      player_colors[i].g,
                                      player_colors[i].b);
        }
    }

    // Draw both food items
    Renderer_DrawFood(_Renderer, &_MpCtx->food[0]);
    Renderer_DrawFood(_Renderer, &_MpCtx->food[1]);
}

void Renderer_DrawMultiplayerHudBorder(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_Renderer || !_Renderer->sdlRenderer || !_MpCtx)
        return;

    // HUD is exactly 2 cells wide (40px), covering the grid cells
    const int border_thickness = HUD_BORDER_THICKNESS;

    // Get local player's combo for pulse effect
    int local_combo = 0;
    float local_multiplier = 1.0f;
    if (_MpCtx->localPlayerIndex >= 0 && _MpCtx->localPlayerIndex < MAX_MULTIPLAYER_PLAYERS)
    {
        local_combo = _MpCtx->players[_MpCtx->localPlayerIndex].comboCount;
        local_multiplier = _MpCtx->players[_MpCtx->localPlayerIndex].comboMultiplier;
    }

    // Draw HUD with pulse effect based on combo (multiplayer always uses classic mode visuals)
    // Note: Multiplayer doesn't have access to settings, so default to true
    drawHudWithPulse(_Renderer, local_combo, local_multiplier, false, true);

    // Player colors matching game rendering
    SDL_Color player_colors[MAX_MULTIPLAYER_PLAYERS] = {
        {255, 60, 60, 255}, // Red for P1
        {60, 120, 255, 255} // Blue for P2
    };

    // Draw P1 on left side of HUD
    if (_MpCtx->players[0].joined)
    {
        char p1_text[64];
        const char *status = _MpCtx->players[0].alive ? "" : " [DEAD]";
        snprintf(p1_text, sizeof(p1_text), "P1: %d%s", _MpCtx->players[0].score, status);

        SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontSmall, p1_text, player_colors[0]);
        if (surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
            if (texture)
            {
                // Position in top-left of HUD border
                SDL_Rect dest = {
                    30,                                  // Left side with padding
                    (border_thickness - surface->h) / 2, // Vertically centered in top border
                    surface->w,
                    surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }

    // Draw P2 on right side of HUD
    if (_MpCtx->players[1].joined)
    {
        char p2_text[64];
        const char *status = _MpCtx->players[1].alive ? "" : " [DEAD]";
        snprintf(p2_text, sizeof(p2_text), "P2: %d%s", _MpCtx->players[1].score, status);

        SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontSmall, p2_text, player_colors[1]);
        if (surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
            if (texture)
            {
                // Position in top-right of HUD border
                SDL_Rect dest = {
                    WINDOW_WIDTH - surface->w - 30,      // Right side with padding
                    (border_thickness - surface->h) / 2, // Vertically centered in top border
                    surface->w,
                    surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }

    // Draw combo/multiplier info at the bottom for local player
    if (local_combo > 0)
    {
        char combo_text[64];
        snprintf(combo_text, sizeof(combo_text), "COMBO x%d   %.1fx MULTIPLIER", local_combo, local_multiplier);

        SDL_Color combo_color = {255, 200, 50, 255}; // Gold color for combo
        SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontSmall, combo_text, combo_color);
        if (surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
            if (texture)
            {
                // Center at bottom of screen
                SDL_Rect dest = {
                    WINDOW_WIDTH / 2 - surface->w / 2,
                    WINDOW_HEIGHT - border_thickness + (border_thickness - surface->h) / 2,
                    surface->w,
                    surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

/* Draw mode selection screen (host only) */
void Renderer_DrawModeSelection(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_Renderer || !_Renderer->sdlRenderer || !_MpCtx)
        return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color gray = {150, 150, 150, 255};

    // Title
    SDL_Surface *titleSurface = TTF_RenderText_Solid(_Renderer->fontLarge, "SELECT GAME MODE", white);
    if (titleSurface)
    {
        SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, titleSurface);
        if (titleTexture)
        {
            SDL_Rect titleRect = {
                WINDOW_WIDTH / 2 - titleSurface->w / 2,
                150,
                titleSurface->w,
                titleSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture);
        }
        SDL_FreeSurface(titleSurface);
    }

    // Mode options
    const char *modes[] = {"TURN BATTLE", "1VS1"};
    int yPos = 300;

    for (int i = 0; i < 2; i++)
    {
        SDL_Color color = (_MpCtx->modeSelection == i) ? green : gray;
        const char *prefix = (_MpCtx->modeSelection == i) ? "> " : "  ";

        char modeText[64];
        snprintf(modeText, sizeof(modeText), "%s%s", prefix, modes[i]);

        SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontMedium, modeText, color);
        if (surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
            if (texture)
            {
                SDL_Rect dest = {
                    WINDOW_WIDTH / 2 - surface->w / 2,
                    yPos,
                    surface->w,
                    surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
        yPos += 80;
    }

    // Instructions
    SDL_Surface *instrSurface = TTF_RenderText_Solid(_Renderer->fontSmall, "Arrow Keys: Select  |  Enter: Confirm", gray);
    if (instrSurface)
    {
        SDL_Texture *instrTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, instrSurface);
        if (instrTexture)
        {
            SDL_Rect instrRect = {
                WINDOW_WIDTH / 2 - instrSurface->w / 2,
                WINDOW_HEIGHT - 100,
                instrSurface->w,
                instrSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, instrTexture, NULL, &instrRect);
            SDL_DestroyTexture(instrTexture);
        }
        SDL_FreeSurface(instrSurface);
    }
}

/* Draw ready-up screen for turn battle */
void Renderer_DrawReadyUp(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_Renderer || !_Renderer->sdlRenderer || !_MpCtx)
        return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color red = {255, 100, 100, 255};
    SDL_Color gray = {150, 150, 150, 255};

    // Title with mode
    const char *modeName = (_MpCtx->turnBattleMode == MODE_POWERUP) ? "POWER-UP" : "CLASSIC";
    char titleText[64];
    snprintf(titleText, sizeof(titleText), "TURN BATTLE - %s", modeName);
    SDL_Surface *titleSurface = TTF_RenderText_Solid(_Renderer->fontLarge, titleText, white);
    if (titleSurface)
    {
        SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, titleSurface);
        if (titleTexture)
        {
            SDL_Rect titleRect = {
                WINDOW_WIDTH / 2 - titleSurface->w / 2,
                100,
                titleSurface->w,
                titleSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture);
        }
        SDL_FreeSurface(titleSurface);
    }

    // Player list with ready status
    int yPos = 250;
    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        if (_MpCtx->players[i].joined)
        {
            char playerText[128];
            const char *readyStatus = _MpCtx->players[i].ready ? "[READY]" : "[NOT READY]";
            SDL_Color statusColor = _MpCtx->players[i].ready ? green : red;

            snprintf(playerText, sizeof(playerText), "%s", _MpCtx->players[i].name);

            // Player name
            SDL_Surface *nameSurface = TTF_RenderText_Solid(_Renderer->fontMedium, playerText, white);
            if (nameSurface)
            {
                SDL_Texture *nameTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, nameSurface);
                if (nameTexture)
                {
                    SDL_Rect nameRect = {
                        WINDOW_WIDTH / 2 - 200,
                        yPos,
                        nameSurface->w,
                        nameSurface->h};
                    SDL_RenderCopy(_Renderer->sdlRenderer, nameTexture, NULL, &nameRect);
                    SDL_DestroyTexture(nameTexture);
                }
                SDL_FreeSurface(nameSurface);
            }

            // Ready status
            SDL_Surface *statusSurface = TTF_RenderText_Solid(_Renderer->fontMedium, readyStatus, statusColor);
            if (statusSurface)
            {
                SDL_Texture *statusTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, statusSurface);
                if (statusTexture)
                {
                    SDL_Rect statusRect = {
                        WINDOW_WIDTH / 2 + 50,
                        yPos,
                        statusSurface->w,
                        statusSurface->h};
                    SDL_RenderCopy(_Renderer->sdlRenderer, statusTexture, NULL, &statusRect);
                    SDL_DestroyTexture(statusTexture);
                }
                SDL_FreeSurface(statusSurface);
            }

            yPos += 60;
        }
    }

    // Instructions
    const char *instructions = _MpCtx->isHost
                                   ? "SPACE READY  ENTER START  ESC LEAVE"
                                   : "SPACE READY  ESC LEAVE";

    SDL_Surface *instrSurface = TTF_RenderText_Solid(_Renderer->fontSmall, instructions, white);
    if (instrSurface)
    {
        SDL_Texture *instrTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, instrSurface);
        if (instrTexture)
        {
            SDL_Rect instrRect = {
                WINDOW_WIDTH / 2 - instrSurface->w / 2,
                WINDOW_HEIGHT - 100,
                instrSurface->w,
                instrSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, instrTexture, NULL, &instrRect);
            SDL_DestroyTexture(instrTexture);
        }
        SDL_FreeSurface(instrSurface);
    }
}

/* Draw turn playing screen */
void Renderer_DrawTurnPlaying(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_Renderer || !_Renderer->sdlRenderer || !_MpCtx)
        return;

    // Draw the local game (background, grid, snake, food, etc.)
    Renderer_DrawBackground(_Renderer, SDL_GetTicks());
    Renderer_DrawBackgroundOverlay(_Renderer);
    Renderer_DrawGrid(_Renderer, 2);
    Renderer_DrawWalls(_Renderer);

    if (_MpCtx->localGame.state == GAME_PLAYING)
    {
        Renderer_DrawSnake(_Renderer, &_MpCtx->localGame.snake);
        Renderer_DrawFood(_Renderer, &_MpCtx->localGame.food);

        // Draw power-ups if any
        for (int i = 0; i < MAX_POWERUPS; i++)
        {
            if (_MpCtx->localGame.powerUps[i].active)
            {
                Renderer_DrawPowerUp(_Renderer, &_MpCtx->localGame.powerUps[i]);
            }
        }
    }

    // Draw HUD border - show attempt during countdown, score during gameplay
    bool showAttempt = (_MpCtx->state == MP_STATE_COUNTDOWN);
    Renderer_DrawHudBorderWithScore(_Renderer, _MpCtx->localGame.snake.score,
                                    _MpCtx->currentAttempt, showAttempt);
}

/* Draw turn waiting screen */
void Renderer_DrawTurnWaiting(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_Renderer || !_Renderer->sdlRenderer || !_MpCtx)
        return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color gray = {150, 150, 150, 255};

    // Title
    SDL_Surface *titleSurface = TTF_RenderText_Solid(_Renderer->fontLarge, "WAITING FOR PLAYERS...", white);
    if (titleSurface)
    {
        SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, titleSurface);
        if (titleTexture)
        {
            SDL_Rect titleRect = {
                WINDOW_WIDTH / 2 - titleSurface->w / 2,
                150,
                titleSurface->w,
                titleSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture);
        }
        SDL_FreeSurface(titleSurface);
    }

    // Show player completion status
    int yPos = 300;
    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        if (_MpCtx->players[i].joined)
        {
            char statusText[128];
            const char *status = _MpCtx->players[i].turnFinished ? "FINISHED" : "PLAYING...";
            SDL_Color statusColor = _MpCtx->players[i].turnFinished ? green : gray;

            snprintf(statusText, sizeof(statusText), "%s: %s", _MpCtx->players[i].name, status);

            SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontMedium, statusText, statusColor);
            if (surface)
            {
                SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
                if (texture)
                {
                    SDL_Rect dest = {
                        WINDOW_WIDTH / 2 - surface->w / 2,
                        yPos,
                        surface->w,
                        surface->h};
                    SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                    SDL_DestroyTexture(texture);
                }
                SDL_FreeSurface(surface);
            }

            yPos += 60;
        }
    }

    // Your best score
    int localIdx = Multiplayer_GetLocalPlayerIndex(_MpCtx);
    if (localIdx >= 0)
    {
        char scoreText[128];
        snprintf(scoreText, sizeof(scoreText), "Your Best Score: %d", _MpCtx->players[localIdx].bestScore);

        SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontMedium, scoreText, green);
        if (surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
            if (texture)
            {
                SDL_Rect dest = {
                    WINDOW_WIDTH / 2 - surface->w / 2,
                    yPos + 60,
                    surface->w,
                    surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

/* Draw turn battle results */
void Renderer_DrawTurnResults(Renderer *_Renderer, MultiplayerContext *_MpCtx)
{
    if (!_Renderer || !_Renderer->sdlRenderer || !_MpCtx)
        return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color platinum = {229, 228, 226, 255};
    SDL_Color gold = {255, 215, 0, 255};
    SDL_Color silver = {192, 192, 192, 255};
    SDL_Color bronze = {205, 127, 50, 255};
    SDL_Color gray = {150, 150, 150, 255};

    // Sort players by best score (descending)
    int sortedIndices[MAX_MULTIPLAYER_PLAYERS];
    int playerCount = 0;
    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        if (_MpCtx->players[i].joined)
        {
            sortedIndices[playerCount++] = i;
        }
    }

    // Bubble sort by bestScore (descending)
    for (int i = 0; i < playerCount - 1; i++)
    {
        for (int j = 0; j < playerCount - i - 1; j++)
        {
            if (_MpCtx->players[sortedIndices[j]].bestScore < _MpCtx->players[sortedIndices[j + 1]].bestScore)
            {
                int temp = sortedIndices[j];
                sortedIndices[j] = sortedIndices[j + 1];
                sortedIndices[j + 1] = temp;
            }
        }
    }

    // Clamp result page index
    if (_MpCtx->resultPageIndex < 0)
        _MpCtx->resultPageIndex = 0;
    if (_MpCtx->resultPageIndex >= playerCount)
        _MpCtx->resultPageIndex = playerCount - 1;

    // Get current player to display
    int displayIdx = sortedIndices[_MpCtx->resultPageIndex];
    int placement = _MpCtx->resultPageIndex + 1;

    // Title
    SDL_Surface *titleSurface = TTF_RenderText_Solid(_Renderer->fontLarge, "TURN BATTLE RESULTS", white);
    if (titleSurface)
    {
        SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, titleSurface);
        if (titleTexture)
        {
            SDL_Rect titleRect = {
                WINDOW_WIDTH / 2 - titleSurface->w / 2,
                50,
                titleSurface->w,
                titleSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture);
        }
        SDL_FreeSurface(titleSurface);
    }

    // Medal (animated) - only for top 4 players
    unsigned int tick = SDL_GetTicks();
    int medalFrame = (tick / 80) % MEDAL_FRAMES;
    SDL_Texture *medalTexture = NULL;
    SDL_Color nameColor = white;

    // Only top 4 players get medals
    if (placement == 1)
    {
        medalTexture = _Renderer->medalPlatinum[medalFrame];
        nameColor = platinum;
    }
    else if (placement == 2)
    {
        medalTexture = _Renderer->medalGold[medalFrame];
        nameColor = gold;
    }
    else if (placement == 3)
    {
        medalTexture = _Renderer->medalSilver[medalFrame];
        nameColor = silver;
    }
    else if (placement == 4)
    {
        medalTexture = _Renderer->medalBronze[medalFrame];
        nameColor = bronze;
    }
    else
    {
        // Players 5+ get no medal, just gray text
        nameColor = gray;
    }

    // Only render medal if we have one (top 4 only)
    if (medalTexture)
    {
        int medalSize = 120;
        SDL_Rect medalRect = {
            WINDOW_WIDTH / 2 - medalSize / 2,
            120,
            medalSize,
            medalSize};
        SDL_RenderCopy(_Renderer->sdlRenderer, medalTexture, NULL, &medalRect);
    }

    // Placement text
    char placementText[32];
    const char *suffix = "TH";
    if (placement == 1)
        suffix = "ST";
    else if (placement == 2)
        suffix = "ND";
    else if (placement == 3)
        suffix = "RD";
    else if (placement == 4)
        suffix = "TH";
    snprintf(placementText, sizeof(placementText), "%d%s PLACE", placement, suffix);

    SDL_Surface *placeSurface = TTF_RenderText_Solid(_Renderer->fontMedium, placementText, nameColor);
    if (placeSurface)
    {
        SDL_Texture *placeTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, placeSurface);
        if (placeTexture)
        {
            SDL_Rect placeRect = {
                WINDOW_WIDTH / 2 - placeSurface->w / 2,
                255,
                placeSurface->w,
                placeSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, placeTexture, NULL, &placeRect);
            SDL_DestroyTexture(placeTexture);
        }
        SDL_FreeSurface(placeSurface);
    }

    // Player name
    SDL_Surface *nameSurface = TTF_RenderText_Solid(_Renderer->fontMedium, _MpCtx->players[displayIdx].name, nameColor);
    if (nameSurface)
    {
        SDL_Texture *nameTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, nameSurface);
        if (nameTexture)
        {
            SDL_Rect nameRect = {
                WINDOW_WIDTH / 2 - nameSurface->w / 2,
                295,
                nameSurface->w,
                nameSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, nameTexture, NULL, &nameRect);
            SDL_DestroyTexture(nameTexture);
        }
        SDL_FreeSurface(nameSurface);
    }

    // Best score (right under player name)
    char bestText[128];
    snprintf(bestText, sizeof(bestText), "BEST SCORE: %d", _MpCtx->players[displayIdx].bestScore);

    SDL_Surface *bestSurface = TTF_RenderText_Solid(_Renderer->fontMedium, bestText, green);
    if (bestSurface)
    {
        SDL_Texture *bestTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, bestSurface);
        if (bestTexture)
        {
            SDL_Rect bestRect = {
                WINDOW_WIDTH / 2 - bestSurface->w / 2,
                330,
                bestSurface->w,
                bestSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, bestTexture, NULL, &bestRect);
            SDL_DestroyTexture(bestTexture);
        }
        SDL_FreeSurface(bestSurface);
    }

    // Show all 3 attempts
    int yPos = 350;
    for (int attempt = 0; attempt < 3; attempt++)
    {
        char attemptText[128];
        snprintf(attemptText, sizeof(attemptText),
                 "Attempt %d: Score %d  |  Length %d  |  Time %.1fs",
                 attempt + 1,
                 _MpCtx->players[displayIdx].attempts[attempt].score,
                 _MpCtx->players[displayIdx].attempts[attempt].length,
                 _MpCtx->players[displayIdx].attempts[attempt].survivalTime / 1000.0f);

        SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontSmall, attemptText, gray);
        if (surface)
        {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
            if (texture)
            {
                SDL_Rect dest = {
                    WINDOW_WIDTH / 2 - surface->w / 2,
                    yPos,
                    surface->w,
                    surface->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }

        yPos += 30;
    }

    // Navigation arrows (if multiple players)
    if (playerCount > 1)
    {
        SDL_Color arrowColor = {255, 255, 255, 200};

        // Left arrow
        if (_MpCtx->resultPageIndex > 0)
        {
            SDL_Surface *leftSurface = TTF_RenderText_Solid(_Renderer->fontLarge, "<", arrowColor);
            if (leftSurface)
            {
                SDL_Texture *leftTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, leftSurface);
                if (leftTexture)
                {
                    SDL_Rect leftRect = {50, WINDOW_HEIGHT / 2 - 30, leftSurface->w, leftSurface->h};
                    SDL_RenderCopy(_Renderer->sdlRenderer, leftTexture, NULL, &leftRect);
                    SDL_DestroyTexture(leftTexture);
                }
                SDL_FreeSurface(leftSurface);
            }
        }

        // Right arrow
        if (_MpCtx->resultPageIndex < playerCount - 1)
        {
            SDL_Surface *rightSurface = TTF_RenderText_Solid(_Renderer->fontLarge, ">", arrowColor);
            if (rightSurface)
            {
                SDL_Texture *rightTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, rightSurface);
                if (rightTexture)
                {
                    SDL_Rect rightRect = {WINDOW_WIDTH - 80, WINDOW_HEIGHT / 2 - 30, rightSurface->w, rightSurface->h};
                    SDL_RenderCopy(_Renderer->sdlRenderer, rightTexture, NULL, &rightRect);
                    SDL_DestroyTexture(rightTexture);
                }
                SDL_FreeSurface(rightSurface);
            }
        }

        // Page indicator removed as per user request
    }

    // Instructions
    SDL_Surface *instrSurface = TTF_RenderText_Solid(_Renderer->fontSmall,
                                                     "Left/Right: Navigate  |  Enter: Lobby  |  ESC: Menu", white);
    if (instrSurface)
    {
        SDL_Texture *instrTexture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, instrSurface);
        if (instrTexture)
        {
            SDL_Rect instrRect = {
                WINDOW_WIDTH / 2 - instrSurface->w / 2,
                WINDOW_HEIGHT - 30,
                instrSurface->w,
                instrSurface->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, instrTexture, NULL, &instrRect);
            SDL_DestroyTexture(instrTexture);
        }
        SDL_FreeSurface(instrSurface);
    }
}

// Render scoreboard with animated medals
void Renderer_DrawScoreboard(Renderer *_Renderer, Scoreboard *_Scoreboard, unsigned int _Tick)
{
    if (!_Renderer || !_Scoreboard)
        return;

    // Title
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gold = {255, 215, 0, 255};

    SDL_Surface *title_surf = TTF_RenderText_Solid(_Renderer->fontLarge, "SCOREBOARD", white);
    if (title_surf)
    {
        SDL_Texture *title_tex = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surf);
        if (title_tex)
        {
            SDL_Rect title_dest = {
                WINDOW_WIDTH / 2 - title_surf->w / 2,
                40,
                title_surf->w,
                title_surf->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, title_tex, NULL, &title_dest);
            SDL_DestroyTexture(title_tex);
        }
        SDL_FreeSurface(title_surf);
    }

    // Display top 4 with medals
    int display_count = _Scoreboard->count > 4 ? 4 : _Scoreboard->count;
    int start_y = 120;
    int spacing = 100;

    // Calculate animation frame (cycle through 13 frames at 100ms per frame)
    int frame = (_Tick / 100) % MEDAL_FRAMES;

    for (int i = 0; i < display_count; i++)
    {
        int y = start_y + (i * spacing);

        // Select medal texture
        SDL_Texture *medal_texture = NULL;
        if (i == 0)
            medal_texture = _Renderer->medalPlatinum[frame];
        else if (i == 1)
            medal_texture = _Renderer->medalGold[frame];
        else if (i == 2)
            medal_texture = _Renderer->medalSilver[frame];
        else if (i == 3)
            medal_texture = _Renderer->medalBronze[frame];

        // Draw medal (left side)
        if (medal_texture)
        {
            int medal_w, medal_h;
            SDL_QueryTexture(medal_texture, NULL, NULL, &medal_w, &medal_h);

            // Scale medals to 64x64
            SDL_Rect medal_dest = {
                50,
                y - 32, // Center vertically
                64,
                64};
            SDL_RenderCopy(_Renderer->sdlRenderer, medal_texture, NULL, &medal_dest);
        }

        // Draw rank, name, and score
        char rank_text[32];
        snprintf(rank_text, sizeof(rank_text), "#%d", i + 1);

        SDL_Color rank_color = (i == 0) ? gold : white;
        SDL_Surface *rank_surf = TTF_RenderText_Solid(_Renderer->fontMedium, rank_text, rank_color);
        if (rank_surf)
        {
            SDL_Texture *rank_tex = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, rank_surf);
            if (rank_tex)
            {
                SDL_Rect rank_dest = {
                    130,
                    y - 20,
                    rank_surf->w,
                    rank_surf->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, rank_tex, NULL, &rank_dest);
                SDL_DestroyTexture(rank_tex);
            }
            SDL_FreeSurface(rank_surf);
        }

        // Name
        SDL_Surface *name_surf = TTF_RenderText_Solid(_Renderer->fontSmall, _Scoreboard->entries[i].name, white);
        if (name_surf)
        {
            SDL_Texture *name_tex = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, name_surf);
            if (name_tex)
            {
                SDL_Rect name_dest = {
                    130,
                    y + 5,
                    name_surf->w,
                    name_surf->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, name_tex, NULL, &name_dest);
                SDL_DestroyTexture(name_tex);
            }
            SDL_FreeSurface(name_surf);
        }

        // Score (right aligned)
        char score_text[32];
        snprintf(score_text, sizeof(score_text), "%d", _Scoreboard->entries[i].score);

        SDL_Surface *score_surf = TTF_RenderText_Solid(_Renderer->fontMedium, score_text, gold);
        if (score_surf)
        {
            SDL_Texture *score_tex = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, score_surf);
            if (score_tex)
            {
                SDL_Rect score_dest = {
                    WINDOW_WIDTH - score_surf->w - 50,
                    y - 15,
                    score_surf->w,
                    score_surf->h};
                SDL_RenderCopy(_Renderer->sdlRenderer, score_tex, NULL, &score_dest);
                SDL_DestroyTexture(score_tex);
            }
            SDL_FreeSurface(score_surf);
        }
    }

    // Instructions at bottom
    SDL_Color gray = {150, 150, 150, 255};
    SDL_Surface *inst_surf = TTF_RenderText_Solid(_Renderer->fontSmall, "ESC BACK", white);
    if (inst_surf)
    {
        SDL_Texture *inst_tex = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, inst_surf);
        if (inst_tex)
        {
            SDL_Rect inst_dest = {
                WINDOW_WIDTH / 2 - inst_surf->w / 2,
                WINDOW_HEIGHT - 60,
                inst_surf->w,
                inst_surf->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, inst_tex, NULL, &inst_dest);
            SDL_DestroyTexture(inst_tex);
        }
        SDL_FreeSurface(inst_surf);
    }
}

/* Main frame rendering - handles all game states */
void Renderer_DrawFrame(Renderer *_Renderer, Game *_Game, MultiplayerContext *_MpCtx,
                        Scoreboard *_Scoreboard, unsigned int _CurrentTime, int _MainMenuSelection, void *_Audio)
{
    (void)_Audio; // Used in multiplayer for chat sounds

    // Clear screen
    Renderer_Clear(_Renderer);

    // Set background based on current mode
    if (_Game->state == GAME_MULTIPLAYER && _MpCtx)
    {
        _Renderer->currentBackground = _MpCtx->selectedBackground;
    }
    else
    {
        _Renderer->currentBackground = _Game->selectedBackground;
    }

    // Draw background
    Renderer_DrawBackground(_Renderer, _CurrentTime);
    Renderer_DrawBackgroundOverlay(_Renderer);

    // Draw based on state
    if (_Game->state == GAME_LOADING)
    {
        Renderer_DrawLoadingScreen(_Renderer, _Game->loadingProgress, _CurrentTime);
    }
    else if (_Game->state == GAME_MAIN_MENU)
    {
        Renderer_DrawMainMenu(_Renderer, _MainMenuSelection);
    }
    else if (_Game->state == GAME_MODE_SELECT)
    {
        Renderer_DrawModeSelect(_Renderer, _Game);
    }
    else if (_Game->state == GAME_NAME_INPUT)
    {
        Renderer_DrawNameInput(_Renderer, _Game, _CurrentTime);
    }
    else if (_Game->state == GAME_MENU)
    {
        Renderer_DrawMenu(_Renderer, _Game->selectedBackground);
    }
    else if (_Game->state == GAME_SCOREBOARD)
    {
        Renderer_DrawScoreboard(_Renderer, _Scoreboard, _CurrentTime);
    }
    else if (_Game->state == GAME_OPTIONS)
    {
        Renderer_DrawOptions(_Renderer, _Game);
    }
    else if (_Game->state == GAME_PLAYING || _Game->state == GAME_OVER || _Game->state == GAME_PAUSED)
    {
        // Singleplayer _Game
        Renderer_DrawGrid(_Renderer, _Game->settings.gridAlpha);
        Renderer_DrawSnake(_Renderer, &_Game->snake);
        Renderer_DrawFood(_Renderer, &_Game->food);

        // Draw all active power-ups (Power-Up mode only)
        if (_Game->gameMode == MODE_POWERUP)
        {
            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                if (_Game->powerUps[i].active)
                {
                    Renderer_DrawPowerUp(_Renderer, &_Game->powerUps[i]);
                }
            }
        }

        // Draw explosion if active
        if (_Game->explosion.active)
        {
            Renderer_DrawExplosion(_Renderer, _Game->explosion.position, _Game->explosion.frame);
        }

        // Game over overlay
        if (_Game->state == GAME_OVER)
        {
            // Semi-transparent background
            SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, 200);
            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(_Renderer->sdlRenderer, &overlay);

            // "GAME OVER" title - blink if highscore
            SDL_Color white = {255, 255, 255, 255};
            SDL_Color blink_colors[] = {
                {255, 100, 100, 255}, // Red
                {255, 200, 100, 255}, // Orange
                {255, 255, 100, 255}, // Yellow
                {100, 255, 100, 255}, // Green
                {100, 200, 255, 255}, // Cyan
                {200, 100, 255, 255}  // Purple
            };

            SDL_Color title_color = white;
            if (_Game->isHighscore)
            {
                int blink_phase = (_CurrentTime / 250) % 6; // Blink faster for excitement
                title_color = blink_colors[blink_phase];
            }

            SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontLarge, "GAME OVER", title_color);
            if (title_surface)
            {
                SDL_Texture *title_texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
                if (title_texture)
                {
                    SDL_Rect title_dest = {
                        WINDOW_WIDTH / 2 - title_surface->w / 2,
                        WINDOW_HEIGHT / 2 - 80,
                        title_surface->w,
                        title_surface->h};
                    SDL_RenderCopy(_Renderer->sdlRenderer, title_texture, NULL, &title_dest);
                    SDL_DestroyTexture(title_texture);
                }
                SDL_FreeSurface(title_surface);
            }

            // Menu options
            const char *options[] = {"TRY AGAIN", "MAIN MENU"};
            SDL_Color selected_color = {0, 255, 0, 255};
            SDL_Color normal_color = {200, 200, 200, 255};

            for (int i = 0; i < 2; i++)
            {
                SDL_Color color = (i == _Game->gameOverSelection) ? selected_color : normal_color;
                SDL_Surface *opt_surface = TTF_RenderText_Solid(_Renderer->fontMedium, options[i], color);
                if (opt_surface)
                {
                    SDL_Texture *opt_texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, opt_surface);
                    if (opt_texture)
                    {
                        SDL_Rect opt_dest = {
                            WINDOW_WIDTH / 2 - opt_surface->w / 2,
                            WINDOW_HEIGHT / 2 + 20 + (i * 40),
                            opt_surface->w,
                            opt_surface->h};
                        SDL_RenderCopy(_Renderer->sdlRenderer, opt_texture, NULL, &opt_dest);
                        SDL_DestroyTexture(opt_texture);
                    }
                    SDL_FreeSurface(opt_surface);
                }
            }
        }

        // Pause menu overlay
        if (_Game->state == GAME_PAUSED)
        {
            Renderer_DrawPauseMenu(_Renderer, _Game->pauseMenuSelection);
        }

        // Draw HUD border with score (not in pause state)
        if (_Game->state != GAME_PAUSED)
        {
            Renderer_DrawHudBorderWithCombo(_Renderer, _Game->snake.score, _Game->comboCount, _Game->comboMultiplier, _Game->gameMode, _Game->settings.comboEffects, _Game->level, _Game->xp, _Game->xpToNextLevel);
        }

        // Draw level-up effect (Power-Up mode only)
        if (_Game->levelUpActive && _Game->gameMode == MODE_POWERUP)
        {
            unsigned int elapsed = _CurrentTime - _Game->levelUpStartTime;
            const unsigned int effectDuration = 2000; // 2 seconds

            if (elapsed < effectDuration)
            {
                // Flash overlay
                int flashAlpha = 0;
                if (elapsed < 200) // Initial bright flash
                {
                    flashAlpha = 200 - (elapsed * 200 / 200);
                }
                else if (elapsed % 400 < 200) // Pulsing after
                {
                    flashAlpha = 80;
                }

                if (flashAlpha > 0)
                {
                    SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 220, 255, flashAlpha);
                    SDL_Rect flashRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
                    SDL_RenderFillRect(_Renderer->sdlRenderer, &flashRect);
                }

                // "LEVEL UP!" text with zoom and fade
                float progress = (float)elapsed / (float)effectDuration;
                float scale = 2.0f - progress; // Start big, shrink to 1.0
                int alpha = (int)(255 * (1.0f - progress)); // Fade out

                char levelText[32];
                snprintf(levelText, sizeof(levelText), "LEVEL %d!", _Game->level);

                SDL_Color cyan = {100, 220, 255, (Uint8)alpha};
                SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontLarge, levelText, cyan);
                if (surface)
                {
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
                    if (texture)
                    {
                        SDL_SetTextureAlphaMod(texture, alpha);

                        int w = (int)(surface->w * scale);
                        int h = (int)(surface->h * scale);

                        SDL_Rect dest = {
                            WINDOW_WIDTH / 2 - w / 2,
                            WINDOW_HEIGHT / 2 - h / 2,
                            w,
                            h
                        };
                        SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                        SDL_DestroyTexture(texture);
                    }
                    SDL_FreeSurface(surface);
                }
            }
            else
            {
                // Effect finished
                _Game->levelUpActive = false;
            }
        }
    }
    else if (_Game->state == GAME_MULTIPLAYER && _MpCtx)
    {
        // Multiplayer rendering
        if (_MpCtx->state == MP_STATE_MENU)
        {
            Renderer_DrawMultiplayerMenu(_Renderer, _MpCtx);
        }
        else if (_MpCtx->state == MP_STATE_ENTERING_NAME)
        {
            Renderer_DrawMultiplayerRoomName(_Renderer, _MpCtx, _CurrentTime);
        }
        else if (_MpCtx->state == MP_STATE_BROWSING)
        {
            Renderer_DrawMultiplayerBrowse(_Renderer, _MpCtx);
        }
        else if (_MpCtx->state == MP_STATE_HOSTING || _MpCtx->state == MP_STATE_JOINING || _MpCtx->state == MP_STATE_DISCONNECTED)
        {
            // Show status message
            const char *status_msg = "Connecting...";
            if (_MpCtx->state == MP_STATE_HOSTING)
                status_msg = "Hosting _Game...";
            else if (_MpCtx->state == MP_STATE_JOINING)
                status_msg = "Joining _Game...";
            else if (_MpCtx->state == MP_STATE_DISCONNECTED)
                status_msg = _MpCtx->errorMessage;

            SDL_Color white = {255, 255, 255, 255};
            SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontMedium, status_msg, white);
            if (surface)
            {
                SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
                if (texture)
                {
                    SDL_Rect dest = {
                        WINDOW_WIDTH / 2 - surface->w / 2,
                        WINDOW_HEIGHT / 2 - surface->h / 2,
                        surface->w,
                        surface->h};
                    SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                    SDL_DestroyTexture(texture);
                }
                SDL_FreeSurface(surface);
            }
        }
        else if (_MpCtx->state == MP_STATE_READY_UP)
        {
            Renderer_DrawReadyUp(_Renderer, _MpCtx);
        }
        else if (_MpCtx->state == MP_STATE_TURN_PLAYING)
        {
            Renderer_DrawTurnPlaying(_Renderer, _MpCtx);
        }
        else if (_MpCtx->state == MP_STATE_TURN_WAITING)
        {
            Renderer_DrawTurnWaiting(_Renderer, _MpCtx);
        }
        else if (_MpCtx->state == MP_STATE_TURN_RESULTS)
        {
            Renderer_DrawTurnResults(_Renderer, _MpCtx);
        }
        else if (_MpCtx->state == MP_STATE_LOBBY ||
                 (_MpCtx->state == MP_STATE_CHATTING && _MpCtx->previousState == MP_STATE_LOBBY) ||
                 (_MpCtx->state == MP_STATE_CHANGING_NICK && _MpCtx->previousState == MP_STATE_LOBBY))
        {
            Renderer_DrawMultiplayerLobby(_Renderer, _MpCtx);

            // Overlay chat or nick change screens
            if (_MpCtx->state == MP_STATE_CHATTING && _MpCtx->previousState == MP_STATE_LOBBY)
            {
                Renderer_DrawChatOverlay(_Renderer, _MpCtx);
            }
            else if (_MpCtx->state == MP_STATE_CHANGING_NICK && _MpCtx->previousState == MP_STATE_LOBBY)
            {
                Renderer_DrawNickOverlay(_Renderer, _MpCtx);
            }
        }
        else if (_MpCtx->state == MP_STATE_PLAYING || _MpCtx->state == MP_STATE_GAME_OVER ||
                 _MpCtx->state == MP_STATE_COUNTDOWN)
        {
            // For turn battle countdown, draw turn playing background
            if (_MpCtx->state == MP_STATE_COUNTDOWN && _MpCtx->gameMode == MP_MODE_TURN_BATTLE)
            {
                Renderer_DrawTurnPlaying(_Renderer, _MpCtx);
            }
            else
            {
                Renderer_DrawMultiplayerGame(_Renderer, _MpCtx);
            }

            // Show intense countdown (3, 2, 1) with zoom-in and dark pulsing overlay
            if (_MpCtx->state == MP_STATE_COUNTDOWN)
            {
                int seconds_left = (_MpCtx->gameStartTime - _CurrentTime) / 1000 + 1;
                int time_in_second = _CurrentTime % 1000;

                // Very dark pulsing overlay - game barely visible until countdown reaches 1
                SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);

                // Base darkness levels - much darker now
                int base_alpha = 0;
                if (seconds_left == 3)
                    base_alpha = 240; // Almost black
                else if (seconds_left == 2)
                    base_alpha = 220; // Still very dark
                else if (seconds_left == 1)
                    base_alpha = 180 - (time_in_second * 180 / 1000); // Fade out during last second

                // Strong pulsating effect
                float pulse_cycle = (_CurrentTime % 250) / 250.0f;
                float pulse = (sin(pulse_cycle * 3.14159f * 2) + 1.0f) / 2.0f;
                int pulse_alpha = (int)(pulse * 40); // Strong pulse

                int final_alpha = base_alpha + pulse_alpha;
                if (final_alpha < 0) final_alpha = 0;
                if (final_alpha > 255) final_alpha = 255;

                SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, final_alpha);
                SDL_Rect overlayRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
                SDL_RenderFillRect(_Renderer->sdlRenderer, &overlayRect);

                const char *countdown_text = NULL;
                if (seconds_left == 3)
                    countdown_text = "3";
                else if (seconds_left == 2)
                    countdown_text = "2";
                else if (seconds_left == 1)
                    countdown_text = "1";

                if (countdown_text)
                {
                    SDL_Color white = {255, 255, 255, 255};

                    // Zoom-in effect - starts big (2.5x) and shrinks to normal size (1.0x)
                    float scale = 2.5f - (1.5f * (time_in_second / 1000.0f));

                    // Fade in effect at start of each second
                    int text_alpha = (time_in_second < 100) ? (time_in_second * 255 / 100) : 255;

                    SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontLarge, countdown_text, white);
                    if (surface)
                    {
                        SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
                        if (texture)
                        {
                            SDL_SetTextureAlphaMod(texture, text_alpha);

                            int scaled_w = (int)(surface->w * scale);
                            int scaled_h = (int)(surface->h * scale);

                            SDL_Rect dest = {
                                WINDOW_WIDTH / 2 - scaled_w / 2,
                                WINDOW_HEIGHT / 2 - scaled_h / 2,
                                scaled_w,
                                scaled_h};
                            SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                            SDL_DestroyTexture(texture);
                        }
                        SDL_FreeSurface(surface);
                    }
                }
            }

            // Game over overlay
            if (_MpCtx->state == MP_STATE_GAME_OVER)
            {
                // Semi-transparent background
                SDL_SetRenderDrawBlendMode(_Renderer->sdlRenderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(_Renderer->sdlRenderer, 0, 0, 0, 200);
                SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
                SDL_RenderFillRect(_Renderer->sdlRenderer, &overlay);

                // Determine winner
                int winner = -1;
                for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
                {
                    if (_MpCtx->players[i].joined && _MpCtx->players[i].alive)
                    {
                        winner = i;
                        break;
                    }
                }

                const char *game_over_text = winner >= 0 ? "PLAYER %d WINS!" : "DRAW!";
                char final_text[64];
                if (winner >= 0)
                {
                    snprintf(final_text, sizeof(final_text), game_over_text, winner + 1);
                }
                else
                {
                    strncpy(final_text, game_over_text, sizeof(final_text) - 1);
                }

                // Game over title
                SDL_Color white = {255, 255, 255, 255};
                SDL_Surface *title_surface = TTF_RenderText_Solid(_Renderer->fontLarge, final_text, white);
                if (title_surface)
                {
                    SDL_Texture *title_texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, title_surface);
                    if (title_texture)
                    {
                        SDL_Rect title_dest = {
                            WINDOW_WIDTH / 2 - title_surface->w / 2,
                            WINDOW_HEIGHT / 2 - 80,
                            title_surface->w,
                            title_surface->h};
                        SDL_RenderCopy(_Renderer->sdlRenderer, title_texture, NULL, &title_dest);
                        SDL_DestroyTexture(title_texture);
                    }
                    SDL_FreeSurface(title_surface);
                }

                // Menu options (only show if host)
                if (_MpCtx->isHost)
                {
                    const char *options[] = {"TRY AGAIN", "QUIT GAME"};
                    SDL_Color selected_color = {0, 255, 0, 255};
                    SDL_Color normal_color = {200, 200, 200, 255};

                    for (int i = 0; i < 2; i++)
                    {
                        SDL_Color color = (i == _MpCtx->gameOverSelection) ? selected_color : normal_color;
                        SDL_Surface *opt_surface = TTF_RenderText_Solid(_Renderer->fontMedium, options[i], color);
                        if (opt_surface)
                        {
                            SDL_Texture *opt_texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, opt_surface);
                            if (opt_texture)
                            {
                                SDL_Rect opt_dest = {
                                    WINDOW_WIDTH / 2 - opt_surface->w / 2,
                                    WINDOW_HEIGHT / 2 + 20 + (i * 40),
                                    opt_surface->w,
                                    opt_surface->h};
                                SDL_RenderCopy(_Renderer->sdlRenderer, opt_texture, NULL, &opt_dest);
                                SDL_DestroyTexture(opt_texture);
                            }
                            SDL_FreeSurface(opt_surface);
                        }
                    }
                }
                else
                {
                    // Client: show waiting message
                    SDL_Color gray = {150, 150, 150, 255};
                    SDL_Surface *wait_surface = TTF_RenderText_Solid(_Renderer->fontSmall, "Waiting for host...", gray);
                    if (wait_surface)
                    {
                        SDL_Texture *wait_texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, wait_surface);
                        if (wait_texture)
                        {
                            SDL_Rect wait_dest = {
                                WINDOW_WIDTH / 2 - wait_surface->w / 2,
                                WINDOW_HEIGHT / 2 + 60,
                                wait_surface->w,
                                wait_surface->h};
                            SDL_RenderCopy(_Renderer->sdlRenderer, wait_texture, NULL, &wait_dest);
                            SDL_DestroyTexture(wait_texture);
                        }
                        SDL_FreeSurface(wait_surface);
                    }
                }
            }

            // Draw HUD for multiplayer
            Renderer_DrawMultiplayerHudBorder(_Renderer, _MpCtx);
        }
    }

    // Apply brightness overlay (affects entire screen)
    Renderer_DrawBrightnessOverlay(_Renderer, _Game->settings.brightness);

    // Present the frame
    Renderer_Present(_Renderer);
}
