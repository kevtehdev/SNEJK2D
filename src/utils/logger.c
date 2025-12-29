#include "../../include/utils/logger.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>

/* ANSI color codes for console output */
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_DEBUG   "\x1b[36m"  // Cyan
#define ANSI_COLOR_INFO    "\x1b[32m"  // Green
#define ANSI_COLOR_WARNING "\x1b[33m"  // Yellow
#define ANSI_COLOR_ERROR   "\x1b[31m"  // Red

/* Log level names */
static const char* LOG_LEVEL_NAMES[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR"
};

/* Log level colors */
static const char* LOG_LEVEL_COLORS[] = {
    ANSI_COLOR_DEBUG,
    ANSI_COLOR_INFO,
    ANSI_COLOR_WARNING,
    ANSI_COLOR_ERROR
};

bool Logger_Init(Logger *_Logger, const char *_LogFilePath, LogLevel _MinLevel, LogOutput _Output)
{
    if (!_Logger)
        return false;

    memset(_Logger, 0, sizeof(Logger));

    _Logger->minLevel = _MinLevel;
    _Logger->output = _Output;
    _Logger->includeTimestamp = true;
    _Logger->includeLevel = true;
    _Logger->coloredOutput = true;
    _Logger->logFile = NULL;

    // Open log file if file output is enabled
    if ((_Output & LOG_OUTPUT_FILE) && _LogFilePath)
    {
        strncpy(_Logger->logFilePath, _LogFilePath, sizeof(_Logger->logFilePath) - 1);
        _Logger->logFile = fopen(_LogFilePath, "a");  // Append mode
        if (!_Logger->logFile)
        {
            fprintf(stderr, "[Logger] Failed to open log file: %s\n", _LogFilePath);
            return false;
        }

        // Write session separator
        fprintf(_Logger->logFile, "\n========== New Session ==========\n");
        fflush(_Logger->logFile);
    }

    printf("[Logger] Initialized (level: %s, output: %s)\n",
           LOG_LEVEL_NAMES[_MinLevel],
           (_Output & LOG_OUTPUT_BOTH) == LOG_OUTPUT_BOTH ? "console+file" :
           (_Output & LOG_OUTPUT_FILE) ? "file" : "console");

    return true;
}

void Logger_Shutdown(Logger *_Logger)
{
    if (!_Logger)
        return;

    // Print statistics before shutdown
    if (_Logger->output & LOG_OUTPUT_CONSOLE)
    {
        Logger_PrintStats(_Logger);
    }

    // Close log file
    if (_Logger->logFile)
    {
        fprintf(_Logger->logFile, "========== Session End ==========\n\n");
        fclose(_Logger->logFile);
        _Logger->logFile = NULL;
    }

    printf("[Logger] Shutdown complete\n");
}

/* Internal helper to format and write log message */
static void LoggerWriteMessage(Logger *_Logger, LogLevel _Level, const char *_Format, va_list _Args)
{
    if (_Level < _Logger->minLevel)
        return;

    char buffer[1024];
    char timestamp[32] = "";
    int offset = 0;

    // Add timestamp
    if (_Logger->includeTimestamp)
    {
        time_t now = time(NULL);
        struct tm *timeInfo = localtime(&now);
        strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", timeInfo);
    }

    // Format message
    vsnprintf(buffer, sizeof(buffer), _Format, _Args);

    // Write to console
    if (_Logger->output & LOG_OUTPUT_CONSOLE)
    {
        if (_Logger->coloredOutput && _Logger->includeLevel)
        {
            printf("%s%s %s%s %s%s\n",
                   _Logger->includeTimestamp ? timestamp : "",
                   _Logger->includeTimestamp ? "" : "",
                   LOG_LEVEL_COLORS[_Level],
                   _Logger->includeLevel ? LOG_LEVEL_NAMES[_Level] : "",
                   ANSI_COLOR_RESET,
                   buffer);
        }
        else
        {
            printf("%s %s %s\n",
                   _Logger->includeTimestamp ? timestamp : "",
                   _Logger->includeLevel ? LOG_LEVEL_NAMES[_Level] : "",
                   buffer);
        }
    }

    // Write to file
    if ((_Logger->output & LOG_OUTPUT_FILE) && _Logger->logFile)
    {
        fprintf(_Logger->logFile, "%s %s %s\n",
                _Logger->includeTimestamp ? timestamp : "",
                _Logger->includeLevel ? LOG_LEVEL_NAMES[_Level] : "",
                buffer);
        fflush(_Logger->logFile);  // Ensure it's written immediately
    }

    // Update statistics
    _Logger->logCount[_Level]++;
}

void Logger_Log(Logger *_Logger, LogLevel _Level, const char *_Format, ...)
{
    if (!_Logger || !_Format)
        return;

    va_list args;
    va_start(args, _Format);
    LoggerWriteMessage(_Logger, _Level, _Format, args);
    va_end(args);
}

void Logger_Debug(Logger *_Logger, const char *_Format, ...)
{
    if (!_Logger || !_Format)
        return;

    va_list args;
    va_start(args, _Format);
    LoggerWriteMessage(_Logger, LOG_LEVEL_DEBUG, _Format, args);
    va_end(args);
}

void Logger_Info(Logger *_Logger, const char *_Format, ...)
{
    if (!_Logger || !_Format)
        return;

    va_list args;
    va_start(args, _Format);
    LoggerWriteMessage(_Logger, LOG_LEVEL_INFO, _Format, args);
    va_end(args);
}

void Logger_Warning(Logger *_Logger, const char *_Format, ...)
{
    if (!_Logger || !_Format)
        return;

    va_list args;
    va_start(args, _Format);
    LoggerWriteMessage(_Logger, LOG_LEVEL_WARNING, _Format, args);
    va_end(args);
}

void Logger_Error(Logger *_Logger, const char *_Format, ...)
{
    if (!_Logger || !_Format)
        return;

    va_list args;
    va_start(args, _Format);
    LoggerWriteMessage(_Logger, LOG_LEVEL_ERROR, _Format, args);
    va_end(args);
}

void Logger_SetMinLevel(Logger *_Logger, LogLevel _Level)
{
    if (!_Logger)
        return;

    _Logger->minLevel = _Level;
    printf("[Logger] Minimum log level set to: %s\n", LOG_LEVEL_NAMES[_Level]);
}

void Logger_SetOutput(Logger *_Logger, LogOutput _Output)
{
    if (!_Logger)
        return;

    _Logger->output = _Output;
}

void Logger_EnableTimestamp(Logger *_Logger, bool _Enable)
{
    if (!_Logger)
        return;

    _Logger->includeTimestamp = _Enable;
}

void Logger_EnableColors(Logger *_Logger, bool _Enable)
{
    if (!_Logger)
        return;

    _Logger->coloredOutput = _Enable;
}

void Logger_PrintStats(Logger *_Logger)
{
    if (!_Logger)
        return;

    printf("\n[Logger] Statistics:\n");
    printf("  DEBUG:   %u\n", _Logger->logCount[LOG_LEVEL_DEBUG]);
    printf("  INFO:    %u\n", _Logger->logCount[LOG_LEVEL_INFO]);
    printf("  WARNING: %u\n", _Logger->logCount[LOG_LEVEL_WARNING]);
    printf("  ERROR:   %u\n", _Logger->logCount[LOG_LEVEL_ERROR]);
    printf("  Total:   %u\n",
           _Logger->logCount[LOG_LEVEL_DEBUG] +
           _Logger->logCount[LOG_LEVEL_INFO] +
           _Logger->logCount[LOG_LEVEL_WARNING] +
           _Logger->logCount[LOG_LEVEL_ERROR]);
}

void Logger_ClearStats(Logger *_Logger)
{
    if (!_Logger)
        return;

    memset(_Logger->logCount, 0, sizeof(_Logger->logCount));
}

void Logger_Flush(Logger *_Logger)
{
    if (!_Logger)
        return;

    if (_Logger->logFile)
    {
        fflush(_Logger->logFile);
    }
}
