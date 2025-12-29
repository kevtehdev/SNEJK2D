#include "../../include/core/state_machine.h"
#include <stdio.h>
#include <string.h>

/* Initialize state machine with context */
void StateMachine_Init(StateMachine *_Sm, StateContext *_Ctx)
{
    memset(_Sm, 0, sizeof(StateMachine));
    _Sm->context = _Ctx;  // Store pointer, not copy!
    _Sm->currentState = NULL;
    _Sm->previousState = NULL;
    _Sm->stateChanged = false;
}

/* Change to a new state (handles enter/exit lifecycle) */
void StateMachine_ChangeState(StateMachine *_Sm, State *_NewState)
{
    if (_Sm->currentState == _NewState)
        return;

    // Exit current state
    if (_Sm->currentState && _Sm->currentState->exit)
    {
        printf("[StateMachine] Exiting state: %s\n", _Sm->currentState->name);
        _Sm->currentState->exit(_Sm->context);
    }

    // Update state pointers
    _Sm->previousState = _Sm->currentState;
    _Sm->currentState = _NewState;
    _Sm->stateChanged = true;

    // Enter new state
    if (_Sm->currentState && _Sm->currentState->enter)
    {
        printf("[StateMachine] Entering state: %s\n", _Sm->currentState->name);
        _Sm->currentState->enter(_Sm->context);
    }
}

/* Update current state */
void StateMachine_Update(StateMachine *_Sm, unsigned int _DeltaTime)
{
    if (_Sm->currentState && _Sm->currentState->update)
    {
        _Sm->currentState->update(_Sm->context, _DeltaTime);
    }
}

/* Render current state */
void StateMachine_Render(StateMachine *_Sm)
{
    if (_Sm->currentState && _Sm->currentState->render)
    {
        _Sm->currentState->render(_Sm->context);
    }
}

/* Handle input for current state */
void StateMachine_HandleInput(StateMachine *_Sm, SDL_Event *_Event)
{
    if (_Sm->currentState && _Sm->currentState->handleInput)
    {
        _Sm->currentState->handleInput(_Sm->context, _Event);
    }
}

/* Get current state */
State* StateMachine_GetCurrentState(StateMachine *_Sm)
{
    return _Sm->currentState;
}

/* Shutdown state machine */
void StateMachine_Shutdown(StateMachine *_Sm)
{
    // Exit current state if any
    if (_Sm->currentState && _Sm->currentState->exit)
    {
        _Sm->currentState->exit(_Sm->context);
    }

    _Sm->currentState = NULL;
    _Sm->previousState = NULL;
}

/* Forward declarations of all state instances (defined in individual state files) */
extern State g_LoadingState;
extern State g_MainMenuState;
extern State g_ModeSelectState;
extern State g_NameInputState;
extern State g_MenuState;
extern State g_PlayingState;
extern State g_PausedState;
extern State g_GameOverState;
extern State g_ScoreboardState;
extern State g_OptionsState;
extern State g_MultiplayerState;

/* Map GameState enum to actual state instances */
State* StateMachine_GetStateForEnum(int _StateEnum)
{
    switch (_StateEnum)
    {
        case GAME_LOADING:      return &g_LoadingState;
        case GAME_MAIN_MENU:    return &g_MainMenuState;
        case GAME_MODE_SELECT:  return &g_ModeSelectState;
        case GAME_NAME_INPUT:   return &g_NameInputState;
        case GAME_MENU:         return &g_MenuState;
        case GAME_PLAYING:      return &g_PlayingState;
        case GAME_PAUSED:       return &g_PausedState;
        case GAME_OVER:         return &g_GameOverState;
        case GAME_SCOREBOARD:   return &g_ScoreboardState;
        case GAME_OPTIONS:      return &g_OptionsState;
        case GAME_MULTIPLAYER:  return &g_MultiplayerState;
        default:
            fprintf(stderr, "[StateMachine] Unknown state enum: %d\n", _StateEnum);
            return NULL;
    }
}
