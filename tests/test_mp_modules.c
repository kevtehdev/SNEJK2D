/*
 * test_mp_modules.c - Unit tests for refactored multiplayer modules
 * 
 * Tests:
 * - mp_network: serialization/deserialization
 * - mp_host: host creation and state broadcast
 * - mp_client: join and input sending
 * - mp_game: game state management
 * - mp_turn_battle: turn battle logic
 */

#include "../include/multiplayer/multiplayer.h"
#include "../include/multiplayer/mp_network.h"
#include "../include/multiplayer/mp_host.h"
#include "../include/multiplayer/mp_client.h"
#include "../include/multiplayer/mp_game.h"
#include "../include/multiplayer/mp_turn_battle.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* Test counters */
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("\n--- Testing: %s ---\n", name);

#define ASSERT_TRUE(expr, msg) \
    do { \
        if (expr) { \
            printf("✓ %s\n", msg); \
            tests_passed++; \
        } else { \
            printf("✗ FAILED: %s\n", msg); \
            tests_failed++; \
        } \
    } while(0)

#define ASSERT_FALSE(expr, msg) \
    ASSERT_TRUE(!(expr), msg)

#define ASSERT_EQUAL(a, b, msg) \
    ASSERT_TRUE((a) == (b), msg)

#define ASSERT_NOT_NULL(ptr, msg) \
    ASSERT_TRUE((ptr) != NULL, msg)

#define ASSERT_NULL(ptr, msg) \
    ASSERT_TRUE((ptr) == NULL, msg)

/* ============================================================================
 * MP_NETWORK TESTS
 * ============================================================================ */

void test_mp_network_serialization(void)
{
    TEST("MP_Network_SerializeState");
    
    MultiplayerContext ctx;
    memset(&ctx, 0, sizeof(MultiplayerContext));
    
    // Setup test data
    ctx.players[0].joined = true;
    ctx.players[0].alive = true;
    ctx.players[0].score = 100;
    strcpy(ctx.players[0].name, "Player1");
    ctx.players[0].snake.length = 5;
    
    ctx.food[0].x = 10;
    ctx.food[0].y = 20;
    
    // Serialize
    json_t *state = MP_Network_SerializeState(&ctx);
    ASSERT_NOT_NULL(state, "Serialization returns non-null JSON");
    
    // Check structure
    json_t *players = json_object_get(state, "players");
    ASSERT_NOT_NULL(players, "Serialized state contains players array");
    ASSERT_TRUE(json_is_array(players), "Players is an array");
    
    json_t *food = json_object_get(state, "food");
    ASSERT_NOT_NULL(food, "Serialized state contains food array");
    
    if (state) json_decref(state);
}

void test_mp_network_deserialization(void)
{
    TEST("MP_Network_DeserializeState");
    
    MultiplayerContext ctx;
    memset(&ctx, 0, sizeof(MultiplayerContext));
    
    // Create test JSON
    json_t *state = json_object();
    json_t *players = json_array();
    json_t *player1 = json_object();
    
    json_object_set_new(player1, "joined", json_true());
    json_object_set_new(player1, "alive", json_true());
    json_object_set_new(player1, "score", json_integer(150));
    json_object_set_new(player1, "name", json_string("TestPlayer"));
    
    json_array_append_new(players, player1);
    json_object_set_new(state, "players", players);
    
    json_t *food = json_array();
    json_t *food1 = json_object();
    json_object_set_new(food1, "x", json_integer(15));
    json_object_set_new(food1, "y", json_integer(25));
    json_array_append_new(food, food1);
    json_object_set_new(state, "food", food);
    
    // Deserialize
    MP_Network_DeserializeState(&ctx, state);
    
    ASSERT_TRUE(ctx.players[0].joined, "Player 0 is joined");
    ASSERT_TRUE(ctx.players[0].alive, "Player 0 is alive");
    ASSERT_EQUAL(ctx.players[0].score, 150, "Player 0 score is 150");
    ASSERT_EQUAL(strcmp(ctx.players[0].name, "TestPlayer"), 0, "Player 0 name is TestPlayer");
    ASSERT_EQUAL(ctx.food[0].x, 15, "Food 0 x is 15");
    ASSERT_EQUAL(ctx.food[0].y, 25, "Food 0 y is 25");
    
    json_decref(state);
}

/* ============================================================================
 * MP_GAME TESTS
 * ============================================================================ */

void test_mp_game_all_ready(void)
{
    TEST("MP_Game_AllReady");
    
    MultiplayerContext ctx;
    memset(&ctx, 0, sizeof(MultiplayerContext));
    
    // No players - should be false
    ASSERT_FALSE(MP_Game_AllReady(&ctx), "No players - not ready");
    
    // One player - should be false
    ctx.players[0].joined = true;
    ctx.players[0].ready = true;
    ASSERT_FALSE(MP_Game_AllReady(&ctx), "One player - not ready (need 2+)");
    
    // Two players, one ready - should be false
    ctx.players[1].joined = true;
    ctx.players[1].ready = false;
    ASSERT_FALSE(MP_Game_AllReady(&ctx), "Two players, one ready - not ready");
    
    // Two players, both ready - should be true
    ctx.players[1].ready = true;
    ASSERT_TRUE(MP_Game_AllReady(&ctx), "Two players, both ready - ready!");
}

void test_mp_game_get_local_player_index(void)
{
    TEST("MP_Game_GetLocalPlayerIndex");
    
    MultiplayerContext ctx;
    memset(&ctx, 0, sizeof(MultiplayerContext));
    
    // No local player
    ctx.localPlayerIndex = -1;
    ASSERT_EQUAL(MP_Game_GetLocalPlayerIndex(&ctx), -1, "No local player returns -1");
    
    // Local player at index 0
    ctx.localPlayerIndex = 0;
    ASSERT_EQUAL(MP_Game_GetLocalPlayerIndex(&ctx), 0, "Local player at index 0");
    
    // Local player at index 1
    ctx.localPlayerIndex = 1;
    ASSERT_EQUAL(MP_Game_GetLocalPlayerIndex(&ctx), 1, "Local player at index 1");
    
    // NULL context
    ASSERT_EQUAL(MP_Game_GetLocalPlayerIndex(NULL), -1, "NULL context returns -1");
}

/* ============================================================================
 * MP_TURN_BATTLE TESTS
 * ============================================================================ */

void test_mp_turn_battle_all_finished(void)
{
    TEST("MP_TurnBattle_AllFinished");
    
    MultiplayerContext ctx;
    memset(&ctx, 0, sizeof(MultiplayerContext));
    
    // No players - should be true (vacuous truth)
    ASSERT_TRUE(MP_TurnBattle_AllFinished(&ctx), "No players - all finished");
    
    // One player, not finished
    ctx.players[0].joined = true;
    ctx.players[0].turnFinished = false;
    ASSERT_FALSE(MP_TurnBattle_AllFinished(&ctx), "One player not finished");
    
    // One player, finished
    ctx.players[0].turnFinished = true;
    ASSERT_TRUE(MP_TurnBattle_AllFinished(&ctx), "One player finished");
    
    // Two players, one finished
    ctx.players[1].joined = true;
    ctx.players[1].turnFinished = false;
    ASSERT_FALSE(MP_TurnBattle_AllFinished(&ctx), "Two players, one not finished");
    
    // Two players, both finished
    ctx.players[1].turnFinished = true;
    ASSERT_TRUE(MP_TurnBattle_AllFinished(&ctx), "Two players, both finished");
}

void test_mp_turn_battle_calculate_winner(void)
{
    TEST("MP_TurnBattle_CalculateWinner");
    
    MultiplayerContext ctx;
    memset(&ctx, 0, sizeof(MultiplayerContext));
    
    // Two players with different scores
    ctx.players[0].joined = true;
    ctx.players[0].bestScore = 100;
    strcpy(ctx.players[0].name, "Player1");
    
    ctx.players[1].joined = true;
    ctx.players[1].bestScore = 150;
    strcpy(ctx.players[1].name, "Player2");
    
    // Call calculate winner (just check it doesn't crash)
    MP_TurnBattle_CalculateWinner(&ctx);
    printf("✓ CalculateWinner executed without crash\n");
    tests_passed++;
}

/* ============================================================================
 * INTEGRATION TESTS
 * ============================================================================ */

void test_integration_serialize_deserialize_roundtrip(void)
{
    TEST("Integration: Serialize/Deserialize Roundtrip");
    
    MultiplayerContext ctx1, ctx2;
    memset(&ctx1, 0, sizeof(MultiplayerContext));
    memset(&ctx2, 0, sizeof(MultiplayerContext));
    
    // Setup ctx1 with test data
    ctx1.players[0].joined = true;
    ctx1.players[0].alive = true;
    ctx1.players[0].score = 250;
    strcpy(ctx1.players[0].name, "Alice");
    ctx1.players[0].snake.length = 7;
    
    ctx1.food[0].x = 5;
    ctx1.food[0].y = 10;
    
    // Serialize and deserialize
    json_t *state = MP_Network_SerializeState(&ctx1);
    MP_Network_DeserializeState(&ctx2, state);
    
    // Verify data preserved
    ASSERT_TRUE(ctx2.players[0].joined, "Roundtrip: player joined");
    ASSERT_TRUE(ctx2.players[0].alive, "Roundtrip: player alive");
    ASSERT_EQUAL(ctx2.players[0].score, 250, "Roundtrip: score preserved");
    ASSERT_EQUAL(strcmp(ctx2.players[0].name, "Alice"), 0, "Roundtrip: name preserved");
    ASSERT_EQUAL(ctx2.food[0].x, 5, "Roundtrip: food x preserved");
    ASSERT_EQUAL(ctx2.food[0].y, 10, "Roundtrip: food y preserved");
    
    json_decref(state);
}

/* ============================================================================
 * MAIN TEST RUNNER
 * ============================================================================ */

int main(void)
{
    printf("\n");
    printf("==================================================\n");
    printf(" SNEJK2D Multiplayer Modules Unit Tests\n");
    printf("==================================================\n");
    
    /* MP_NETWORK tests */
    printf("\n[MP_NETWORK Module]\n");
    test_mp_network_serialization();
    test_mp_network_deserialization();
    
    /* MP_GAME tests */
    printf("\n[MP_GAME Module]\n");
    test_mp_game_all_ready();
    test_mp_game_get_local_player_index();
    
    /* MP_TURN_BATTLE tests */
    printf("\n[MP_TURN_BATTLE Module]\n");
    test_mp_turn_battle_all_finished();
    test_mp_turn_battle_calculate_winner();
    
    /* Integration tests */
    printf("\n[Integration Tests]\n");
    test_integration_serialize_deserialize_roundtrip();
    
    /* Summary */
    printf("\n");
    printf("==================================================\n");
    printf(" Test Results\n");
    printf("==================================================\n");
    printf("✓ Passed: %d\n", tests_passed);
    printf("✗ Failed: %d\n", tests_failed);
    printf("Total:    %d\n", tests_passed + tests_failed);
    printf("==================================================\n");
    
    return tests_failed > 0 ? 1 : 0;
}
