#ifndef __STATE_MACHINE_H_
#define __STATE_MACHINE_H_

#include "game.h"
#include "../multiplayer/multiplayer.h"
#include "../utils/scoreboard.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

/* Forward declaration */
typedef struct State State;
typedef struct StateMachine StateMachine;

/* Context structure holding all game systems */
typedef struct {
    Game *game;
    void *renderer;           // Renderer*
    void *audio;              // AudioSystem*
    MultiplayerContext **mpCtx;
    Scoreboard *scoreboard;
    unsigned int currentTime;
    int *mainMenuSelection;   // Pointer to g_main_menu_selection
    void *debugDraw;          // DebugDrawConfig*
    void *logger;             // Logger*
} StateContext;

/* State interface - each state implements these callbacks */
struct State {
    const char *name;         // For debugging

    /* Called when entering this state */
    void (*enter)(StateContext *_Ctx);

    /* Called every frame to update state logic */
    void (*update)(StateContext *_Ctx, unsigned int _DeltaTime);

    /* Called every frame to render state */
    void (*render)(StateContext *_Ctx);

    /* Called to handle input events */
    void (*handleInput)(StateContext *_Ctx, SDL_Event *_Event);

    /* Called when exiting this state */
    void (*exit)(StateContext *_Ctx);
};

/* State machine manages state transitions */
struct StateMachine {
    State *currentState;
    State *previousState;
    StateContext *context;  // Pointer to context (not a copy!)
    bool stateChanged;
};

/* State machine API */
void StateMachine_Init(StateMachine *_Sm, StateContext *_Ctx);
void StateMachine_ChangeState(StateMachine *_Sm, State *_NewState);
void StateMachine_Update(StateMachine *_Sm, unsigned int _DeltaTime);
void StateMachine_Render(StateMachine *_Sm);
void StateMachine_HandleInput(StateMachine *_Sm, SDL_Event *_Event);
State* StateMachine_GetCurrentState(StateMachine *_Sm);
void StateMachine_Shutdown(StateMachine *_Sm);

/* Get state by GameState enum value */
State* StateMachine_GetStateForEnum(int _StateEnum);

#endif
