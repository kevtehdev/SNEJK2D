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

Varje spelare försöker äta mat och växa. Första som dör förlorar.

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
