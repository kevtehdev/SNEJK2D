#include "../include/settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SETTINGS_FILE "settings.conf"

void Settings_Init(Settings *_Settings)
{
    // Default values
    _Settings->menuMusicEnabled = true;
    _Settings->gameMusicEnabled = true;
    _Settings->gridAlpha = 2;
    _Settings->brightness = 1.0f;
}

bool Settings_Load(Settings *_Settings)
{
    FILE *file = fopen(SETTINGS_FILE, "r");
    if (!file)
    {
        // No settings file, use defaults
        Settings_Init(_Settings);
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char key[64];
        char value[64];

        if (sscanf(line, "%63[^=]=%63s", key, value) == 2)
        {
            if (strcmp(key, "menuMusicEnabled") == 0)
            {
                _Settings->menuMusicEnabled = (strcmp(value, "true") == 0 || strcmp(value, "1") == 0);
            }
            else if (strcmp(key, "gameMusicEnabled") == 0)
            {
                _Settings->gameMusicEnabled = (strcmp(value, "true") == 0 || strcmp(value, "1") == 0);
            }
            else if (strcmp(key, "gridAlpha") == 0)
            {
                _Settings->gridAlpha = atoi(value);
            }
            else if (strcmp(key, "brightness") == 0)
            {
                _Settings->brightness = atof(value);
            }
        }
    }

    fclose(file);
    return true;
}

bool Settings_Save(Settings *_Settings)
{
    FILE *file = fopen(SETTINGS_FILE, "w");
    if (!file)
    {
        return false;
    }

    fprintf(file, "menuMusicEnabled=%s\n", _Settings->menuMusicEnabled ? "true" : "false");
    fprintf(file, "gameMusicEnabled=%s\n", _Settings->gameMusicEnabled ? "true" : "false");
    fprintf(file, "gridAlpha=%d\n", _Settings->gridAlpha);
    fprintf(file, "brightness=%.2f\n", _Settings->brightness);

    fclose(file);
    return true;
}
