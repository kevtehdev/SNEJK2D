#include "../../include/rendering/renderer_ui.h"
#include "../../include/rendering/renderer_core.h"
#include "../../include/rendering/renderer_background.h"
#include "../../include/rendering/renderer_game.h"
#include "../../include/rendering/renderer_hud.h"
#include "../../include/rendering/renderer_menus.h"
#include "../../include/rendering/renderer_multiplayer.h"
#include "../../include/rendering/renderer_effects.h"
#include "../../include/config.h"
#include <SDL2/SDL.h>

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
