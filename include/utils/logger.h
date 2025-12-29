#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <stdbool.h>
#include <stdio.h>

/* Log levels */
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_COUNT
} LogLevel;

/* Log output targets */
typedef enum {
    LOG_OUTPUT_CONSOLE = 1 << 0,
    LOG_OUTPUT_FILE = 1 << 1,
    LOG_OUTPUT_BOTH = LOG_OUTPUT_CONSOLE | LOG_OUTPUT_FILE
} LogOutput;

/* Logger configuration */
typedef struct {
    LogLevel minLevel;         // Minimum level to log
    LogOutput output;          // Where to output logs
    FILE *logFile;             // File handle for file logging
    char logFilePath[256];     // Path to log file
    bool includeTimestamp;     // Include timestamp in logs
    bool includeLevel;         // Include log level in logs
    bool coloredOutput;        // Use ANSI colors for console output
    unsigned int logCount[LOG_LEVEL_COUNT]; // Count of logs per level
} Logger;

/* Initialize logger */
bool Logger_Init(Logger *_Logger, const char *_LogFilePath, LogLevel _MinLevel, LogOutput _Output);

/* Shutdown logger */
void Logger_Shutdown(Logger *_Logger);

/* Log functions */
void Logger_Debug(Logger *_Logger, const char *_Format, ...);
void Logger_Info(Logger *_Logger, const char *_Format, ...);
void Logger_Warning(Logger *_Logger, const char *_Format, ...);
void Logger_Error(Logger *_Logger, const char *_Format, ...);

/* Generic log function */
void Logger_Log(Logger *_Logger, LogLevel _Level, const char *_Format, ...);

/* Configuration */
void Logger_SetMinLevel(Logger *_Logger, LogLevel _Level);
void Logger_SetOutput(Logger *_Logger, LogOutput _Output);
void Logger_EnableTimestamp(Logger *_Logger, bool _Enable);
void Logger_EnableColors(Logger *_Logger, bool _Enable);

/* Statistics */
void Logger_PrintStats(Logger *_Logger);
void Logger_ClearStats(Logger *_Logger);

/* Utility - flush logs to file */
void Logger_Flush(Logger *_Logger);

#endif
