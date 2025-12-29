#include "../../include/core/state_machine.h"
#include "../../include/input/input_menu.h"

/* Mode Select state - Classic vs Power-Up mode selection */

static void ModeSelectState_Enter(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void ModeSelectState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_Ctx;
    (void)_DeltaTime;
}

static void ModeSelectState_Render(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void ModeSelectState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    Input_HandleModeSelectInput(_Ctx->game, _Event);
}

static void ModeSelectState_Exit(StateContext *_Ctx)
{
    (void)_Ctx;
}

State g_ModeSelectState = {
    .name = "ModeSelect",
    .enter = ModeSelectState_Enter,
    .update = ModeSelectState_Update,
    .render = ModeSelectState_Render,
    .handleInput = ModeSelectState_HandleInput,
    .exit = ModeSelectState_Exit
};
