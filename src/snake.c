#include "../include/snake.h"
#include <stdlib.h>

void Snake_Init(Snake *_Snake)
{
    // Start in center of grid
    int startX = GRID_WIDTH / 2;
    int startY = GRID_HEIGHT / 2;

    _Snake->length = INITIAL_SNAKE_LENGTH;
    _Snake->direction = DIR_RIGHT;
    _Snake->nextDirection = DIR_RIGHT;
    _Snake->alive = true;
    _Snake->score = 0;

    // Initialize input buffer
    _Snake->inputBuffer.count = 0;

    // Initialize segments (head to tail)
    for (int i = 0; i < _Snake->length; i++)
    {
        _Snake->segments[i].x = startX - i;
        _Snake->segments[i].y = startY;
    }
}

void Snake_SetDirection(Snake *_Snake, Direction _Dir)
{
    // Check what direction to validate against (last buffered or current)
    Direction lastDir = _Snake->direction;
    if (_Snake->inputBuffer.count > 0)
    {
        lastDir = _Snake->inputBuffer.buffer[_Snake->inputBuffer.count - 1];
    }

    // Prevent 180-degree turns
    if ((lastDir == DIR_UP && _Dir == DIR_DOWN) ||
        (lastDir == DIR_DOWN && _Dir == DIR_UP) ||
        (lastDir == DIR_LEFT && _Dir == DIR_RIGHT) ||
        (lastDir == DIR_RIGHT && _Dir == DIR_LEFT))
    {
        return;
    }

    // Don't buffer duplicate directions
    if (_Dir == lastDir)
    {
        return;
    }

    // Add to buffer if space available
    if (_Snake->inputBuffer.count < 2)
    {
        _Snake->inputBuffer.buffer[_Snake->inputBuffer.count] = _Dir;
        _Snake->inputBuffer.count++;
    }
}

void Snake_Move(Snake *_Snake)
{
    if (!_Snake->alive)
        return;

    // Consume from input buffer if available
    if (_Snake->inputBuffer.count > 0)
    {
        _Snake->direction = _Snake->inputBuffer.buffer[0];

        // Shift buffer
        for (int i = 0; i < _Snake->inputBuffer.count - 1; i++)
        {
            _Snake->inputBuffer.buffer[i] = _Snake->inputBuffer.buffer[i + 1];
        }
        _Snake->inputBuffer.count--;
    }

    // Calculate new head position
    Position newHead = _Snake->segments[0];

    switch (_Snake->direction)
    {
    case DIR_UP:
        newHead.y--;
        break;
    case DIR_DOWN:
        newHead.y++;
        break;
    case DIR_LEFT:
        newHead.x--;
        break;
    case DIR_RIGHT:
        newHead.x++;
        break;
    }

    // Move body (shift all segments)
    for (int i = _Snake->length - 1; i > 0; i--)
    {
        _Snake->segments[i] = _Snake->segments[i - 1];
    }

    // Place new head
    _Snake->segments[0] = newHead;
}

void Snake_Grow(Snake *_Snake)
{
    if (_Snake->length >= GRID_WIDTH * GRID_HEIGHT - 1)
        return;  // Max length reached

    // Duplicate last segment (will be overwritten on next move)
    _Snake->segments[_Snake->length] = _Snake->segments[_Snake->length - 1];
    _Snake->length++;
    _Snake->score++;
}

bool Snake_CheckWallCollision(Snake *_Snake)
{
    Position head = _Snake->segments[0];
    // Check collision with HUD border (which acts as walls)
    return (head.x < PLAYABLE_MIN_X || head.x > PLAYABLE_MAX_X ||
            head.y < PLAYABLE_MIN_Y || head.y > PLAYABLE_MAX_Y);
}

bool Snake_CheckSelfCollision(Snake *_Snake)
{
    Position head = _Snake->segments[0];

    // Check if head collides with any body segment
    for (int i = 1; i < _Snake->length; i++)
    {
        if (head.x == _Snake->segments[i].x &&
            head.y == _Snake->segments[i].y)
        {
            return true;
        }
    }

    return false;
}
