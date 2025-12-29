#include "../../include/audio/audio.h"
#include "../../include/core/game.h"
#include "../../include/core/event_system.h"
#include "../../include/multiplayer/multiplayer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

bool Audio_Init(AudioSystem *_Audio)
{
    printf("Initializing audio system...\n");

    // Allow disabling audio for multiple instances (prevents conflicts)
    const char *disableAudio = getenv("SNEJK2D_NO_AUDIO");
    if (disableAudio != NULL)
    {
        printf("  ⚠ Audio disabled via SNEJK2D_NO_AUDIO (running multiple instances)\n");
        _Audio->initialized = false;
        _Audio->musicEnabled = false;
        _Audio->sfxEnabled = false;
        return false;
    }

    // Use 44100 Hz to match most audio files (avoid heavy resampling)
    // Increased buffer size to 4096 to reduce crackling
    int bufferSize = 4096;
    int sampleRate = 44100;

    // Initialize SDL_mixer settings
    if (Mix_OpenAudio(sampleRate, MIX_DEFAULT_FORMAT, 2, bufferSize) < 0)
    {
        fprintf(stderr, "Warning: Failed to initialize SDL_mixer: %s\n", Mix_GetError());
        fprintf(stderr, "Continuing without audio...\n");
        fprintf(stderr, "Hint: Set SNEJK2D_NO_AUDIO=1 when running multiple instances\n");
        _Audio->initialized = false;
        return false;
    }

    // Allocate mixing channels
    Mix_AllocateChannels(16);

    _Audio->initialized = true;

    // Allow disabling music via environment variable (for debugging crackling)
    const char *disableMusic = getenv("SNEJK2D_NO_MUSIC");
    _Audio->musicEnabled = (disableMusic == NULL);
    if (!_Audio->musicEnabled)
        printf("  ⚠ Music disabled via SNEJK2D_NO_MUSIC\n");

    _Audio->sfxEnabled = true;
    _Audio->masterVolume = 0.5f;
    _Audio->currentMusic = NULL;
    _Audio->nextChannel = 0;

    // Initialize sound slots
    for (int i = 0; i < SOUND_COUNT; i++)
    {
        _Audio->sounds[i] = NULL;
    }

    // Load background music
    _Audio->menuMusic = Mix_LoadMUS("assets/music/menu.ogg");
    if (!_Audio->menuMusic)
    {
        fprintf(stderr, "Warning: Failed to load menu music: %s\n", Mix_GetError());
    }

    _Audio->singleplayerMusic = Mix_LoadMUS("assets/music/cyberpunk-street.ogg");
    if (!_Audio->singleplayerMusic)
    {
        fprintf(stderr, "Warning: Failed to load singleplayer music: %s\n", Mix_GetError());
    }

    _Audio->multiplayerMusic = Mix_LoadMUS("assets/music/cyberpunk-street.ogg");
    if (!_Audio->multiplayerMusic)
    {
        fprintf(stderr, "Warning: Failed to load multiplayer music: %s\n", Mix_GetError());
    }

    // Load sound effects
    _Audio->sounds[SOUND_CHAT] = Mix_LoadWAV("assets/music/chat-message-sound.wav");
    if (!_Audio->sounds[SOUND_CHAT])
    {
        fprintf(stderr, "Warning: Failed to load chat sound: %s\n", Mix_GetError());
    }

    // Set music volume
    Mix_VolumeMusic((int)(_Audio->masterVolume * MIX_MAX_VOLUME));

    printf("✓ Audio system initialized\n");
    printf("  - Buffer size: %d samples (%.1f ms latency)\n", bufferSize, (bufferSize / (float)sampleRate) * 1000);
    printf("  - Sample rate: %d Hz\n", sampleRate);
    printf("  - Music: %s\n", _Audio->musicEnabled ? "enabled" : "disabled");
    printf("  - SFX: %s\n", _Audio->sfxEnabled ? "enabled" : "disabled");
    printf("  - Volume: %.0f%%\n", _Audio->masterVolume * 100);

    return true;
}

void Audio_Cleanup(AudioSystem *_Audio)
{
    if (!_Audio->initialized)
        return;

    // Stop and free music
    Mix_HaltMusic();
    if (_Audio->menuMusic)
    {
        Mix_FreeMusic((Mix_Music *)_Audio->menuMusic);
        _Audio->menuMusic = NULL;
    }
    if (_Audio->singleplayerMusic)
    {
        Mix_FreeMusic((Mix_Music *)_Audio->singleplayerMusic);
        _Audio->singleplayerMusic = NULL;
    }
    if (_Audio->multiplayerMusic)
    {
        Mix_FreeMusic((Mix_Music *)_Audio->multiplayerMusic);
        _Audio->multiplayerMusic = NULL;
    }

    // Free all loaded sounds
    for (int i = 0; i < SOUND_COUNT; i++)
    {
        if (_Audio->sounds[i])
        {
            Mix_FreeChunk((Mix_Chunk *)_Audio->sounds[i]);
            _Audio->sounds[i] = NULL;
        }
    }

    Mix_CloseAudio();
    Mix_Quit();

    _Audio->initialized = false;
}

void Audio_PlaySound(AudioSystem *_Audio, SoundType _Sound)
{
    if (!_Audio->initialized || !_Audio->sfxEnabled)
        return;

    if (_Sound < 0 || _Sound >= SOUND_COUNT)
        return;

    if (_Audio->sounds[_Sound])
    {
        // Channel management: rotate through channels
        int channel = (_Audio->nextChannel % 8) + 8;
        _Audio->nextChannel++;

        // Set volume on specific channel and play sound
        // Reduced from 128 to 80 to prevent crackling/distortion
        Mix_Volume(channel, (int)(_Audio->masterVolume * 80));
        Mix_PlayChannel(channel, (Mix_Chunk *)_Audio->sounds[_Sound], 0);
    }
}

void Audio_StopSound(AudioSystem *_Audio, SoundType _Sound)
{
    if (!_Audio->initialized)
        return;

    (void)_Sound;  // Unused parameter
    Mix_HaltChannel(-1);
}

void Audio_SetVolume(AudioSystem *_Audio, float _Volume)
{
    if (_Volume < 0.0f) _Volume = 0.0f;
    if (_Volume > 1.0f) _Volume = 1.0f;

    _Audio->masterVolume = _Volume;

    if (_Audio->initialized)
    {
        Mix_VolumeMusic((int)(_Volume * MIX_MAX_VOLUME));
        Mix_Volume(-1, (int)(_Volume * 80));  // Match the volume used in Audio_PlaySound
    }

    printf("Volume: %.0f%%\n", _Volume * 100);
}

void Audio_IncreaseVolume(AudioSystem *_Audio)
{
    float newVolume = _Audio->masterVolume + 0.1f;
    if (newVolume > 1.0f) newVolume = 1.0f;
    Audio_SetVolume(_Audio, newVolume);
}

void Audio_DecreaseVolume(AudioSystem *_Audio)
{
    float newVolume = _Audio->masterVolume - 0.1f;
    if (newVolume < 0.0f) newVolume = 0.0f;
    Audio_SetVolume(_Audio, newVolume);
}

void Audio_ToggleMusic(AudioSystem *_Audio)
{
    _Audio->musicEnabled = !_Audio->musicEnabled;
    printf("Music: %s\n", _Audio->musicEnabled ? "ON" : "OFF");
}

void Audio_ToggleSfx(AudioSystem *_Audio)
{
    _Audio->sfxEnabled = !_Audio->sfxEnabled;
    printf("SFX: %s\n", _Audio->sfxEnabled ? "ON" : "OFF");
}

void Audio_PlayMenuMusic(AudioSystem *_Audio)
{
    if (!_Audio->initialized || !_Audio->musicEnabled)
        return;

    if (!_Audio->menuMusic)
        return;

    // Only restart if different music is playing
    if (_Audio->currentMusic == _Audio->menuMusic && Mix_PlayingMusic())
        return;

    Mix_HaltMusic();
    // Volume already set at init, no need to set it again (can cause glitches)
    Mix_PlayMusic((Mix_Music *)_Audio->menuMusic, -1);  // Loop forever
    _Audio->currentMusic = _Audio->menuMusic;
}

void Audio_PlayGameMusic(AudioSystem *_Audio)
{
    if (!_Audio->initialized || !_Audio->musicEnabled)
        return;

    if (!_Audio->singleplayerMusic)
        return;

    // Only restart if different music is playing
    if (_Audio->currentMusic == _Audio->singleplayerMusic && Mix_PlayingMusic())
        return;

    Mix_HaltMusic();
    // Volume already set at init, no need to set it again (can cause glitches)
    Mix_PlayMusic((Mix_Music *)_Audio->singleplayerMusic, -1);  // Loop forever
    _Audio->currentMusic = _Audio->singleplayerMusic;
}

void Audio_PlayMultiplayerMusic(AudioSystem *_Audio)
{
    if (!_Audio->initialized || !_Audio->musicEnabled)
        return;

    if (!_Audio->multiplayerMusic)
        return;

    // Only restart if different music is playing
    if (_Audio->currentMusic == _Audio->multiplayerMusic && Mix_PlayingMusic())
        return;

    Mix_HaltMusic();
    // Volume already set at init, no need to set it again (can cause glitches)
    Mix_PlayMusic((Mix_Music *)_Audio->multiplayerMusic, -1);  // Loop forever
    _Audio->currentMusic = _Audio->multiplayerMusic;
}

void Audio_StopMusic(AudioSystem *_Audio)
{
    if (!_Audio->initialized)
        return;

    Mix_HaltMusic();
    _Audio->currentMusic = NULL;
}

void Audio_UpdateMusicForState(AudioSystem *_Audio, int _GameState, int _MpState, bool _MenuMusicEnabled, bool _GameMusicEnabled)
{
    if (!_Audio->initialized)
        return;

    // Determine which music to play based on current state
    // Use int values to match enum values from game.h and multiplayer.h
    if (_GameState == (int)GAME_PLAYING)
    {
        // Singleplayer gameplay
        if (_GameMusicEnabled)
            Audio_PlayGameMusic(_Audio);
        else
            Audio_StopMusic(_Audio);
    }
    else if (_GameState == (int)GAME_MULTIPLAYER)
    {
        if (_MpState == (int)MP_STATE_PLAYING || _MpState == (int)MP_STATE_COUNTDOWN ||
            _MpState == (int)MP_STATE_TURN_PLAYING)
        {
            // Multiplayer gameplay (1VS1 and Turn Battle)
            if (_GameMusicEnabled)
                Audio_PlayMultiplayerMusic(_Audio);
            else
                Audio_StopMusic(_Audio);
        }
        else
        {
            // Multiplayer menus (browsing, lobby, etc.)
            if (_MenuMusicEnabled)
                Audio_PlayMenuMusic(_Audio);
            else
                Audio_StopMusic(_Audio);
        }
    }
    else if (_GameState == (int)GAME_MAIN_MENU || _GameState == (int)GAME_NAME_INPUT ||
             _GameState == (int)GAME_MENU || _GameState == (int)GAME_OVER ||
             _GameState == (int)GAME_SCOREBOARD || _GameState == (int)GAME_OPTIONS)
    {
        // Menus and game over
        if (_MenuMusicEnabled)
            Audio_PlayMenuMusic(_Audio);
        else
            Audio_StopMusic(_Audio);
    }
}

/* Event system integration - event callbacks */

static void OnFoodEaten(const Event *_Event, void *_UserData)
{
    (void)_Event; // Unused
    AudioSystem *audio = (AudioSystem*)_UserData;
    Audio_PlaySound(audio, SOUND_EAT);
}

static void OnSnakeDied(const Event *_Event, void *_UserData)
{
    (void)_Event; // Unused
    AudioSystem *audio = (AudioSystem*)_UserData;
    Audio_PlaySound(audio, SOUND_DEATH);
}

void Audio_SubscribeToEvents(AudioSystem *_Audio, EventDispatcher *_Dispatcher)
{
    if (!_Dispatcher || !_Audio)
        return;

    // Subscribe to food eaten event
    EventSystem_Subscribe(_Dispatcher, EVENT_FOOD_EATEN, OnFoodEaten, _Audio);

    // Subscribe to snake died event
    EventSystem_Subscribe(_Dispatcher, EVENT_SNAKE_DIED, OnSnakeDied, _Audio);

    printf("[Audio] Subscribed to game events\n");
}
