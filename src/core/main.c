#include "../../include/core/game.h"
#include "../../include/renderer.h"
#include "../../include/audio/audio.h"
#include "../../include/multiplayer/multiplayer.h"
#include "../../include/utils/scoreboard.h"
#include "../../include/utils/logger.h"
#include "../../include/core/runtime_config.h"
#include "../../include/utils/debug_draw.h"
#include "../../include/input.h"
#include "../../include/core/state_machine.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // Initialize Logger (early, before other systems)
    Logger logger;
    Logger_Init(&logger, "snejk2d.log", LOG_LEVEL_INFO, LOG_OUTPUT_BOTH);
    Logger_Info(&logger, "=== SNEJK2D Starting ===");

    // Initialize Runtime Configuration
    RuntimeConfig_Init(&g_RuntimeConfig);
    RuntimeConfig_SetLogger(&logger);

    // Try to load config from file (optional)
    const char *configPath = "snejk2d.conf";
    if (RuntimeConfig_Load(&g_RuntimeConfig, configPath))
    {
        Logger_Info(&logger, "Loaded configuration from %s", configPath);
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        Logger_Error(&logger, "SDL initialization failed: %s", SDL_GetError());
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        Logger_Shutdown(&logger);
        return 1;
    }
    Logger_Info(&logger, "SDL initialized successfully");

    // Initialize systems
    Game game;
    Game_Init(&game);

    Scoreboard scoreboard;
    Scoreboard_Load(&scoreboard);

    Renderer renderer;
    if (!Renderer_Init(&renderer))
    {
        Logger_Error(&logger, "Renderer initialization failed");
        SDL_Quit();
        Logger_Shutdown(&logger);
        return 1;
    }
    Logger_Info(&logger, "Renderer initialized successfully");

    AudioSystem audio;
    Audio_Init(&audio);
    Audio_PlayMenuMusic(&audio);
    Logger_Info(&logger, "Audio system initialized");

    // Subscribe audio system to events
    if (game.eventDispatcher)
    {
        Audio_SubscribeToEvents(&audio, game.eventDispatcher);
        Logger_Info(&logger, "Audio subscribed to event system");
    }

    // Initialize Debug Draw system
    DebugDrawConfig debugDraw;
    DebugDraw_Init(&debugDraw);
    Logger_Info(&logger, "Debug visualization system initialized");

    MultiplayerContext *mpCtx = NULL;
    GameState lastState = GAME_MAIN_MENU;
    MultiplayerState lastMpState = MP_STATE_DISCONNECTED;

    // Initialize state machine
    StateMachine stateMachine;
    StateContext stateContext = {
        .game = &game,
        .renderer = &renderer,
        .audio = &audio,
        .scoreboard = &scoreboard,
        .mpCtx = &mpCtx,
        .currentTime = 0,
        .mainMenuSelection = &g_main_menu_selection,
        .debugDraw = &debugDraw,
        .logger = &logger
    };
    StateMachine_Init(&stateMachine, &stateContext);
    Logger_Info(&logger, "State machine initialized");

    // Start with loading state
    StateMachine_ChangeState(&stateMachine, StateMachine_GetStateForEnum(GAME_LOADING));

    // Main game loop
    while (game.running)
    {
        unsigned int currentTime = SDL_GetTicks();
        stateContext.currentTime = currentTime;

        // Handle state transitions
        if (stateMachine.stateChanged || (stateMachine.currentState &&
            StateMachine_GetStateForEnum(game.state) != stateMachine.currentState))
        {
            StateMachine_ChangeState(&stateMachine, StateMachine_GetStateForEnum(game.state));
        }

        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                game.running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                // F3 toggles debug visualization
                if (event.key.keysym.sym == SDLK_F3)
                {
                    DebugDraw_ToggleAll(&debugDraw);
                }
                else
                {
                    StateMachine_HandleInput(&stateMachine, &event);
                }
            }
            else
            {
                StateMachine_HandleInput(&stateMachine, &event);
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

        // Update state machine
        StateMachine_Update(&stateMachine, 16);

        // Update explosion (handled globally)
        Game_UpdateExplosion(&game, currentTime);

        // Render everything
        StateMachine_Render(&stateMachine);
        Renderer_DrawFrame(&renderer, &game, mpCtx, &scoreboard, currentTime, g_main_menu_selection, &audio);

        // Render debug overlays (if enabled)
        DebugDraw_Render(&renderer, &game, mpCtx, &debugDraw, currentTime);

        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    Logger_Info(&logger, "Shutting down...");

    StateMachine_Shutdown(&stateMachine);
    if (mpCtx)
    {
        Multiplayer_Destroy(mpCtx);
    }

    // Cleanup event system
    if (game.eventDispatcher)
    {
        EventSystem_Shutdown(game.eventDispatcher);
        free(game.eventDispatcher);
        game.eventDispatcher = NULL;
    }

    Audio_Cleanup(&audio);
    Renderer_Cleanup(&renderer);
    SDL_Quit();

    Logger_Info(&logger, "=== SNEJK2D Shutdown Complete ===");
    Logger_PrintStats(&logger);
    Logger_Shutdown(&logger);

    return 0;
}
