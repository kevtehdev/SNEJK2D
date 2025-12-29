#include "../../include/multiplayer/mp_message_queue.h"
#include <string.h>
#include <stdio.h>
#include <SDL2/SDL.h>

void MP_MsgQueue_Init(MP_MessageQueue *_Queue)
{
    if (!_Queue)
        return;

    memset(_Queue, 0, sizeof(MP_MessageQueue));
    _Queue->head = 0;
    _Queue->tail = 0;
    _Queue->count = 0;
    _Queue->messagesProcessed = 0;
    _Queue->messagesDropped = 0;
}

void MP_MsgQueue_Cleanup(MP_MessageQueue *_Queue)
{
    if (!_Queue)
        return;

    // Free all pending messages
    MP_MsgQueue_Clear(_Queue);
}

bool MP_MsgQueue_Push(MP_MessageQueue *_Queue, MP_MessageType _Type,
                      int64_t _MessageId, const char *_ClientId, json_t *_Data)
{
    if (!_Queue)
        return false;

    // Check if queue is full
    if (_Queue->count >= MP_MSG_QUEUE_SIZE)
    {
        _Queue->messagesDropped++;
        printf("[MsgQueue] WARNING: Queue full, dropping message (type=%d)\n", _Type);
        if (_Data)
            json_decref(_Data);
        return false;
    }

    // Add message at tail
    MP_Message *msg = &_Queue->messages[_Queue->tail];
    msg->type = _Type;
    msg->messageId = _MessageId;
    if (_ClientId)
    {
        strncpy(msg->clientId, _ClientId, sizeof(msg->clientId) - 1);
        msg->clientId[sizeof(msg->clientId) - 1] = '\0';
    }
    else
    {
        msg->clientId[0] = '\0';
    }

    // Increment reference count if data exists
    msg->data = _Data ? json_incref(_Data) : NULL;
    msg->timestamp = SDL_GetTicks();

    // Update tail and count
    _Queue->tail = (_Queue->tail + 1) % MP_MSG_QUEUE_SIZE;
    _Queue->count++;

    return true;
}

bool MP_MsgQueue_Pop(MP_MessageQueue *_Queue, MP_Message *_OutMsg)
{
    if (!_Queue || !_OutMsg)
        return false;

    // Check if queue is empty
    if (_Queue->count == 0)
        return false;

    // Copy message from head
    *_OutMsg = _Queue->messages[_Queue->head];

    // Clear the slot (but don't free data - caller owns it now)
    memset(&_Queue->messages[_Queue->head], 0, sizeof(MP_Message));

    // Update head and count
    _Queue->head = (_Queue->head + 1) % MP_MSG_QUEUE_SIZE;
    _Queue->count--;
    _Queue->messagesProcessed++;

    return true;
}

bool MP_MsgQueue_Peek(MP_MessageQueue *_Queue, MP_Message *_OutMsg)
{
    if (!_Queue || !_OutMsg)
        return false;

    // Check if queue is empty
    if (_Queue->count == 0)
        return false;

    // Copy message from head without removing
    *_OutMsg = _Queue->messages[_Queue->head];

    return true;
}

bool MP_MsgQueue_IsEmpty(MP_MessageQueue *_Queue)
{
    return _Queue ? (_Queue->count == 0) : true;
}

bool MP_MsgQueue_IsFull(MP_MessageQueue *_Queue)
{
    return _Queue ? (_Queue->count >= MP_MSG_QUEUE_SIZE) : false;
}

int MP_MsgQueue_GetCount(MP_MessageQueue *_Queue)
{
    return _Queue ? _Queue->count : 0;
}

void MP_MsgQueue_Clear(MP_MessageQueue *_Queue)
{
    if (!_Queue)
        return;

    // Free all pending message data
    for (int i = 0; i < MP_MSG_QUEUE_SIZE; i++)
    {
        if (_Queue->messages[i].data)
        {
            json_decref(_Queue->messages[i].data);
            _Queue->messages[i].data = NULL;
        }
    }

    _Queue->head = 0;
    _Queue->tail = 0;
    _Queue->count = 0;
}
