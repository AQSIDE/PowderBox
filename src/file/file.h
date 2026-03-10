#ifndef FILE_H
#define FILE_H

#include "raylib.h"
#include "map.h"

#define MAX_MAPS 256
#define BASE_MAP_DIR "maps"

typedef struct {
    char name[MAX_MAP_NAME];
    char path[128];
    long long timestamp;
    Texture2D preview;
    int width;
    int height;
} MapInfo;

typedef struct {
    MapInfo maps[MAX_MAPS];
    int mapCount;
} MapList;

void loadMapList(MapList* list, const char* folder);
void saveMapToFolder(Map* map, char* name, const char* folder);
MapFile* loadMapFromPath(const char* path);
void deleteMapFile(const char* path);

#endif