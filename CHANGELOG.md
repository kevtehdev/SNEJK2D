# Changelog

## [v1.4.1] - 2025-12-29

### Performance
- Removed excessive debug printf statements from state_multiplayer.c
  - Statements were printing 60 times per second causing console flooding
  - Improved performance and reduced terminal spam

## [v1.4.0] - 2025-12-29

### Code Refactoring
- Multiplayer code split into 6 focused modules
- mp_core.c reduced from 1859 to 768 lines (-59%)
- New modules: mp_network, mp_host, mp_client, mp_game, mp_turn_battle
- All functions kept backwards compatible using wrappers

### New Systems
- Logger system (logs to snejk2d.log for debugging)
- Runtime config system (loads from snejk2d.conf if present)
- Debug overlay (toggle with F3 to show FPS and game stats)

### Build System
- Updated Makefile.win with all new modules
- Simplified build_windows.sh to use Makefile.win
- Added *.log files to .gitignore

## [1.3.7] - 2025-12-28

### Bug Fixes
- Fixed XP bar and level display in multiplayer Turn Battle mode
  - XP bar now shows correctly during Power-Up mode gameplay
  - Level display now appears in top HUD border
  - "LEVEL X!" animation now triggers on level-up in multiplayer
  - Combo multiplier info now visible in Turn Battle

## [1.3.6] - 2025-12-28

### Audio
- Added in-game volume controls
  - Press `+` or `=` to increase volume
  - Press `-` to decrease volume
  - Works in both singleplayer and multiplayer
  - Volume adjusts in 10% increments (0-100%)

### Documentation
- Added comprehensive asset attribution (assets/ASSETS_LICENSE.md)
  - All backgrounds by Ansimuz (Forest, Mountain, Cyberpunk, City, Country, Underwater)
  - Medals by Buch from OpenGameArt (CC0 license)
  - Music by Troubadeck
  - PressStart2P font from Google Fonts (SIL OFL 1.1)
- Improved README with detailed power-up scoring information
  - Regular food: +10 points
  - Golden Apple: +100 points
  - Speed Boost: +20 points
  - Score Multiplier: +50 points
  - Combo system explained (1.5x, 2x, 3x, 5x multipliers)

### UI
- Fixed scoreboard "ESC BACK" text position (no longer overlaps #4 position)

## [1.3.4] - 2025-12-28

### Security
- Fixed buffer overflow vulnerabilities in string operations
  - Replaced unsafe `strcat()` with `strncat()` in chat and nickname input
  - Replaced unsafe `strcpy()` with `memmove()` and `strncpy()`
- Fixed potential race conditions in multiplayer threading
  - Added mutex protection for thread state variables
- Memory leak analysis: Valgrind confirms zero memory leaks

### Gameplay
- Fixed snake tail collision detection
  - You can no longer collide with your own tail (it moves away)
  - Allows for tighter turns and professional Snake gameplay
  - Matches classic Snake behavior (Nokia, Google Snake)

### Technical
- All string operations now use bounds-checked functions
- Thread-safe multiplayer connection handling
- Improved code quality and security practices

## [1.3.3] - 2025-12-27

### Power-Ups
- Removed Shield power-up for clearer gameplay
- Speed Boost now cyan instead of yellow for better visibility
- Three distinct power-ups: Golden Apple (yellow), Speed Boost (cyan), Score Multiplier (magenta)

### Turn Battle Mode Selection
- Choose between Classic and Power-Up modes in Turn Battle
- Host uses arrow keys in lobby to switch between modes
- Selected mode shown in lobby and ready screen
- Mode syncs automatically to all players

### UI
- Cleaner lobby footer - removed redundant instruction text
- Mode displayed clearly in cyan

## [1.3.2] - 2025-12-27

### Turn Battle Mode Selection
- Choose between Classic and Power-Up modes in Turn Battle
- Host uses arrow keys in lobby to switch between modes
- Selected mode shown in lobby and ready screen
- Mode syncs automatically to all players

### UI
- Cleaner lobby footer - removed redundant instruction text
- Mode displayed clearly in cyan

## [1.3.1] - 2025-12-27

### Turn Battle
- Attempt counter moved to HUD area during countdown
- New countdown animation - zooms from large to normal with dark pulsing overlay
- Background overlay added for consistency with singleplayer
- Grid opacity reduced to 2 for cleaner look

### Power-Up Mode
- Reduced simultaneous power-ups from 5 to 3
- Added level system with XP progression
  - Gain 10 XP per food eaten
  - Level up at 100, 150, 200, 250 XP etc.
- Level display in top HUD next to score
- XP progress bar in bottom HUD with cyan styling
- Level-up celebration effect with screen flash and zoom text

### UI
- Borderless window mode
- OPTIONS menu text capitalized for consistency
- Better visual feedback throughout

## [1.3.0] - 2025-12-27

### Changed
- Turn Battle is now the default multiplayer mode
- Realtime mode renamed to 1VS1
- Countdown reduced from 5 to 3 seconds
- Results screen improvements - best score moved next to player name, navigation text in white at bottom

### Added
- Chat notification sound when messages arrive
- Lightning effect during countdown with random flashes and pulsing
- Retro loading screen with rotating messages

### Fixed
- Audio crackling by increasing buffer size and lowering sound effect volume
- Results screen text overlap
- Buffer overflow in scoreboard
- Chat and nick input trigger key appearing in text

## [1.2.0] - 2025-12-26

### Added
- **Turn Battle Results Redesign**: New paginated results view with animated medals
  - Navigate between players with arrow keys (Left/Right) or A/D
  - Platinum medal for 1st place, Gold for 2nd, Silver for 3rd, Bronze for 4th+
  - Players automatically sorted by best score
  - Clean, focused view showing one player at a time with all attempt details

- **Multi-Instance Audio Support**: Environment variable to disable audio when running multiple instances
  - Set `SNEJK2D_NO_AUDIO=1` to run without audio
  - Perfect for local multiplayer testing
  - Prevents audio conflicts between processes

### Fixed
- **Multiplayer Disconnect Handling**: Improved stability when players leave during turn battle
  - Players now broadcast disconnect messages before leaving
  - Host automatically shows results if opponent disconnects during waiting phase
  - No more infinite waiting when opponent leaves unexpectedly

- **Turn Battle Synchronization**: Results display immediately when all players finish
  - Fixed edge case where host finishing last would cause waiting state
  - Both host and client see results at the same time

- **Input Lag Reduction**: Maintained the 85-95% input lag reduction from v1.1.0
  - Reduced from 100-250ms to 16-32ms

### Technical Improvements
- Added `player_left` and `player_leaving` event handlers
- Improved graceful disconnect with proper state cleanup
- Enhanced turn battle state management
- Better error recovery in multiplayer sessions

## [1.1.0] - 2025-12-25

### Added
- Turn Battle multiplayer mode
- Pause menu overlay improvements
- New game mode in singleplayer

### Fixed
- Input lag reduced by 85-95% (from 100-250ms to 16-32ms)
- Max speed and HUD effects for combos
- Release process improvements

## [1.0.0] - Initial Release
- Classic Snake gameplay
- Multiplayer support
- Power-up system
- Scoreboard
