#include "../../include/core/state_machine.h"
#include "../../include/input/input_menu.h"

/* Paused state - game paused */

static void PausedState_Enter(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void PausedState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_Ctx;
    (void)_DeltaTime;
}

static void PausedState_Render(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void PausedState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    Input_HandlePauseMenuInput(_Ctx->game, _Event);
}

static void PausedState_Exit(StateContext *_Ctx)
{
    (void)_Ctx;
}

State g_PausedState = {
    .name = "Paused",
    .enter = PausedState_Enter,
    .update = PausedState_Update,
    .render = PausedState_Render,
    .handleInput = PausedState_HandleInput,
    .exit = PausedState_Exit
};
