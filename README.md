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

Players compete online. One creates lobby (HOST GAME), others join (JOIN GAME). Supports up to 10 players.

**Spelsätt:**
- **1VS1**: Klassiskt multiplayer där första som dör förlorar (2 spelare)
- **TURN BATTLE**: Varje spelare spelar 3 försök solo, högsta poäng vinner (upp till 10 spelare)
  - Host kan välja mellan Classic och Power-Up mode med piltangenter i lobby

### Testning av multiplayer lokalt

För att testa multiplayer på samma dator, starta två terminaler:

**Terminal 1 (HOST med ljud):**
```bash
./snejk2d
```

**Terminal 2 (CLIENT utan ljud):**
```bash
SNEJK2D_NO_AUDIO=1 ./snejk2d
```

Detta förhindrar ljudkonflikter när båda instanserna körs samtidigt.

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
