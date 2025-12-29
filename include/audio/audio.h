#ifndef __AUDIO_H_
#define __AUDIO_H_

#include <stdbool.h>

typedef enum
{
    SOUND_EAT,
    SOUND_DEATH,
    SOUND_MENU_SELECT,
    SOUND_COUNTDOWN,
    SOUND_CHAT,
    SOUND_COUNT
} SoundType;

typedef struct
{
    void *audioDevice;
    void *sounds[SOUND_COUNT];
    void *menuMusic;
    void *singleplayerMusic;
    void *multiplayerMusic;
    void *currentMusic; // Track currently playing music
    int nextChannel;    // Channel rotation counter for sound effects
    bool initialized;
    bool musicEnabled;
    bool sfxEnabled;
    float masterVolume;
} AudioSystem;

bool Audio_Init(AudioSystem *_Audio);
void Audio_Cleanup(AudioSystem *_Audio);

void Audio_PlaySound(AudioSystem *_Audio, SoundType _Sound);
void Audio_StopSound(AudioSystem *_Audio, SoundType _Sound);

void Audio_SetVolume(AudioSystem *_Audio, float _Volume);
void Audio_IncreaseVolume(AudioSystem *_Audio);
void Audio_DecreaseVolume(AudioSystem *_Audio);
void Audio_ToggleMusic(AudioSystem *_Audio);
void Audio_ToggleSfx(AudioSystem *_Audio);

void Audio_PlayMenuMusic(AudioSystem *_Audio);
void Audio_PlayGameMusic(AudioSystem *_Audio);
void Audio_PlayMultiplayerMusic(AudioSystem *_Audio);
void Audio_StopMusic(AudioSystem *_Audio);

// State-based music handling (uses int for state enums to avoid circular dependencies)
void Audio_UpdateMusicForState(AudioSystem *_Audio, int _GameState, int _MpState, bool _MenuMusicEnabled, bool _GameMusicEnabled);

// Event system integration (forward declaration to avoid circular dependency)
typedef struct EventDispatcher EventDispatcher;
void Audio_SubscribeToEvents(AudioSystem *_Audio, EventDispatcher *_Dispatcher);

#endif
