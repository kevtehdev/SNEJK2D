#include "../../include/rendering/renderer_hud.h"
#include "../../include/config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdio.h>

/* Static helper: Draw HUD border with pulse effect based on combo */
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

/* Draw HUD border with score or attempt counter */
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

/* Draw HUD border with combo, XP bar, and level */
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

/* Draw minimal HUD (simple score display) */
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

/* Draw multiplayer HUD border with player scores */
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
        // Don't show DEAD status during countdown
        const char *status = (_MpCtx->players[0].alive || _MpCtx->state == MP_STATE_COUNTDOWN) ? "" : " [DEAD]";
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
        // Don't show DEAD status during countdown
        const char *status = (_MpCtx->players[1].alive || _MpCtx->state == MP_STATE_COUNTDOWN) ? "" : " [DEAD]";
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
