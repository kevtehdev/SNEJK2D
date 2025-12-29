#include "../../include/core/state_machine.h"
#include "../../include/input/input_menu.h"

/* Menu state - background selection */

static void MenuState_Enter(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void MenuState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_Ctx;
    (void)_DeltaTime;
}

static void MenuState_Render(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void MenuState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    Input_HandleGameMenuInput(_Ctx->game, _Event);
}

static void MenuState_Exit(StateContext *_Ctx)
{
    (void)_Ctx;
}

State g_MenuState = {
    .name = "Menu",
    .enter = MenuState_Enter,
    .update = MenuState_Update,
    .render = MenuState_Render,
    .handleInput = MenuState_HandleInput,
    .exit = MenuState_Exit
};
