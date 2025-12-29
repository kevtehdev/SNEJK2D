#include "../../include/core/event_system.h"
#include <string.h>
#include <stdio.h>

/* Initialize event dispatcher */
void EventSystem_Init(EventDispatcher *_Dispatcher)
{
    memset(_Dispatcher, 0, sizeof(EventDispatcher));

    for (int i = 0; i < EVENT_TYPE_COUNT; i++)
    {
        _Dispatcher->listenerCount[i] = 0;
        for (int j = 0; j < MAX_EVENT_LISTENERS; j++)
        {
            _Dispatcher->listeners[i][j].active = false;
            _Dispatcher->listeners[i][j].callback = NULL;
            _Dispatcher->listeners[i][j].userData = NULL;
        }
    }

    _Dispatcher->queueHead = 0;
    _Dispatcher->queueTail = 0;
    _Dispatcher->queueSize = 0;
    _Dispatcher->eventsProcessed = 0;
    _Dispatcher->eventsQueued = 0;
}

/* Register an event listener */
bool EventSystem_Subscribe(EventDispatcher *_Dispatcher, EventType _Type, EventListener _Callback, void *_UserData)
{
    if (_Type >= EVENT_TYPE_COUNT || !_Callback)
    {
        fprintf(stderr, "[EventSystem] Invalid event type or callback\n");
        return false;
    }

    /* Find free slot */
    for (int i = 0; i < MAX_EVENT_LISTENERS; i++)
    {
        if (!_Dispatcher->listeners[_Type][i].active)
        {
            _Dispatcher->listeners[_Type][i].callback = _Callback;
            _Dispatcher->listeners[_Type][i].userData = _UserData;
            _Dispatcher->listeners[_Type][i].active = true;
            _Dispatcher->listenerCount[_Type]++;
            return true;
        }
    }

    fprintf(stderr, "[EventSystem] Too many listeners for event type %d\n", _Type);
    return false;
}

/* Unregister an event listener */
bool EventSystem_Unsubscribe(EventDispatcher *_Dispatcher, EventType _Type, EventListener _Callback)
{
    if (_Type >= EVENT_TYPE_COUNT)
        return false;

    for (int i = 0; i < MAX_EVENT_LISTENERS; i++)
    {
        if (_Dispatcher->listeners[_Type][i].active &&
            _Dispatcher->listeners[_Type][i].callback == _Callback)
        {
            _Dispatcher->listeners[_Type][i].active = false;
            _Dispatcher->listeners[_Type][i].callback = NULL;
            _Dispatcher->listeners[_Type][i].userData = NULL;
            _Dispatcher->listenerCount[_Type]--;
            return true;
        }
    }

    return false;
}

/* Emit an event immediately */
void EventSystem_Emit(EventDispatcher *_Dispatcher, const Event *_Event)
{
    if (!_Event || _Event->type >= EVENT_TYPE_COUNT)
        return;

    /* Call all registered listeners for this event type */
    for (int i = 0; i < MAX_EVENT_LISTENERS; i++)
    {
        if (_Dispatcher->listeners[_Event->type][i].active)
        {
            EventListenerEntry *listener = &_Dispatcher->listeners[_Event->type][i];
            listener->callback(_Event, listener->userData);
        }
    }

    _Dispatcher->eventsProcessed++;
}

/* Queue an event for later processing */
bool EventSystem_Queue(EventDispatcher *_Dispatcher, const Event *_Event)
{
    if (!_Event || _Event->type >= EVENT_TYPE_COUNT)
        return false;

    /* Check if queue is full */
    if (_Dispatcher->queueSize >= MAX_EVENT_QUEUE)
    {
        fprintf(stderr, "[EventSystem] Event queue full!\n");
        return false;
    }

    /* Add event to queue */
    _Dispatcher->eventQueue[_Dispatcher->queueTail] = *_Event;
    _Dispatcher->queueTail = (_Dispatcher->queueTail + 1) % MAX_EVENT_QUEUE;
    _Dispatcher->queueSize++;
    _Dispatcher->eventsQueued++;

    return true;
}

/* Process all queued events */
void EventSystem_ProcessQueue(EventDispatcher *_Dispatcher)
{
    while (_Dispatcher->queueSize > 0)
    {
        /* Get event from queue */
        Event event = _Dispatcher->eventQueue[_Dispatcher->queueHead];
        _Dispatcher->queueHead = (_Dispatcher->queueHead + 1) % MAX_EVENT_QUEUE;
        _Dispatcher->queueSize--;

        /* Emit the event */
        EventSystem_Emit(_Dispatcher, &event);
    }
}

/* Clear all event listeners */
void EventSystem_ClearListeners(EventDispatcher *_Dispatcher)
{
    for (int i = 0; i < EVENT_TYPE_COUNT; i++)
    {
        for (int j = 0; j < MAX_EVENT_LISTENERS; j++)
        {
            _Dispatcher->listeners[i][j].active = false;
            _Dispatcher->listeners[i][j].callback = NULL;
            _Dispatcher->listeners[i][j].userData = NULL;
        }
        _Dispatcher->listenerCount[i] = 0;
    }
}

/* Clear event queue */
void EventSystem_ClearQueue(EventDispatcher *_Dispatcher)
{
    _Dispatcher->queueHead = 0;
    _Dispatcher->queueTail = 0;
    _Dispatcher->queueSize = 0;
}

/* Cleanup event system */
void EventSystem_Shutdown(EventDispatcher *_Dispatcher)
{
    EventSystem_ClearListeners(_Dispatcher);
    EventSystem_ClearQueue(_Dispatcher);

    printf("[EventSystem] Shutdown - Events processed: %u, Events queued: %u\n",
           _Dispatcher->eventsProcessed, _Dispatcher->eventsQueued);
}

/* Helper functions to create common events */

Event Event_CreateSnakeDied(void)
{
    Event event;
    memset(&event, 0, sizeof(Event));
    event.type = EVENT_SNAKE_DIED;
    return event;
}

Event Event_CreateFoodEaten(void)
{
    Event event;
    memset(&event, 0, sizeof(Event));
    event.type = EVENT_FOOD_EATEN;
    return event;
}

Event Event_CreateScoreChanged(int _NewScore, int _OldScore)
{
    Event event;
    memset(&event, 0, sizeof(Event));
    event.type = EVENT_SCORE_CHANGED;
    event.data.scoreChanged.score = _NewScore;
    event.data.scoreChanged.previousScore = _OldScore;
    return event;
}

Event Event_CreateGameStateChanged(GameState _NewState, GameState _OldState)
{
    Event event;
    memset(&event, 0, sizeof(Event));
    event.type = EVENT_GAME_STATE_CHANGED;
    event.data.gameStateChanged.newState = _NewState;
    event.data.gameStateChanged.oldState = _OldState;
    return event;
}

Event Event_CreatePowerUpCollected(PowerUpType _Type, Position _Pos)
{
    Event event;
    memset(&event, 0, sizeof(Event));
    event.type = EVENT_POWERUP_COLLECTED;
    event.data.powerUp.type = _Type;
    event.data.powerUp.position = _Pos;
    return event;
}

Event Event_CreatePowerUpExpired(PowerUpType _Type)
{
    Event event;
    memset(&event, 0, sizeof(Event));
    event.type = EVENT_POWERUP_EXPIRED;
    event.data.powerUp.type = _Type;
    event.data.powerUp.position = (Position){0, 0}; // Not needed for expiration
    return event;
}

Event Event_CreatePowerUpSpawned(PowerUpType _Type, Position _Pos)
{
    Event event;
    memset(&event, 0, sizeof(Event));
    event.type = EVENT_POWERUP_SPAWNED;
    event.data.powerUp.type = _Type;
    event.data.powerUp.position = _Pos;
    return event;
}

Event Event_CreateLevelUp(int _NewLevel, int _XpRemaining, int _XpToNextLevel)
{
    Event event;
    memset(&event, 0, sizeof(Event));
    event.type = EVENT_LEVEL_UP;
    event.data.levelUp.newLevel = _NewLevel;
    event.data.levelUp.xpRemaining = _XpRemaining;
    event.data.levelUp.xpToNextLevel = _XpToNextLevel;
    return event;
}

Event Event_CreateExplosion(Position _Pos, unsigned int _Time)
{
    Event event;
    memset(&event, 0, sizeof(Event));
    event.type = EVENT_EXPLOSION_TRIGGERED;
    event.data.explosion.position = _Pos;
    event.data.explosion.currentTime = _Time;
    return event;
}
