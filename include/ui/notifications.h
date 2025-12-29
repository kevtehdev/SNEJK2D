#ifndef __NOTIFICATIONS_H_
#define __NOTIFICATIONS_H_

#include "../core/event_system.h"
#include <stdbool.h>

#define MAX_NOTIFICATIONS 5
#define NOTIFICATION_DURATION 2000 // ms

typedef enum {
    NOTIF_TYPE_INFO,
    NOTIF_TYPE_SUCCESS,
    NOTIF_TYPE_WARNING,
    NOTIF_TYPE_POWERUP,
    NOTIF_TYPE_LEVELUP
} NotificationType;

typedef struct {
    char message[128];
    NotificationType type;
    unsigned int startTime;
    bool active;
} Notification;

typedef struct {
    Notification notifications[MAX_NOTIFICATIONS];
    int count;
} NotificationSystem;

/* Initialize notification system */
void Notifications_Init(NotificationSystem *_NotifSys);

/* Subscribe to game events */
void Notifications_SubscribeToEvents(NotificationSystem *_NotifSys, EventDispatcher *_Dispatcher);

/* Add a notification manually */
void Notifications_Add(NotificationSystem *_NotifSys, const char *_Message, NotificationType _Type, unsigned int _CurrentTime);

/* Update notification timers and remove expired ones */
void Notifications_Update(NotificationSystem *_NotifSys, unsigned int _CurrentTime);

/* Get active notifications for rendering */
const Notification* Notifications_GetActive(NotificationSystem *_NotifSys, int *_OutCount);

/* Clear all notifications */
void Notifications_Clear(NotificationSystem *_NotifSys);

#endif
