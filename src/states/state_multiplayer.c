#include "../../include/core/state_machine.h"
#include "../../include/input/input_multiplayer.h"
#include "../../include/multiplayer/multiplayer.h"
#include <SDL2/SDL.h>
#include <stdio.h>

/* Multiplayer state - handles all multiplayer modes */

static void MultiplayerState_Enter(StateContext *_Ctx)
{
    (void)_Ctx;
    printf("[MultiplayerState] Entered multiplayer state\n");
}

static void MultiplayerState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_DeltaTime;

    MultiplayerContext *mpCtx = *_Ctx->mpCtx;
    if (!mpCtx)
    {
        printf("[MultiplayerState] WARNING: mpCtx is NULL!\n");
        return;
    }

    // Debug: Print state every 60 frames (roughly once per second)
    static int debugCounter = 0;
    if (++debugCounter >= 60)
    {
        printf("[MultiplayerState] Update - mpCtx->state=%d, isHost=%d\n", mpCtx->state, mpCtx->isHost);
        debugCounter = 0;
    }

    // Update host logic
    if (mpCtx->isHost && mpCtx->state == MP_STATE_PLAYING)
    {
        Multiplayer_HostUpdate(mpCtx, _Ctx->currentTime);
    }

    // Update turn battle local game
    if (mpCtx->state == MP_STATE_TURN_PLAYING)
    {
        printf("[MultiplayerState] Updating turn battle local game\n");
        Game_Update(&mpCtx->localGame);

        // Check if player died
        if (mpCtx->localGame.state == GAME_OVER)
        {
            printf("[MultiplayerState] Player died, finishing turn attempt\n");
            Multiplayer_FinishTurnAttempt(mpCtx);
        }
    }

    // Update countdown
    if (mpCtx->state == MP_STATE_COUNTDOWN)
    {
        static int countdownDebug = 0;
        if (++countdownDebug >= 60)
        {
            printf("[MultiplayerState] COUNTDOWN: currentTime=%u, gameStartTime=%u, diff=%d\n",
                   _Ctx->currentTime, mpCtx->gameStartTime, (int)(_Ctx->currentTime - mpCtx->gameStartTime));
            countdownDebug = 0;
        }
    }
    Multiplayer_UpdateCountdown(mpCtx, _Ctx->currentTime);
}

static void MultiplayerState_Render(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void MultiplayerState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    MultiplayerContext *mpCtx = *_Ctx->mpCtx;
    if (!mpCtx)
        return;

    // Handle ESC to return to main menu from multiplayer menu
    if (mpCtx->state == MP_STATE_MENU && _Event->type == SDL_KEYDOWN &&
        _Event->key.keysym.sym == SDLK_ESCAPE)
    {
        _Ctx->game->state = GAME_MAIN_MENU;
        return;
    }

    // Handle ESC to cancel hosting and return to main menu
    if ((mpCtx->state == MP_STATE_HOSTING || mpCtx->state == MP_STATE_LOBBY) &&
        mpCtx->isHost && _Event->type == SDL_KEYDOWN &&
        _Event->key.keysym.sym == SDLK_ESCAPE)
    {
        // Clean up multiplayer and return to main menu
        if (mpCtx->api)
        {
            Multiplayer_Destroy(*_Ctx->mpCtx);
            *_Ctx->mpCtx = NULL;
        }
        _Ctx->game->state = GAME_MAIN_MENU;
        return;
    }

    // Handle regular multiplayer input
    Input_HandleMultiplayerInput(mpCtx, _Event, _Ctx->game, _Ctx->audio);

    // Check if player wants to disconnect and clean up
    if (mpCtx->state == MP_STATE_DISCONNECTED)
    {
        Multiplayer_Destroy(*_Ctx->mpCtx);
        *_Ctx->mpCtx = NULL;
        _Ctx->game->state = GAME_MAIN_MENU;
    }
}

static void MultiplayerState_Exit(StateContext *_Ctx)
{
    (void)_Ctx;
}

State g_MultiplayerState = {
    .name = "Multiplayer",
    .enter = MultiplayerState_Enter,
    .update = MultiplayerState_Update,
    .render = MultiplayerState_Render,
    .handleInput = MultiplayerState_HandleInput,
    .exit = MultiplayerState_Exit
};
