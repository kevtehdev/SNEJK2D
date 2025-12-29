#include "../../include/core/state_machine.h"
#include "../../include/input/input_menu.h"

/* Options state - settings menu */

static void OptionsState_Enter(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void OptionsState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_Ctx;
    (void)_DeltaTime;
}

static void OptionsState_Render(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void OptionsState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    Input_HandleOptionsInput(_Ctx->game, _Event, _Ctx->audio);
}

static void OptionsState_Exit(StateContext *_Ctx)
{
    (void)_Ctx;
}

State g_OptionsState = {
    .name = "Options",
    .enter = OptionsState_Enter,
    .update = OptionsState_Update,
    .render = OptionsState_Render,
    .handleInput = OptionsState_HandleInput,
    .exit = OptionsState_Exit
};
