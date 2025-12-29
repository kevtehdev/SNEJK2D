#include "../../include/core/state_machine.h"
#include "../../include/input/input_menu.h"
#include "../../include/utils/scoreboard.h"
#include <string.h>

/* Game Over state - display score and handle high score */

static void GameOverState_Enter(StateContext *_Ctx)
{
    // Trigger explosion
    if (!_Ctx->game->explosion.active)
    {
        Game_TriggerExplosion(_Ctx->game, _Ctx->game->snake.segments[0], _Ctx->currentTime);
    }

    // Save score (only once) and check if highscore
    if (!_Ctx->game->scoreSaved && _Ctx->game->snake.score > 0)
    {
        Scoreboard_AddScore(_Ctx->scoreboard, _Ctx->game->snake.score, _Ctx->game->playerName);

        // Check if this is a highscore (top 4)
        _Ctx->game->isHighscore = false;
        for (int i = 0; i < 4 && i < _Ctx->scoreboard->count; i++)
        {
            if (_Ctx->scoreboard->entries[i].score == _Ctx->game->snake.score &&
                strcmp(_Ctx->scoreboard->entries[i].name, _Ctx->game->playerName) == 0)
            {
                _Ctx->game->isHighscore = true;
                break;
            }
        }

        Scoreboard_Save(_Ctx->scoreboard);
        _Ctx->game->scoreSaved = true;
    }
}

static void GameOverState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_DeltaTime;
    Game_UpdateExplosion(_Ctx->game, _Ctx->currentTime);
}

static void GameOverState_Render(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void GameOverState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    Input_HandleGameOverInput(_Ctx->game, _Event);
}

static void GameOverState_Exit(StateContext *_Ctx)
{
    (void)_Ctx;
}

State g_GameOverState = {
    .name = "GameOver",
    .enter = GameOverState_Enter,
    .update = GameOverState_Update,
    .render = GameOverState_Render,
    .handleInput = GameOverState_HandleInput,
    .exit = GameOverState_Exit
};
