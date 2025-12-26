# SNEJK2D

Snake game with singleplayer and multiplayer.

## Play

```bash
./snejk2d
```

Windows:
```bash
snejk2d.exe
```

## Controls

**Menus:**
- Arrow keys / WASD: navigate
- Enter: select
- ESC: back

**Game:**
- Arrow keys / WASD: move

## Game Modes

**Classic:** Traditional snake gameplay

**Power-Up:** Enhanced mode with collectible power-ups
- Golden Apple (yellow): +3 length
- Speed Boost (cyan): temporary speed increase
- Shield (blue): one-hit protection
- Score Multiplier (magenta): 10x points for 5 seconds

## Multiplayer

Two players compete online. One creates lobby (HOST GAME), the other joins (JOIN GAME).

First player to die loses.

## Building

Requires SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, jansson and uuid.

Ubuntu/WSL2:
```bash
make deps
make
```

Windows requires MinGW and SDL2 libraries.

## License

See LICENSE file.
