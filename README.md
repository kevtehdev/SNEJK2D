# SNEJK2D

A modern Snake game with singleplayer and online multiplayer. Built with SDL2 in C.

## Quick Start

**Linux:**
```bash
./snejk2d
```

**Windows:**
```bash
snejk2d.exe
```

## Game Modes

### Singleplayer

**Classic Mode**
Traditional snake gameplay. Eat food, grow longer, avoid walls and yourself.

**Power-Up Mode**
Enhanced gameplay with collectible power-ups, combo system, and level progression:

**Regular Food** (red apple):
- +1 length
- +10 points (affected by combo multiplier and active power-ups)
- +10 XP

**Power-Ups:**
- **Golden Apple** (yellow): +3 length, +100 points, +5 combo
- **Speed Boost** (cyan): +20 points, +1 combo, temporary speed increase
- **Score Multiplier** (magenta): +50 points, +3 combo, 10x score multiplier for 5 seconds

**Combo System:**
Eating food quickly builds combos that multiply your score:
- 3+ combo: 1.5x multiplier
- 6+ combo: 2x multiplier
- 10+ combo: 3x multiplier
- 15+ combo: 5x multiplier

**Level System:**
Gain 10 XP per food eaten. Level up at 100, 150, 200, 250 XP and beyond.

### Multiplayer

Supports up to 10 players online. One player hosts, others join using the session code.

**1VS1 Mode** (2 players)
Classic competitive mode. First to die loses.

**Turn Battle Mode** (up to 10 players)
Each player gets 3 solo attempts. Highest score wins. Host can choose between Classic and Power-Up gameplay using arrow keys in the lobby.

## Controls

**Menus:**
- Arrow keys / WASD: Navigate
- Enter: Select
- ESC: Back

**In-Game:**
- Arrow keys / WASD: Move snake
- + / =: Increase volume
- -: Decrease volume
- ESC: Pause

**Multiplayer Lobby:**
- Space: Ready/unready
- Enter: Start game (host only)
- C: Open chat
- N: Change nickname
- Arrow keys: Change mode (host only, Turn Battle)

## Building from Source

Requires SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, jansson, and uuid.

**Ubuntu/Debian/WSL2:**
```bash
make deps  # Install dependencies
make       # Build
```

**Windows:**
Requires MinGW cross-compiler and SDL2 libraries. See `build_windows.sh` for details.

## Testing Multiplayer Locally

Run two instances on the same machine:

**Terminal 1 (with audio):**
```bash
./snejk2d
```

**Terminal 2 (without audio):**
```bash
SNEJK2D_NO_AUDIO=1 ./snejk2d
```

This prevents audio conflicts when running multiple instances.

## License

This project uses the MIT License for code. See the [LICENSE](LICENSE) file for details.

For third-party assets (graphics, music, fonts), see [assets/ASSETS_LICENSE.md](assets/ASSETS_LICENSE.md) for full attribution and licensing information.
