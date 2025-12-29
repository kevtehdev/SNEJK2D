#include "../../include/core/state_machine.h"
#include "../../include/input/input_menu.h"

/* Name Input state - player enters their name */

static void NameInputState_Enter(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void NameInputState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_Ctx;
    (void)_DeltaTime;
}

static void NameInputState_Render(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void NameInputState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    Input_HandleNameInput(_Ctx->game, _Event);
}

static void NameInputState_Exit(StateContext *_Ctx)
{
    (void)_Ctx;
}

State g_NameInputState = {
    .name = "NameInput",
    .enter = NameInputState_Enter,
    .update = NameInputState_Update,
    .render = NameInputState_Render,
    .handleInput = NameInputState_HandleInput,
    .exit = NameInputState_Exit
};
