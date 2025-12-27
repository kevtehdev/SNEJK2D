#include "../include/scoreboard.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../src/mpapi/c_client/libs/jansson/jansson.h"

#define SCOREBOARD_FILE "scoreboard.json"

void Scoreboard_Init(Scoreboard *_Scoreboard)
{
    _Scoreboard->count = 0;
    for (int i = 0; i < MAX_SCOREBOARD_ENTRIES; i++)
    {
        _Scoreboard->entries[i].score = 0;
        _Scoreboard->entries[i].name[0] = '\0';
        _Scoreboard->entries[i].timestamp = 0;
    }
}

bool Scoreboard_Load(Scoreboard *_Scoreboard)
{
    Scoreboard_Init(_Scoreboard);

    FILE *filePointer = fopen(SCOREBOARD_FILE, "r");
    if (!filePointer)
    {
        // File doesn't exist yet, not an error
        return true;
    }

    json_error_t error;
    json_t *root = json_loadf(filePointer, 0, &error);
    fclose(filePointer);

    if (!root)
    {
        fprintf(stderr, "Failed to parse scoreboard: %s\n", error.text);
        return false;
    }

    json_t *entriesArray = json_object_get(root, "entries");
    if (!json_is_array(entriesArray))
    {
        json_decref(root);
        return false;
    }

    size_t count = json_array_size(entriesArray);
    if (count > MAX_SCOREBOARD_ENTRIES)
        count = MAX_SCOREBOARD_ENTRIES;

    for (size_t i = 0; i < count; i++)
    {
        json_t *entry = json_array_get(entriesArray, i);
        json_t *scoreJson = json_object_get(entry, "score");
        json_t *nameJson = json_object_get(entry, "name");
        json_t *timestampJson = json_object_get(entry, "timestamp");

        if (json_is_integer(scoreJson) && json_is_string(nameJson))
        {
            _Scoreboard->entries[i].score = json_integer_value(scoreJson);
            strncpy(_Scoreboard->entries[i].name, json_string_value(nameJson), 31);
            _Scoreboard->entries[i].name[31] = '\0';

            if (json_is_integer(timestampJson))
                _Scoreboard->entries[i].timestamp = json_integer_value(timestampJson);
            else
                _Scoreboard->entries[i].timestamp = 0;

            _Scoreboard->count++;
        }
    }

    json_decref(root);
    return true;
}

bool Scoreboard_Save(Scoreboard *_Scoreboard)
{
    json_t *root = json_object();
    json_t *entriesArray = json_array();

    for (int i = 0; i < _Scoreboard->count; i++)
    {
        json_t *entry = json_object();
        json_object_set_new(entry, "score", json_integer(_Scoreboard->entries[i].score));
        json_object_set_new(entry, "name", json_string(_Scoreboard->entries[i].name));
        json_object_set_new(entry, "timestamp", json_integer(_Scoreboard->entries[i].timestamp));
        json_array_append_new(entriesArray, entry);
    }

    json_object_set_new(root, "entries", entriesArray);

    FILE *filePointer = fopen(SCOREBOARD_FILE, "w");
    if (!filePointer)
    {
        json_decref(root);
        fprintf(stderr, "Failed to open scoreboard file for writing\n");
        return false;
    }

    int result = json_dumpf(root, filePointer, JSON_INDENT(2));
    fclose(filePointer);
    json_decref(root);

    return (result == 0);
}

bool Scoreboard_AddScore(Scoreboard *_Scoreboard, int _Score, const char *_Name)
{
    // Find insertion position
    int insertPos = -1;
    for (int i = 0; i < _Scoreboard->count; i++)
    {
        if (_Score > _Scoreboard->entries[i].score)
        {
            insertPos = i;
            break;
        }
    }

    // Score doesn't make the top 10
    if (insertPos == -1 && _Scoreboard->count >= MAX_SCOREBOARD_ENTRIES)
        return false;

    // Score goes at the end
    if (insertPos == -1)
    {
        insertPos = _Scoreboard->count;
    }
    // Shift entries down
    else
    {
        int moveCount = _Scoreboard->count - insertPos;
        if (_Scoreboard->count >= MAX_SCOREBOARD_ENTRIES)
            moveCount = MAX_SCOREBOARD_ENTRIES - insertPos - 1;

        for (int i = moveCount; i > 0; i--)
        {
            _Scoreboard->entries[insertPos + i] = _Scoreboard->entries[insertPos + i - 1];
        }
    }

    // Insert new entry
    _Scoreboard->entries[insertPos].score = _Score;
    strncpy(_Scoreboard->entries[insertPos].name, _Name, 31);
    _Scoreboard->entries[insertPos].name[31] = '\0';
    _Scoreboard->entries[insertPos].timestamp = (unsigned int)time(NULL);

    if (_Scoreboard->count < MAX_SCOREBOARD_ENTRIES)
        _Scoreboard->count++;

    return true;
}

int Scoreboard_GetRank(Scoreboard *_Scoreboard, int _Score)
{
    for (int i = 0; i < _Scoreboard->count; i++)
    {
        if (_Scoreboard->entries[i].score == _Score)
            return i + 1;  // 1-based rank
    }
    return 0;  // Not found
}
