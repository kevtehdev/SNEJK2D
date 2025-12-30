# SNEJK2D

snake game with multiplayer. nothing fancy, just works.

## what is this

reimagined snake built with SDL2 in C

features:
- classic snake mechanics (eat, grow, don't hit walls)
- power-ups and combo system if you're into that
- online multiplayer for up to 10 players
- no ads, no bs, just snake

## how to run

download from releases, extract, run the executable.

**linux:** `./snejk2d`
**windows:** `snejk2d.exe`

## game modes

### singleplayer

**classic** - you know how this works. eat apples, get longer, try not to die.

**power-up mode** - same thing but with extra stuff:
- golden apples (yellow) give you bonus points
- speed boosts (cyan) make you go fast for a bit
- score multipliers (magenta) are basically cheat codes for 5 seconds
- combo system rewards you for eating fast
- level up by eating food (10 xp per apple)

### multiplayer

**1vs1** - two players, one winner. straightforward competitive snake.

**turn battle** - everyone gets 3 tries, best score wins. works with 2-10 players. host picks classic or power-up mode before starting.

multiplayer uses session codes. one person hosts, others join with the code. that's it.

## controls

**menus:**
arrow keys or wasd to move around, enter to select, esc to go back

**in game:**
arrow keys or wasd to move the snake
`+` or `=` to turn up volume
`-` to turn down volume
esc to pause

**multiplayer lobby:**
space to ready up
enter to start (if you're host)
c for chat
n to change your name
arrow keys to switch modes (turn battle only, host only)

## building it yourself

you'll need SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, and some other stuff.

**linux/wsl:**
```bash
make deps    # gets everything you need
make         # builds the game
```

**windows:**
cross-compile with mingw. check `build_windows.sh` if you really want to do this.

## testing multiplayer locally

run two instances:
```bash
# terminal 1 (normal)
./snejk2d

# terminal 2 (no audio to avoid conflicts)
SNEJK2D_NO_AUDIO=1 ./snejk2d
```

## credits

the game code is mine, mit license.

multiplayer api is mpapi https://github.com/robin-onvo/mpapi

assets (backgrounds, music, fonts) are from various creators. full attribution in `assets/ASSETS_LICENSE.md`.

## notes

- backgrounds by ansimuz (forest, mountain, cyberpunk, city, etc)
- music by troubadeck
- font is press start 2p from google fonts
- medals by buch

