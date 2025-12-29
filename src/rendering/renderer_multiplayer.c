#include "../../include/rendering/renderer_multiplayer.h"
#include "../../include/rendering/renderer_hud.h"
#include "../../include/rendering/renderer_game.h"
#include "../../include/config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

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
    const char *map_names[] = {"de_cyberpunk", "de_forest", "de_underwater", "de_mountain", "de_country", "de_city"};
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
            const char *map_names[] = {"de_cyberpunk", "de_forest", "de_underwater", "de_mountain", "de_country", "de_city"};
            int map_id = _MpCtx->browsedGames[i].mapId;
            if (map_id < 0 || map_id >= 6)
                map_id = 0; // Default to first map if invalid
            const char *map_name = map_names[map_id];

            // Shorten name if needed
            char short_name[32];
            const char *name_to_show = _MpCtx->browsedGames[i].name;
            if (strlen(name_to_show) > 20)
            {
                strncpy(short_name, name_to_show, 17);
                short_name[17] = '\0';
                strncat(short_name, "...", sizeof(short_name) - strlen(short_name) - 1);
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
    if (_MpCtx->isHost)
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

    // Draw HUD border with full info (XP, level, combo) in Power-Up mode
    // Show attempt number during countdown
    if (_MpCtx->state == MP_STATE_COUNTDOWN)
    {
        Renderer_DrawHudBorderWithScore(_Renderer, _MpCtx->localGame.snake.score,
                                        _MpCtx->currentAttempt, true);
    }
    else
    {
        Renderer_DrawHudBorderWithCombo(_Renderer, _MpCtx->localGame.snake.score,
                                        _MpCtx->localGame.comboCount,
                                        _MpCtx->localGame.comboMultiplier,
                                        _MpCtx->localGame.gameMode,
                                        true, // combo effects
                                        _MpCtx->localGame.level,
                                        _MpCtx->localGame.xp,
                                        _MpCtx->localGame.xpToNextLevel);
    }

    // Draw level-up effect (Power-Up mode only)
    if (_MpCtx->localGame.levelUpActive && _MpCtx->localGame.gameMode == MODE_POWERUP)
    {
        unsigned int currentTime = SDL_GetTicks();
        unsigned int elapsed = currentTime - _MpCtx->localGame.levelUpStartTime;
        const unsigned int effectDuration = 2000; // 2 seconds

        if (elapsed < effectDuration)
        {
            float progress = (float)elapsed / (float)effectDuration;
            float scale = 2.0f - progress;
            int alpha = (int)(255 * (1.0f - progress));

            char levelText[32];
            snprintf(levelText, sizeof(levelText), "LEVEL %d!", _MpCtx->localGame.level);

            SDL_Color cyan = {100, 220, 255, (Uint8)alpha};
            SDL_Surface *surface = TTF_RenderText_Solid(_Renderer->fontLarge, levelText, cyan);
            if (surface)
            {
                SDL_Texture *texture = SDL_CreateTextureFromSurface(_Renderer->sdlRenderer, surface);
                if (texture)
                {
                    int w = (int)(surface->w * scale);
                    int h = (int)(surface->h * scale);
                    SDL_Rect dest = {
                        WINDOW_WIDTH / 2 - w / 2,
                        WINDOW_HEIGHT / 2 - h / 2,
                        w, h};
                    SDL_SetTextureAlphaMod(texture, (Uint8)alpha);
                    SDL_RenderCopy(_Renderer->sdlRenderer, texture, NULL, &dest);
                    SDL_DestroyTexture(texture);
                }
                SDL_FreeSurface(surface);
            }
        }
        else
        {
            _MpCtx->localGame.levelUpActive = false;
        }
    }
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
