# Changelog

## [1.3.0] - 2025-12-27

### Changed
- Turn Battle is now the default multiplayer mode
- Realtime mode renamed to 1VS1
- Countdown reduced from 5 to 3 seconds
- Results screen improvements - best score moved next to player name, navigation text in white at bottom

### Added
- Chat notification sound when messages arrive
- Lightning effect during countdown with random flashes and pulsing

### Fixed
- Audio crackling by increasing buffer size and lowering sound effect volume
- Results screen text overlap

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
