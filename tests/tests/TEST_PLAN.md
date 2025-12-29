# SNEJK2D Multiplayer Module Test Plan

## Overview

This document outlines the testing strategy for the refactored multiplayer modules.

## Module Structure

```
src/multiplayer/
├── mp_core.c         (768 lines) - Main coordination, event handling
├── mp_network.c      (385 lines) - Serialization, deserialization, browsing
├── mp_host.c         (348 lines) - Host creation, updates, broadcasting
├── mp_client.c       (219 lines) - Join, input sending, ready status
├── mp_game.c         (220 lines) - Game state management
└── mp_turn_battle.c  (184 lines) - Turn battle mode
```

## Test Coverage

### 1. MP_NETWORK Module (mp_network.c)

**Functions:**
- `MP_Network_SerializeState()` - Serialize game state to JSON
- `MP_Network_DeserializeState()` - Deserialize JSON to game state
- `MP_Network_BrowseGames()` - Browse available multiplayer games

**Test Cases:**
- ✓ Serialize empty context
- ✓ Serialize context with 1 player
- ✓ Serialize context with 2 players
- ✓ Serialize snake segments correctly
- ✓ Serialize combo system data
- ✓ Deserialize and verify all fields match
- ✓ Roundtrip: serialize → deserialize → verify identity
- ✓ Handle NULL context gracefully
- ✓ Browse games filters by student number
- ✓ Parse map ID from room name format

### 2. MP_HOST Module (mp_host.c)

**Functions:**
- `MP_Host_Create()` - Create new multiplayer session
- `MP_Host_Update()` - Update game state (host authority)
- `MP_Host_BroadcastState()` - Broadcast state to clients

**Test Cases:**
- ✓ Create host with valid player name
- ✓ Host auto-joins as player 0
- ✓ Room name includes student number and map ID
- ✓ Broadcast interval is 16ms (minimal input lag)
- ✓ Host update moves all player snakes
- ✓ Host detects collisions (wall, self, other players)
- ✓ Host handles food collection
- ✓ Host calculates combo multipliers correctly
- ✓ Broadcast sends valid JSON state

### 3. MP_CLIENT Module (mp_client.c)

**Functions:**
- `MP_Client_Join()` - Join existing session
- `MP_Client_SendInput()` - Send input to server
- `MP_Client_ToggleReady()` - Toggle ready status

**Test Cases:**
- ✓ Join with valid session ID
- ✓ Parse existing players from join response
- ✓ Identify local player correctly
- ✓ Initialize snakes at correct positions
- ✓ Send input only when alive
- ✓ Don't send input if host
- ✓ Toggle ready status
- ✓ Broadcast ready status to server
- ✓ Handle join failures gracefully

### 4. MP_GAME Module (mp_game.c)

**Functions:**
- `MP_Game_Start()` - Start multiplayer game
- `MP_Game_Restart()` - Restart after game over
- `MP_Game_UpdateCountdown()` - Countdown management
- `MP_Game_AllReady()` - Check if all players ready
- `MP_Game_GetLocalPlayerIndex()` - Get local player index

**Test Cases:**
- ✓ AllReady returns false with < 2 players
- ✓ AllReady returns false when not all ready
- ✓ AllReady returns true when all ready (2+ players)
- ✓ GetLocalPlayerIndex returns -1 for NULL context
- ✓ GetLocalPlayerIndex returns correct index
- ✓ Start game transitions to countdown for 1VS1
- ✓ Start game transitions to ready-up for turn battle
- ✓ Countdown initializes snakes correctly
- ✓ Countdown spawns food items
- ✓ Restart resets all player states

### 5. MP_TURN_BATTLE Module (mp_turn_battle.c)

**Functions:**
- `MP_TurnBattle_StartAttempt()` - Start turn attempt
- `MP_TurnBattle_FinishAttempt()` - Finish and record attempt
- `MP_TurnBattle_SubmitResults()` - Submit results to host
- `MP_TurnBattle_AllFinished()` - Check if all players finished
- `MP_TurnBattle_CalculateWinner()` - Calculate winner by best score

**Test Cases:**
- ✓ AllFinished returns true for no players
- ✓ AllFinished returns false if any player not finished
- ✓ AllFinished returns true when all finished
- ✓ StartAttempt initializes local game
- ✓ StartAttempt applies correct game mode
- ✓ FinishAttempt records score, length, time
- ✓ After 3 attempts, calculate best score
- ✓ After 3 attempts, submit results
- ✓ CalculateWinner finds highest score
- ✓ Host broadcasts results when all finished

## Integration Tests

### End-to-End Scenarios

**1. Complete 1VS1 Game Flow**
1. Player 1 hosts game
2. Player 2 joins game
3. Both players toggle ready
4. Game starts with countdown
5. Players send inputs
6. Host updates game state
7. Host broadcasts state (16ms interval)
8. Client receives and deserializes state
9. Player dies or wins
10. Game over, option to restart

**2. Complete Turn Battle Flow**
1. Player 1 hosts turn battle
2. Player 2 joins
3. Both ready
4. Game starts ready-up phase
5. Player 1 completes 3 attempts
6. Player 2 completes 3 attempts
7. Host calculates winner
8. Results displayed

**3. Network Robustness**
- Handle player disconnection mid-game
- Handle late joins (reject after game started)
- Handle invalid JSON from client
- Handle slow network (16ms broadcast tolerance)

## Manual Testing Checklist

### Before Push to Main

- [ ] Build succeeds without warnings
- [ ] Game launches and reaches main menu
- [ ] Can host a game
- [ ] Can join a game from another instance
- [ ] Both players see each other in lobby
- [ ] Ready status works for both players
- [ ] Game starts after both ready
- [ ] Countdown displays correctly
- [ ] Snakes move smoothly for both players
- [ ] Input lag is minimal (< 50ms perceived)
- [ ] Food collection works
- [ ] Collision detection works (wall, self, other player)
- [ ] Game over displays correctly
- [ ] Can restart game
- [ ] Turn battle mode works (3 attempts per player)
- [ ] Turn battle winner calculated correctly
- [ ] Can return to lobby and start new game
- [ ] No memory leaks (valgrind clean)
- [ ] No segfaults or crashes

## Performance Benchmarks

### Target Metrics
- **State broadcast latency**: < 20ms (target: 16ms)
- **Serialization time**: < 5ms for typical game state
- **Deserialization time**: < 5ms
- **Memory usage**: < 1MB for MultiplayerContext
- **Network bandwidth**: < 10KB/s per player at 60 FPS

### Profiling Commands
```bash
# Memory leak check
valgrind --leak-check=full ./snejk2d

# CPU profiling
perf record -g ./snejk2d
perf report

# Network profiling
tcpdump -i lo -w multiplayer.pcap
wireshark multiplayer.pcap
```

## Known Issues / Edge Cases

1. **JOIN GAME refresh bug**: When entering JOIN GAME before any games exist, pressing R doesn't show newly created games. Need to investigate MP_Network_BrowseGames() caching.

2. **Fast input spam**: Multiple rapid inputs (< 16ms apart) might be lost before next broadcast. Consider input queue.

3. **Race condition**: If both players press ready simultaneously, might cause double-start. Add mutex or state check.

## Success Criteria

✓ All modules compile without errors
✓ No breaking changes to public API
✓ Code reduced by 37% (1205 → 768 lines in mp_core.c)
✓ Separation of concerns achieved
✓ Each module has single responsibility
✓ Wrapper functions maintain backwards compatibility
✓ Documentation complete
