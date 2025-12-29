#include "../../include/ui/notifications.h"
#include "../../include/entities/snake.h"
#include <stdio.h>
#include <string.h>

void Notifications_Init(NotificationSystem *_NotifSys)
{
    memset(_NotifSys, 0, sizeof(NotificationSystem));
    _NotifSys->count = 0;
}

/* Event listener callbacks */

static void OnPowerUpCollected(const Event *_Event, void *_UserData)
{
    NotificationSystem *notifSys = (NotificationSystem*)_UserData;

    const char *powerUpName = "Power-Up";
    switch (_Event->data.powerUp.type)
    {
        case POWERUP_GOLDEN_APPLE:
            powerUpName = "Golden Apple";
            break;
        case POWERUP_SCORE_MULTIPLIER:
            powerUpName = "Score Multiplier";
            break;
        case POWERUP_SPEED_BOOST:
            powerUpName = "Speed Boost";
            break;
        default:
            break;
    }

    char message[128];
    snprintf(message, sizeof(message), "%s collected!", powerUpName);

    // Find an inactive slot
    for (int i = 0; i < MAX_NOTIFICATIONS; i++)
    {
        if (!notifSys->notifications[i].active)
        {
            strncpy(notifSys->notifications[i].message, message, sizeof(notifSys->notifications[i].message) - 1);
            notifSys->notifications[i].message[sizeof(notifSys->notifications[i].message) - 1] = '\0';
            notifSys->notifications[i].type = NOTIF_TYPE_POWERUP;
            notifSys->notifications[i].startTime = 0; // Will be set by caller
            notifSys->notifications[i].active = true;
            if (notifSys->count < MAX_NOTIFICATIONS)
                notifSys->count++;
            break;
        }
    }
}

static void OnPowerUpExpired(const Event *_Event, void *_UserData)
{
    NotificationSystem *notifSys = (NotificationSystem*)_UserData;

    const char *powerUpName = "Power-Up";
    switch (_Event->data.powerUp.type)
    {
        case POWERUP_SCORE_MULTIPLIER:
            powerUpName = "Score Multiplier";
            break;
        case POWERUP_SPEED_BOOST:
            powerUpName = "Speed Boost";
            break;
        default:
            break;
    }

    char message[128];
    snprintf(message, sizeof(message), "%s expired", powerUpName);

    // Find an inactive slot
    for (int i = 0; i < MAX_NOTIFICATIONS; i++)
    {
        if (!notifSys->notifications[i].active)
        {
            strncpy(notifSys->notifications[i].message, message, sizeof(notifSys->notifications[i].message) - 1);
            notifSys->notifications[i].message[sizeof(notifSys->notifications[i].message) - 1] = '\0';
            notifSys->notifications[i].type = NOTIF_TYPE_WARNING;
            notifSys->notifications[i].startTime = 0;
            notifSys->notifications[i].active = true;
            if (notifSys->count < MAX_NOTIFICATIONS)
                notifSys->count++;
            break;
        }
    }
}

static void OnLevelUp(const Event *_Event, void *_UserData)
{
    NotificationSystem *notifSys = (NotificationSystem*)_UserData;

    char message[128];
    snprintf(message, sizeof(message), "Level Up! Level %d", _Event->data.levelUp.newLevel);

    // Find an inactive slot
    for (int i = 0; i < MAX_NOTIFICATIONS; i++)
    {
        if (!notifSys->notifications[i].active)
        {
            strncpy(notifSys->notifications[i].message, message, sizeof(notifSys->notifications[i].message) - 1);
            notifSys->notifications[i].message[sizeof(notifSys->notifications[i].message) - 1] = '\0';
            notifSys->notifications[i].type = NOTIF_TYPE_LEVELUP;
            notifSys->notifications[i].startTime = 0;
            notifSys->notifications[i].active = true;
            if (notifSys->count < MAX_NOTIFICATIONS)
                notifSys->count++;
            break;
        }
    }
}

void Notifications_SubscribeToEvents(NotificationSystem *_NotifSys, EventDispatcher *_Dispatcher)
{
    if (!_Dispatcher || !_NotifSys)
        return;

    // Subscribe to power-up events
    EventSystem_Subscribe(_Dispatcher, EVENT_POWERUP_COLLECTED, OnPowerUpCollected, _NotifSys);
    EventSystem_Subscribe(_Dispatcher, EVENT_POWERUP_EXPIRED, OnPowerUpExpired, _NotifSys);

    // Subscribe to level-up event
    EventSystem_Subscribe(_Dispatcher, EVENT_LEVEL_UP, OnLevelUp, _NotifSys);

    printf("[Notifications] Subscribed to game events\n");
}

void Notifications_Add(NotificationSystem *_NotifSys, const char *_Message, NotificationType _Type, unsigned int _CurrentTime)
{
    // Find an inactive slot or replace oldest
    int slot = -1;
    unsigned int oldestTime = _CurrentTime;

    for (int i = 0; i < MAX_NOTIFICATIONS; i++)
    {
        if (!_NotifSys->notifications[i].active)
        {
            slot = i;
            break;
        }

        // Track oldest notification
        if (_NotifSys->notifications[i].startTime < oldestTime)
        {
            oldestTime = _NotifSys->notifications[i].startTime;
            slot = i;
        }
    }

    if (slot >= 0)
    {
        strncpy(_NotifSys->notifications[slot].message, _Message, sizeof(_NotifSys->notifications[slot].message) - 1);
        _NotifSys->notifications[slot].message[sizeof(_NotifSys->notifications[slot].message) - 1] = '\0';
        _NotifSys->notifications[slot].type = _Type;
        _NotifSys->notifications[slot].startTime = _CurrentTime;
        _NotifSys->notifications[slot].active = true;

        // Update count
        int activeCount = 0;
        for (int i = 0; i < MAX_NOTIFICATIONS; i++)
        {
            if (_NotifSys->notifications[i].active)
                activeCount++;
        }
        _NotifSys->count = activeCount;
    }
}

void Notifications_Update(NotificationSystem *_NotifSys, unsigned int _CurrentTime)
{
    for (int i = 0; i < MAX_NOTIFICATIONS; i++)
    {
        if (!_NotifSys->notifications[i].active)
            continue;

        // Set start time if not set (from event listeners)
        if (_NotifSys->notifications[i].startTime == 0)
        {
            _NotifSys->notifications[i].startTime = _CurrentTime;
        }

        // Check if expired
        if (_CurrentTime - _NotifSys->notifications[i].startTime > NOTIFICATION_DURATION)
        {
            _NotifSys->notifications[i].active = false;
            _NotifSys->count--;
        }
    }
}

const Notification* Notifications_GetActive(NotificationSystem *_NotifSys, int *_OutCount)
{
    *_OutCount = _NotifSys->count;
    return _NotifSys->notifications;
}

void Notifications_Clear(NotificationSystem *_NotifSys)
{
    for (int i = 0; i < MAX_NOTIFICATIONS; i++)
    {
        _NotifSys->notifications[i].active = false;
    }
    _NotifSys->count = 0;
}
