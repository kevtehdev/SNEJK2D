#include "../../include/core/state_machine.h"
#include "../../include/input/input_gameplay.h"
#include "../../include/audio/audio.h"
#include <SDL2/SDL.h>

/* Playing state - singleplayer gameplay */

static void PlayingState_Enter(StateContext *_Ctx)
{
    // Start game music
    AudioSystem *audio = (AudioSystem*)_Ctx->audio;
    if (_Ctx->game->settings.gameMusicEnabled)
    {
        Audio_PlayGameMusic(audio);
    }
}

static void PlayingState_Update(StateContext *_Ctx, unsigned int _DeltaTime)
{
    (void)_DeltaTime;
    Game_Update(_Ctx->game);
}

static void PlayingState_Render(StateContext *_Ctx)
{
    (void)_Ctx;
}

static void PlayingState_HandleInput(StateContext *_Ctx, SDL_Event *_Event)
{
    Input_HandlePlayingInput(_Ctx->game, _Event, _Ctx->audio);
}

static void PlayingState_Exit(StateContext *_Ctx)
{
    (void)_Ctx;
}

State g_PlayingState = {
    .name = "Playing",
    .enter = PlayingState_Enter,
    .update = PlayingState_Update,
    .render = PlayingState_Render,
    .handleInput = PlayingState_HandleInput,
    .exit = PlayingState_Exit
};
