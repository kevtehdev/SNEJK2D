/*
 * mp_turn_battle.c - Turn battle mode implementation
 * Part of SNEJK2D multiplayer refactoring (Phase 4)
 *
 * Functions:
 * - Start/finish turn attempts
 * - Submit results
 * - Calculate winner
 */

#include "../../include/multiplayer/multiplayer.h"
#include "../../include/multiplayer/mp_turn_battle.h"
#include "../../include/multiplayer/mp_game.h"
#include "../../include/core/game.h"
#include <SDL2/SDL.h>
#include <stdio.h>

/*
 * Start a turn battle attempt (1 of 3)
 */
void MP_TurnBattle_StartAttempt(MultiplayerContext *_Ctx)
{
    if (!_Ctx || _Ctx->gameMode != MP_MODE_TURN_BATTLE)
        return;

    // Initialize local game for this attempt
    Game_Init(&_Ctx->localGame);
    _Ctx->localGame.state = GAME_PLAYING;
    _Ctx->localGame.selectedBackground = _Ctx->selectedBackground;
    _Ctx->localGame.gameMode = _Ctx->turnBattleMode; // Apply selected mode (Classic or Power-Up)

    // Start with countdown (same as 1VS1 mode)
    _Ctx->state = MP_STATE_COUNTDOWN;
    _Ctx->countdownStart = SDL_GetTicks();
    _Ctx->gameStartTime = _Ctx->countdownStart + 3000; // 3 second countdown
    _Ctx->attemptStartTime = _Ctx->gameStartTime; // Attempt starts after countdown

    printf("Starting turn attempt %d/3 with countdown\n", _Ctx->currentAttempt + 1);
}

/*
 * Finish current turn attempt and record results
 */
void MP_TurnBattle_FinishAttempt(MultiplayerContext *_Ctx)
{
    if (!_Ctx || _Ctx->gameMode != MP_MODE_TURN_BATTLE)
        return;

    int localIdx = MP_Game_GetLocalPlayerIndex(_Ctx);
    if (localIdx < 0)
        return;

    // Record attempt results
    TurnAttempt *attempt = &_Ctx->players[localIdx].attempts[_Ctx->currentAttempt];
    attempt->score = _Ctx->localGame.snake.score;
    attempt->length = _Ctx->localGame.snake.length;
    attempt->survivalTime = SDL_GetTicks() - _Ctx->attemptStartTime;

    _Ctx->currentAttempt++;
    _Ctx->players[localIdx].completedAttempts = _Ctx->currentAttempt;

    printf("Finished attempt %d - Score: %d, Length: %d, Time: %ums\n",
           _Ctx->currentAttempt, attempt->score, attempt->length, attempt->survivalTime);

    // Check if all 3 attempts are done
    if (_Ctx->currentAttempt >= 3)
    {
        // Calculate best score
        int bestScore = 0;
        for (int i = 0; i < 3; i++)
        {
            if (_Ctx->players[localIdx].attempts[i].score > bestScore)
                bestScore = _Ctx->players[localIdx].attempts[i].score;
        }
        _Ctx->players[localIdx].bestScore = bestScore;
        _Ctx->players[localIdx].turnFinished = true;

        // Submit results to host
        MP_TurnBattle_SubmitResults(_Ctx);
        _Ctx->state = MP_STATE_TURN_WAITING;

        // If we're host and all players are now finished, show results immediately
        if (_Ctx->isHost && MP_TurnBattle_AllFinished(_Ctx))
        {
            MP_TurnBattle_CalculateWinner(_Ctx);
            _Ctx->resultPageIndex = 0; // Start at winner
            _Ctx->state = MP_STATE_TURN_RESULTS;

            // Broadcast results state to all players
            json_t *resultsMsg = json_object();
            json_object_set_new(resultsMsg, "type", json_string("show_results"));
            mpapi_game(_Ctx->api, resultsMsg, NULL);
            json_decref(resultsMsg);
        }
    }
    else
    {
        // Start next attempt
        MP_TurnBattle_StartAttempt(_Ctx);
    }
}

/*
 * Submit turn battle results to host
 */
void MP_TurnBattle_SubmitResults(MultiplayerContext *_Ctx)
{
    if (!_Ctx || !_Ctx->api)
        return;

    int localIdx = MP_Game_GetLocalPlayerIndex(_Ctx);
    if (localIdx < 0)
        return;

    // Create results message
    json_t *message = json_object();
    json_object_set_new(message, "type", json_string("turn_results"));
    json_object_set_new(message, "playerIndex", json_integer(localIdx));
    json_object_set_new(message, "bestScore", json_integer(_Ctx->players[localIdx].bestScore));

    // Add all attempt results
    json_t *attemptsArray = json_array();
    for (int i = 0; i < 3; i++)
    {
        json_t *attemptObj = json_object();
        json_object_set_new(attemptObj, "score", json_integer(_Ctx->players[localIdx].attempts[i].score));
        json_object_set_new(attemptObj, "length", json_integer(_Ctx->players[localIdx].attempts[i].length));
        json_object_set_new(attemptObj, "survivalTime", json_integer(_Ctx->players[localIdx].attempts[i].survivalTime));
        json_array_append_new(attemptsArray, attemptObj);
    }
    json_object_set_new(message, "attempts", attemptsArray);

    mpapi_game(_Ctx->api, message, NULL);
    json_decref(message);

    printf("Submitted turn results - Best score: %d\n", _Ctx->players[localIdx].bestScore);
}

/*
 * Check if all players finished their turns
 */
bool MP_TurnBattle_AllFinished(MultiplayerContext *_Ctx)
{
    if (!_Ctx)
        return false;

    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        if (_Ctx->players[i].joined && !_Ctx->players[i].turnFinished)
            return false;
    }

    return true;
}

/*
 * Calculate turn battle winner based on best scores
 */
void MP_TurnBattle_CalculateWinner(MultiplayerContext *_Ctx)
{
    if (!_Ctx)
        return;

    int winnerIdx = -1;
    int highestScore = -1;

    for (int i = 0; i < MAX_MULTIPLAYER_PLAYERS; i++)
    {
        if (_Ctx->players[i].joined)
        {
            if (_Ctx->players[i].bestScore > highestScore)
            {
                highestScore = _Ctx->players[i].bestScore;
                winnerIdx = i;
            }
        }
    }

    if (winnerIdx >= 0)
    {
        printf("Turn Battle Winner: %s with score %d\n",
               _Ctx->players[winnerIdx].name, highestScore);
    }
}
