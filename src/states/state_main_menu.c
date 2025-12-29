#include "../../include/core/state_machine.h"
#include "../../include/input/input_menu.h"
#include "../../include/audio/audio.h"
#include <SDL2/SDL.h>

/* Main Menu state - singleplayer/multiplayer/scoreboard/options/exit selection */

static void MainMenuState_Enter(StateContext *_Ctx)
{
    // Start menu music
    AudioSystem *audio = (AudioSystem*)_Ctx->audio;
    Audio_PlayMenuMusic(audio);
}

static void MainMenuState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    // No game logic updates needed for menu
    (void)_Ctx;
    (void)_DeltaTime;
}

static void MainMenuState_Render(StateContext *_Ctx)
{
    // Rendering is handled by Renderer_DrawFrame in main loop
    (void)_Ctx;
}

static void MainMenuState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    // Delegate to input handler
    Input_HandleMainMenuInput(_Ctx->game, _Event, _Ctx->mpCtx, _Ctx->audio);
}

static void MainMenuState_Exit(StateContext *_Ctx)
{
    // Cleanup if needed
    (void)_Ctx;
}

/* Global state instance */
State g_MainMenuState = {
    .name = "MainMenu",
    .enter = MainMenuState_Enter,
    .update = MainMenuState_Update,
    .render = MainMenuState_Render,
    .handleInput = MainMenuState_HandleInput,
    .exit = MainMenuState_Exit
};
