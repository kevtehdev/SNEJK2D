#include "../../include/core/state_machine.h"
#include "../../include/rendering/renderer_ui.h"
#include <SDL2/SDL.h>

/* Loading state - displays retro loading screen */

static void LoadingState_Enter(StateContext *_Ctx)
{
    // Initialize loading start time
    _Ctx->game->loadingStartTime = SDL_GetTicks();
    _Ctx->game->loadingProgress = 0.0f;
}

static void LoadingState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_DeltaTime;  // Unused

    // Update loading progress (simulate 2.5 second loading time)
    unsigned int elapsed = _Ctx->currentTime - _Ctx->game->loadingStartTime;
    _Ctx->game->loadingProgress = (float)elapsed / 2500.0f;

    if (_Ctx->game->loadingProgress >= 1.0f)
    {
        _Ctx->game->loadingProgress = 1.0f;
        // Transition to main menu
        _Ctx->game->state = GAME_MAIN_MENU;
    }
}

static void LoadingState_Render(StateContext *_Ctx)
{
    // Rendering is handled by Renderer_DrawFrame in main loop
    // This is just a placeholder for future direct rendering
    (void)_Ctx;
}

static void LoadingState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    // No input during loading screen
    (void)_Ctx;
    (void)_Event;
}

static void LoadingState_Exit(StateContext *_Ctx)
{
    // Cleanup if needed
    (void)_Ctx;
}

/* Global state instance */
State g_LoadingState = {
    .name = "Loading",
    .enter = LoadingState_Enter,
    .update = LoadingState_Update,
    .render = LoadingState_Render,
    .handleInput = LoadingState_HandleInput,
    .exit = LoadingState_Exit
};
