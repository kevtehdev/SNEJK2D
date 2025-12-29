#ifndef __MP_MESSAGE_QUEUE_H_
#define __MP_MESSAGE_QUEUE_H_

#include "../../src/mpapi/c_client/libs/jansson/jansson.h"
#include <stdbool.h>
#include <stdint.h>

/* Message queue system for multiplayer networking */
/* Buffers incoming messages for processing in order */

#define MP_MSG_QUEUE_SIZE 64

typedef enum {
    MP_MSG_TYPE_STATE_UPDATE,
    MP_MSG_TYPE_INPUT,
    MP_MSG_TYPE_PLAYER_JOIN,
    MP_MSG_TYPE_PLAYER_LEAVE,
    MP_MSG_TYPE_GAME_START,
    MP_MSG_TYPE_GAME_END,
    MP_MSG_TYPE_CHAT,
    MP_MSG_TYPE_CUSTOM
} MP_MessageType;

typedef struct {
    MP_MessageType type;
    int64_t messageId;
    char clientId[64];
    json_t *data;  // Owned by message queue, will be freed
    unsigned int timestamp;
} MP_Message;

typedef struct {
    MP_Message messages[MP_MSG_QUEUE_SIZE];
    int head;  // Next message to read
    int tail;  // Next position to write
    int count; // Number of messages in queue
    unsigned int messagesProcessed;
    unsigned int messagesDropped;
} MP_MessageQueue;

/* Initialize message queue */
void MP_MsgQueue_Init(MP_MessageQueue *_Queue);

/* Cleanup message queue */
void MP_MsgQueue_Cleanup(MP_MessageQueue *_Queue);

/* Enqueue a message (copies data) */
bool MP_MsgQueue_Push(MP_MessageQueue *_Queue, MP_MessageType _Type,
                      int64_t _MessageId, const char *_ClientId, json_t *_Data);

/* Dequeue a message (caller must free data with json_decref) */
bool MP_MsgQueue_Pop(MP_MessageQueue *_Queue, MP_Message *_OutMsg);

/* Peek at next message without removing it */
bool MP_MsgQueue_Peek(MP_MessageQueue *_Queue, MP_Message *_OutMsg);

/* Check if queue is empty */
bool MP_MsgQueue_IsEmpty(MP_MessageQueue *_Queue);

/* Check if queue is full */
bool MP_MsgQueue_IsFull(MP_MessageQueue *_Queue);

/* Get number of messages in queue */
int MP_MsgQueue_GetCount(MP_MessageQueue *_Queue);

/* Clear all messages */
void MP_MsgQueue_Clear(MP_MessageQueue *_Queue);

#endif
