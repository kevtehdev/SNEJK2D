#ifndef __MULTIPLAYER_H_
#define __MULTIPLAYER_H_

#include "config.h"
#include "snake.h"
#include "game.h"
#include "../src/mpapi/c_client/libs/mpapi.h"
#include "../src/mpapi/c_client/libs/jansson/jansson.h"
#include <stdbool.h>

#define MAX_MULTIPLAYER_PLAYERS 2  // 1v1 only
#define MAX_BROWSEABLE_GAMES 10

// Menu options
typedef enum {
    MP_MENU_HOST = 0,
    MP_MENU_JOIN = 1
} MultiplayerMenuOption;

// Game modes
typedef enum {
    MP_MODE_REALTIME = 0,    // Classic 1v1 realtime
    MP_MODE_TURN_BATTLE = 1  // Turn-based 3 attempts
} MultiplayerGameMode;

// Multiplayer states
typedef enum {
    MP_STATE_MENU,           // Choosing host/join
    MP_STATE_ENTERING_NAME,  // Entering room name for hosting
    MP_STATE_BROWSING,       // Browsing public games (join)
    MP_STATE_HOSTING,        // Creating session
    MP_STATE_JOINING,        // Joining session
    MP_STATE_LOBBY,          // Waiting for opponent
    MP_STATE_CHATTING,       // Chat mode in lobby (C key)
    MP_STATE_CHANGING_NICK,  // Changing nickname in lobby (N key)
    MP_STATE_MODE_SELECT,    // Selecting game mode (host only)
    MP_STATE_READY_UP,       // Players ready up for turn battle
    MP_STATE_COUNTDOWN,      // 3-2-1 countdown
    MP_STATE_PLAYING,        // Active game
    MP_STATE_TURN_PLAYING,   // Playing local turn (turn battle)
    MP_STATE_TURN_WAITING,   // Waiting for other players to finish
    MP_STATE_TURN_RESULTS,   // Showing turn battle results
    MP_STATE_GAME_OVER,      // Game ended
    MP_STATE_DISCONNECTED    // Connection lost
} MultiplayerState;

// Browseable game info
typedef struct {
    char sessionId[8];
    char name[64];
    int playerCount;
    int maxPlayers;
    int mapId;  // Background/map index
    MultiplayerGameMode gameMode;  // Game mode (REALTIME or TURN BATTLE)
} BrowseableGame;

// Turn battle attempt result
typedef struct {
    int score;
    int length;
    unsigned int survivalTime;
} TurnAttempt;

// Player in multiplayer
typedef struct {
    Snake snake;
    bool joined;
    bool alive;
    bool ready;
    int score;
    char clientId[64];
    char name[32];
    bool isLocal;

    // Combo system
    int comboCount;
    unsigned int lastFoodTime;
    float comboMultiplier;

    // Turn battle data
    TurnAttempt attempts[3];
    int completedAttempts;
    bool turnFinished;
    int bestScore;
} MultiplayerPlayer;

// Multiplayer game context
typedef struct {
    // mpapi
    mpapi *api;
    int listenerId;

    // Game state
    MultiplayerState state;
    MultiplayerPlayer players[MAX_MULTIPLAYER_PLAYERS];
    Position food[2];  // Two food items for more intense gameplay

    // Session info
    bool isHost;
    int localPlayerIndex;
    char sessionId[8];
    char ourClientId[64];

    // Network
    Direction pendingInput;
    bool hasPendingInput;

    // Timing
    unsigned int lastMoveTime;
    unsigned int currentSpeed;
    unsigned int gameStartTime;
    unsigned int countdownStart;

    // UI
    int selectedBackground;
    char errorMessage[256];
    int menuSelection;
    char sessionInput[8];
    int sessionInputLen;
    char roomName[64];
    int roomNameLen;
    int gameOverSelection;  // 0 = Try Again, 1 = Main Menu

    // Browsing
    BrowseableGame browsedGames[MAX_BROWSEABLE_GAMES];
    int browsedGameCount;
    int selectedGameIndex;

    // Chat system
    char chatMessages[10][128];  // Last 10 chat messages & max length of 128 chars each.
    int chatCount;
    char chatInput[128];
    int chatInputLen;
    MultiplayerState previousState;  // State to return to after chat/nick change

    // Nick change
    char nickInput[32];
    int nickInputLen;

    // Turn battle mode
    MultiplayerGameMode gameMode;
    int modeSelection;           // 0 = Realtime, 1 = Turn Battle
    int currentAttempt;          // Current attempt number (0-2)
    unsigned int attemptStartTime;
    Game localGame;              // Local game instance for turn battle
} MultiplayerContext;

// Lifecycle
MultiplayerContext* Multiplayer_Create(void);
void Multiplayer_Destroy(MultiplayerContext *_Ctx);

// Host operations
int Multiplayer_Host(MultiplayerContext *_Ctx, const char *_PlayerName);
void Multiplayer_HostUpdate(MultiplayerContext *_Ctx, unsigned int _CurrentTime);
void Multiplayer_HostBroadcastState(MultiplayerContext *_Ctx);

// Client operations
int Multiplayer_Join(MultiplayerContext *_Ctx, const char *_SessionId, const char *_PlayerName);
void Multiplayer_ClientSendInput(MultiplayerContext *_Ctx, Direction _Dir);
void Multiplayer_SendQuickPositionUpdate(MultiplayerContext *_Ctx);
int Multiplayer_BrowseGames(MultiplayerContext *_Ctx);

// Common operations
void Multiplayer_StartGame(MultiplayerContext *_Ctx);
void Multiplayer_RestartGame(MultiplayerContext *_Ctx);
void Multiplayer_ToggleReady(MultiplayerContext *_Ctx);
bool Multiplayer_AllReady(MultiplayerContext *_Ctx);
int Multiplayer_GetLocalPlayerIndex(MultiplayerContext *_Ctx);
void Multiplayer_UpdateCountdown(MultiplayerContext *_Ctx, unsigned int _CurrentTime);

// Serialization
json_t* Multiplayer_SerializeState(MultiplayerContext *_Ctx);
void Multiplayer_DeserializeState(MultiplayerContext *_Ctx, json_t *_Data);

// Turn battle operations
void Multiplayer_StartTurnAttempt(MultiplayerContext *_Ctx);
void Multiplayer_FinishTurnAttempt(MultiplayerContext *_Ctx);
void Multiplayer_SubmitTurnResults(MultiplayerContext *_Ctx);
bool Multiplayer_AllTurnsFinished(MultiplayerContext *_Ctx);
void Multiplayer_CalculateTurnWinner(MultiplayerContext *_Ctx);

#endif
