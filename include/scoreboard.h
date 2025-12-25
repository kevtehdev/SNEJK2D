#ifndef __SCOREBOARD_H_
#define __SCOREBOARD_H_

#include <stdbool.h>

#define MAX_SCOREBOARD_ENTRIES 4
#define MEDAL_FRAMES 13

typedef struct {
    int score;
    char name[32];
    unsigned int timestamp;  // Unix timestamp
} ScoreEntry;

typedef struct {
    ScoreEntry entries[MAX_SCOREBOARD_ENTRIES];
    int count;
} Scoreboard;

// All the scoreboard functions for SNEJK2D
void Scoreboard_Init(Scoreboard *_Scoreboard);
bool Scoreboard_Load(Scoreboard *_Scoreboard);
bool Scoreboard_Save(Scoreboard *_Scoreboard);
bool Scoreboard_AddScore(Scoreboard *_Scoreboard, int _Score, const char *_Name);
int Scoreboard_GetRank(Scoreboard *_Scoreboard, int _Score);  // Returns 1-based rank, 0 if not in top 4

#endif
