# SNEJK2D

Snake game med singleplayer och multiplayer (1v1).

## Spela

Starta spelet:
```
./snejk2d
```

Eller på Windows:
```
snejk2d.exe
```

## Kontroller

**Menyer:**
- Piltangenter: navigera
- Enter: välj
- ESC: tillbaka

**Spel:**
- Piltangenter: styr ormen

## Multiplayer

Två spelare kan spela mot varandra online. En skapar lobby (HOST GAME), den andra går med (JOIN GAME).

**Spelsätt:**
- **REALTIME**: Klassiskt multiplayer där första som dör förlorar
- **TURN BATTLE**: Varje spelare spelar 3 försök solo, högsta poäng vinner

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

## Bygga från källkod

Kräver SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, jansson och uuid.

På Ubuntu/WSL2:
```bash
make deps
make
```

På Windows behövs MinGW och alla SDL2-bibliotek.

## License

Se LICENSE fil för info.
