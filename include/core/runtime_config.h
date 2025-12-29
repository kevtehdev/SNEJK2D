#ifndef __RUNTIME_CONFIG_H_
#define __RUNTIME_CONFIG_H_

#include <stdbool.h>
#include "../utils/logger.h"

/* Runtime Configuration - allows changing game parameters at runtime */
/* This complements the compile-time constants in config.h */

typedef struct {
    /* Window & Display */
    int cellSize;
    int gridWidth;
    int gridHeight;
    int windowWidth;
    int windowHeight;

    /* Gameplay */
    int initialSnakeLength;
    int baseSpeed;
    int minSpeed;
    int speedIncrease;

    /* Combo System */
    int comboTimeoutMin;
    int comboTimeoutMax;
    int comboTier2;
    int comboTier3;
    int comboTier4;
    int comboTier5;

    /* Power-Ups */
    int powerUpScoreMultRate;
    int powerUpSpeedBoostRate;
    int powerUpGoldenAppleRate;
    int powerUpSpeedBoostDuration;
    int powerUpScoreMultDuration;
    int powerUpModeCount;
    int powerUpDecayTime;
    int powerUpDecayWarning;

    /* Visual */
    int backgroundOverlayAlpha;
    bool showGrid;
    bool showFPS;

    /* Network */
    char studentNumber[8];

} RuntimeConfig;

/* Global runtime config instance */
extern RuntimeConfig g_RuntimeConfig;

/* Set logger for runtime config messages (optional) */
void RuntimeConfig_SetLogger(Logger *_Logger);

/* Initialize runtime config with defaults from config.h */
void RuntimeConfig_Init(RuntimeConfig *_Config);

/* Load config from file (optional) */
bool RuntimeConfig_Load(RuntimeConfig *_Config, const char *_FilePath);

/* Save config to file */
bool RuntimeConfig_Save(RuntimeConfig *_Config, const char *_FilePath);

/* Reset to defaults */
void RuntimeConfig_Reset(RuntimeConfig *_Config);

/* Print current configuration */
void RuntimeConfig_Print(RuntimeConfig *_Config);

#endif
