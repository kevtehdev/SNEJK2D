#ifndef __SNAKE_H_
#define __SNAKE_H_

#include "../config.h"
#include <stdbool.h>

typedef enum
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct
{
    int x;
    int y;
} Position;

/* Input buffer for smoother controls (allows buffering 2 inputs) */
typedef struct
{
    Direction buffer[2];
    int count;
} InputBuffer;

typedef struct
{
    Position segments[GRID_WIDTH * GRID_HEIGHT]; /* Max possible length */
    int length;
    Direction direction;
    Direction nextDirection; /* Queued direction for next move */
    bool alive;
    int score;
    InputBuffer inputBuffer;
} Snake;

/* Initializes a new snake at the center of the playable area */
void Snake_Init(Snake *_Snake);

/* Moves the snake one step in its current direction */
void Snake_Move(Snake *_Snake);

/* Grows the snake by one segment */
void Snake_Grow(Snake *_Snake);

/* Sets the snake's direction (queued for next move to prevent instant reverse) */
void Snake_SetDirection(Snake *_Snake, Direction _Dir);

/* Checks if the snake has collided with itself */
bool Snake_CheckSelfCollision(Snake *_Snake);

/* Checks if the snake has collided with the walls */
bool Snake_CheckWallCollision(Snake *_Snake);

#endif //__SNAKE_H_
