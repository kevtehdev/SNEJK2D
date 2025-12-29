#include "../../include/rendering/renderer_menus.h"
#include "../../include/config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

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
    const char *bgNames[NUM_BACKGROUNDS] = {"cyberpunk", "forest", "underwater", "mountain", "country", "city"};
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
                WINDOW_HEIGHT - 35,
                inst_surf->w,
                inst_surf->h};
            SDL_RenderCopy(_Renderer->sdlRenderer, inst_tex, NULL, &inst_dest);
            SDL_DestroyTexture(inst_tex);
        }
        SDL_FreeSurface(inst_surf);
    }
}
