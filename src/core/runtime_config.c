#include "../../include/core/runtime_config.h"
#include "../../include/config.h"
#include "../../include/utils/logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Global runtime config instance */
RuntimeConfig g_RuntimeConfig;

/* Optional logger for runtime config (can be NULL) */
static Logger *s_ConfigLogger = NULL;

void RuntimeConfig_SetLogger(Logger *_Logger)
{
    s_ConfigLogger = _Logger;
}

void RuntimeConfig_Init(RuntimeConfig *_Config)
{
    if (!_Config)
        return;

    /* Initialize with compile-time defaults from config.h */
    _Config->cellSize = CELL_SIZE;
    _Config->gridWidth = GRID_WIDTH;
    _Config->gridHeight = GRID_HEIGHT;
    _Config->windowWidth = WINDOW_WIDTH;
    _Config->windowHeight = WINDOW_HEIGHT;

    _Config->initialSnakeLength = INITIAL_SNAKE_LENGTH;
    _Config->baseSpeed = BASE_SPEED;
    _Config->minSpeed = MIN_SPEED;
    _Config->speedIncrease = SPEED_INCREASE;

    _Config->comboTimeoutMin = COMBO_TIMEOUT_MIN;
    _Config->comboTimeoutMax = COMBO_TIMEOUT_MAX;
    _Config->comboTier2 = COMBO_TIER_2;
    _Config->comboTier3 = COMBO_TIER_3;
    _Config->comboTier4 = COMBO_TIER_4;
    _Config->comboTier5 = COMBO_TIER_5;

    _Config->powerUpScoreMultRate = POWERUP_SCORE_MULT_RATE;
    _Config->powerUpSpeedBoostRate = POWERUP_SPEED_BOOST_RATE;
    _Config->powerUpGoldenAppleRate = POWERUP_GOLDEN_APPLE_RATE;
    _Config->powerUpSpeedBoostDuration = POWERUP_SPEED_BOOST_DURATION;
    _Config->powerUpScoreMultDuration = POWERUP_SCORE_MULT_DURATION;
    _Config->powerUpModeCount = POWERUP_MODE_COUNT;
    _Config->powerUpDecayTime = POWERUP_DECAY_TIME;
    _Config->powerUpDecayWarning = POWERUP_DECAY_WARNING;

    _Config->backgroundOverlayAlpha = BACKGROUND_OVERLAY_ALPHA;
    _Config->showGrid = false;
    _Config->showFPS = false;

    strncpy(_Config->studentNumber, STUDENT_NUMBER, sizeof(_Config->studentNumber) - 1);
    _Config->studentNumber[sizeof(_Config->studentNumber) - 1] = '\0';

    if (s_ConfigLogger)
        Logger_Info(s_ConfigLogger, "[RuntimeConfig] Initialized with defaults");
    else
        printf("[RuntimeConfig] Initialized with defaults\n");
}

bool RuntimeConfig_Load(RuntimeConfig *_Config, const char *_FilePath)
{
    if (!_Config || !_FilePath)
        return false;

    FILE *file = fopen(_FilePath, "r");
    if (!file)
    {
        if (s_ConfigLogger)
            Logger_Info(s_ConfigLogger, "[RuntimeConfig] No config file found at %s, using defaults", _FilePath);
        else
            printf("[RuntimeConfig] No config file found at %s, using defaults\n", _FilePath);
        return false;
    }

    // Simple key=value parser
    char line[256];
    int loaded = 0;

    while (fgets(line, sizeof(line), file))
    {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n')
            continue;

        char key[64], value[64];
        if (sscanf(line, "%63[^=]=%63s", key, value) == 2)
        {
            // Parse integer values
            if (strcmp(key, "cellSize") == 0)
                _Config->cellSize = atoi(value);
            else if (strcmp(key, "baseSpeed") == 0)
                _Config->baseSpeed = atoi(value);
            else if (strcmp(key, "minSpeed") == 0)
                _Config->minSpeed = atoi(value);
            else if (strcmp(key, "showGrid") == 0)
                _Config->showGrid = (strcmp(value, "true") == 0 || strcmp(value, "1") == 0);
            else if (strcmp(key, "showFPS") == 0)
                _Config->showFPS = (strcmp(value, "true") == 0 || strcmp(value, "1") == 0);

            loaded++;
        }
    }

    fclose(file);
    if (s_ConfigLogger)
        Logger_Info(s_ConfigLogger, "[RuntimeConfig] Loaded %d settings from %s", loaded, _FilePath);
    else
        printf("[RuntimeConfig] Loaded %d settings from %s\n", loaded, _FilePath);
    return loaded > 0;
}

bool RuntimeConfig_Save(RuntimeConfig *_Config, const char *_FilePath)
{
    if (!_Config || !_FilePath)
        return false;

    FILE *file = fopen(_FilePath, "w");
    if (!file)
    {
        if (s_ConfigLogger)
            Logger_Error(s_ConfigLogger, "[RuntimeConfig] Failed to save config to %s", _FilePath);
        else
            printf("[RuntimeConfig] Failed to save config to %s\n", _FilePath);
        return false;
    }

    fprintf(file, "# SNEJK2D Runtime Configuration\n");
    fprintf(file, "# Generated automatically\n\n");

    fprintf(file, "# Window & Display\n");
    fprintf(file, "cellSize=%d\n", _Config->cellSize);
    fprintf(file, "gridWidth=%d\n", _Config->gridWidth);
    fprintf(file, "gridHeight=%d\n", _Config->gridHeight);

    fprintf(file, "\n# Gameplay\n");
    fprintf(file, "baseSpeed=%d\n", _Config->baseSpeed);
    fprintf(file, "minSpeed=%d\n", _Config->minSpeed);
    fprintf(file, "speedIncrease=%d\n", _Config->speedIncrease);

    fprintf(file, "\n# Visual\n");
    fprintf(file, "showGrid=%s\n", _Config->showGrid ? "true" : "false");
    fprintf(file, "showFPS=%s\n", _Config->showFPS ? "true" : "false");

    fclose(file);
    if (s_ConfigLogger)
        Logger_Info(s_ConfigLogger, "[RuntimeConfig] Saved config to %s", _FilePath);
    else
        printf("[RuntimeConfig] Saved config to %s\n", _FilePath);
    return true;
}

void RuntimeConfig_Reset(RuntimeConfig *_Config)
{
    if (!_Config)
        return;

    if (s_ConfigLogger)
        Logger_Info(s_ConfigLogger, "[RuntimeConfig] Resetting to defaults");
    else
        printf("[RuntimeConfig] Resetting to defaults\n");
    RuntimeConfig_Init(_Config);
}

void RuntimeConfig_Print(RuntimeConfig *_Config)
{
    if (!_Config)
        return;

    printf("\n[RuntimeConfig] Current Configuration:\n");
    printf("  Cell Size: %d\n", _Config->cellSize);
    printf("  Grid: %dx%d\n", _Config->gridWidth, _Config->gridHeight);
    printf("  Window: %dx%d\n", _Config->windowWidth, _Config->windowHeight);
    printf("  Base Speed: %d ms\n", _Config->baseSpeed);
    printf("  Min Speed: %d ms\n", _Config->minSpeed);
    printf("  Show Grid: %s\n", _Config->showGrid ? "Yes" : "No");
    printf("  Show FPS: %s\n", _Config->showFPS ? "Yes" : "No");
    printf("  Student Number: %s\n", _Config->studentNumber);
    printf("\n");
}
