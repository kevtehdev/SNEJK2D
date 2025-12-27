#ifndef __SETTINGS_H_
#define __SETTINGS_H_

#include <stdbool.h>

typedef struct
{
    // Audio settings
    bool menuMusicEnabled; // true = ON, false = OFF
    bool gameMusicEnabled; // true = ON, false = OFF

    // Visual settings
    int gridAlpha;     // 0 = off, 1-10 = transparency level (default 2)
    float brightness;  // 0.5 to 2.0 (default 1.0)
    bool comboEffects; // true = HUD effects ON, false = static HUD (default true)
} Settings;

/* Initialize settings with defaults */
void Settings_Init(Settings *_Settings);

/* Load settings from file */
bool Settings_Load(Settings *_Settings);

/* Save settings to file */
bool Settings_Save(Settings *_Settings);

#endif
