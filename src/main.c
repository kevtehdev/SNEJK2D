#include "../include/game.h"
#include "../include/renderer.h"
#include "../include/audio.h"
#include "../include/multiplayer.h"
#include "../include/scoreboard.h"
#include "../include/input.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize systems
    Game game;
    Game_Init(&game);

    Scoreboard scoreboard;
    Scoreboard_Load(&scoreboard);

    Renderer renderer;
    if (!Renderer_Init(&renderer))
    {
        SDL_Quit();
        return 1;
    }

    AudioSystem audio;
    Audio_Init(&audio);
    Audio_PlayMenuMusic(&audio);

    MultiplayerContext *mpCtx = NULL;
    GameState lastState = GAME_MAIN_MENU;
    MultiplayerState lastMpState = MP_STATE_DISCONNECTED;

    // Main game loop
    while (game.running)
    {
        unsigned int currentTime = SDL_GetTicks();

        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                game.running = false;
            }
            else if (game.state == GAME_MAIN_MENU)
            {
                Input_HandleMainMenuInput(&game, &event, &mpCtx);
            }
            else
            {
                Input_HandleInput(&game, &event, &mpCtx, &scoreboard, &audio);
            }
        }

        // Handle music state changes
        MultiplayerState currentMpState = mpCtx ? mpCtx->state : MP_STATE_DISCONNECTED;
        if (lastState != game.state || lastMpState != currentMpState)
        {
            Audio_UpdateMusicForState(&audio, game.state, currentMpState, game.settings.menuMusicEnabled, game.settings.gameMusicEnabled);
            lastState = game.state;
            lastMpState = currentMpState;
        }

        // Update game logic
        if (game.state == GAME_PLAYING)
        {
            Game_Update(&game);
        }
        else if (game.state == GAME_MULTIPLAYER && mpCtx)
        {
            if (mpCtx->isHost && mpCtx->state == MP_STATE_PLAYING)
            {
                Multiplayer_HostUpdate(mpCtx, currentTime);
            }
            Multiplayer_UpdateCountdown(mpCtx, currentTime);
        }

        // Update explosion
        Game_UpdateExplosion(&game, currentTime);

        // Trigger explosion on death (singleplayer)
        if (game.state == GAME_OVER && !game.explosion.active)
        {
            Game_TriggerExplosion(&game, game.snake.segments[0], currentTime);

            // Save score (only once) and check if highscore
            if (!game.scoreSaved && game.snake.score > 0)
            {
                Scoreboard_AddScore(&scoreboard, game.snake.score, game.playerName);

                // Check if this is a highscore (top 4)
                game.isHighscore = false;
                for (int i = 0; i < 4 && i < scoreboard.count; i++)
                {
                    if (scoreboard.entries[i].score == game.snake.score &&
                        strcmp(scoreboard.entries[i].name, game.playerName) == 0)
                    {
                        game.isHighscore = true;
                        break;
                    }
                }

                Scoreboard_Save(&scoreboard);
                game.scoreSaved = true;
            }
        }

        // Render everything
        Renderer_DrawFrame(&renderer, &game, mpCtx, &scoreboard, currentTime, g_main_menu_selection);

        SDL_Delay(16);  // ~60 FPS
    }

    // Cleanup
    if (mpCtx)
    {
        Multiplayer_Destroy(mpCtx);
    }
    Audio_Cleanup(&audio);
    Renderer_Cleanup(&renderer);
    SDL_Quit();

    return 0;
}
