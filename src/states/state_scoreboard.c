#include "../../include/core/state_machine.h"
#include "../../include/input/input_menu.h"

/* Scoreboard state - display high scores */

static void ScoreboardState_Enter(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void ScoreboardState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_Ctx;
    (void)_DeltaTime;
}

static void ScoreboardState_Render(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void ScoreboardState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    Input_HandleScoreboardInput(_Ctx->game, _Event);
}

static void ScoreboardState_Exit(StateContext *_Ctx)
{
    (void)_Ctx;
}

State g_ScoreboardState = {
    .name = "Scoreboard",
    .enter = ScoreboardState_Enter,
    .update = ScoreboardState_Update,
    .render = ScoreboardState_Render,
    .handleInput = ScoreboardState_HandleInput,
    .exit = ScoreboardState_Exit
};
