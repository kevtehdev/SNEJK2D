# Unit Tests for SNEJK2D Multiplayer Modules

This directory contains unit tests for the refactored multiplayer modules.

## Building and Running Tests

```bash
cd tests
make          # Build tests
make run      # Build and run tests
make clean    # Clean build artifacts
```

## Test Coverage

- **mp_network**: Serialization/deserialization of game state
- **mp_game**: Game state management (ready status, player tracking)
- **mp_turn_battle**: Turn battle mode logic (attempts, scoring, winner calculation)
- **Integration**: End-to-end serialization roundtrip

## Adding New Tests

1. Add test function to `test_mp_modules.c`
2. Call test function from `main()`
3. Use TEST() macro to name test
4. Use ASSERT macros to verify behavior

## Test Macros

- `TEST(name)` - Define test section
- `ASSERT_TRUE(expr, msg)` - Assert expression is true
- `ASSERT_FALSE(expr, msg)` - Assert expression is false
- `ASSERT_EQUAL(a, b, msg)` - Assert two values are equal
- `ASSERT_NOT_NULL(ptr, msg)` - Assert pointer is not NULL
