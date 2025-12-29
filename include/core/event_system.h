#ifndef __EVENT_SYSTEM_H_
#define __EVENT_SYSTEM_H_

#include "game.h"
#include "../entities/snake.h"
#include <stdbool.h>

/* Maximum number of event listeners per event type */
#define MAX_EVENT_LISTENERS 16

/* Maximum number of queued events */
#define MAX_EVENT_QUEUE 64

/* Event types */
typedef enum {
    EVENT_NONE = 0,

    /* Game events */
    EVENT_SNAKE_DIED,
    EVENT_FOOD_EATEN,
    EVENT_SCORE_CHANGED,
    EVENT_GAME_STATE_CHANGED,
    EVENT_GAME_PAUSED,
    EVENT_GAME_RESUMED,

    /* Power-up events */
    EVENT_POWERUP_COLLECTED,
    EVENT_POWERUP_EXPIRED,
    EVENT_POWERUP_SPAWNED,
    EVENT_LEVEL_UP,

    /* Audio events */
    EVENT_MUSIC_CHANGE_REQUESTED,
    EVENT_SOUND_PLAY_REQUESTED,

    /* Visual events */
    EVENT_EXPLOSION_TRIGGERED,
    EVENT_BACKGROUND_CHANGED,

    /* Multiplayer events */
    EVENT_PLAYER_JOINED,
    EVENT_PLAYER_LEFT,
    EVENT_MULTIPLAYER_STATE_CHANGED,

    EVENT_TYPE_COUNT
} EventType;

/* Event data payloads for different event types */
typedef struct {
    int score;
    int previousScore;
} EventData_ScoreChanged;

typedef struct {
    GameState newState;
    GameState oldState;
} EventData_GameStateChanged;

typedef struct {
    PowerUpType type;
    Position position;
} EventData_PowerUp;

typedef struct {
    Position position;
    unsigned int currentTime;
} EventData_Explosion;

typedef struct {
    const char *musicType;  // "menu", "game", etc.
} EventData_MusicChange;

typedef struct {
    const char *soundName;
} EventData_SoundPlay;

typedef struct {
    int backgroundIndex;
} EventData_BackgroundChanged;

typedef struct {
    int newLevel;
    int xpRemaining;
    int xpToNextLevel;
} EventData_LevelUp;

/* Generic event structure */
typedef struct {
    EventType type;

    /* Event data (union for different event types) */
    union {
        EventData_ScoreChanged scoreChanged;
        EventData_GameStateChanged gameStateChanged;
        EventData_PowerUp powerUp;
        EventData_Explosion explosion;
        EventData_MusicChange musicChange;
        EventData_SoundPlay soundPlay;
        EventData_BackgroundChanged backgroundChanged;
        EventData_LevelUp levelUp;

        /* Generic pointer for custom data */
        void *customData;
    } data;
} Event;

/* Event listener callback function type */
typedef void (*EventListener)(const Event *_Event, void *_UserData);

/* Event listener registration */
typedef struct {
    EventListener callback;
    void *userData;
    bool active;
} EventListenerEntry;

/* Event dispatcher/manager */
typedef struct EventDispatcher {
    /* Event listeners for each event type */
    EventListenerEntry listeners[EVENT_TYPE_COUNT][MAX_EVENT_LISTENERS];
    int listenerCount[EVENT_TYPE_COUNT];

    /* Event queue for deferred processing */
    Event eventQueue[MAX_EVENT_QUEUE];
    int queueHead;
    int queueTail;
    int queueSize;

    /* Statistics */
    unsigned int eventsProcessed;
    unsigned int eventsQueued;
} EventDispatcher;

/* Event System API */

/* Initialize event dispatcher */
void EventSystem_Init(EventDispatcher *_Dispatcher);

/* Register an event listener for a specific event type */
bool EventSystem_Subscribe(EventDispatcher *_Dispatcher, EventType _Type, EventListener _Callback, void *_UserData);

/* Unregister an event listener */
bool EventSystem_Unsubscribe(EventDispatcher *_Dispatcher, EventType _Type, EventListener _Callback);

/* Emit an event immediately (synchronous) */
void EventSystem_Emit(EventDispatcher *_Dispatcher, const Event *_Event);

/* Queue an event for later processing (asynchronous) */
bool EventSystem_Queue(EventDispatcher *_Dispatcher, const Event *_Event);

/* Process all queued events */
void EventSystem_ProcessQueue(EventDispatcher *_Dispatcher);

/* Clear all event listeners */
void EventSystem_ClearListeners(EventDispatcher *_Dispatcher);

/* Clear event queue */
void EventSystem_ClearQueue(EventDispatcher *_Dispatcher);

/* Cleanup event system */
void EventSystem_Shutdown(EventDispatcher *_Dispatcher);

/* Helper functions to create common events */
Event Event_CreateSnakeDied(void);
Event Event_CreateFoodEaten(void);
Event Event_CreateScoreChanged(int _NewScore, int _OldScore);
Event Event_CreateGameStateChanged(GameState _NewState, GameState _OldState);
Event Event_CreatePowerUpCollected(PowerUpType _Type, Position _Pos);
Event Event_CreatePowerUpExpired(PowerUpType _Type);
Event Event_CreatePowerUpSpawned(PowerUpType _Type, Position _Pos);
Event Event_CreateLevelUp(int _NewLevel, int _XpRemaining, int _XpToNextLevel);
Event Event_CreateExplosion(Position _Pos, unsigned int _Time);

#endif
