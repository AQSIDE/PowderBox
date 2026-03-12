#ifndef FILE_H
#define FILE_H

#include "raylib.h"
#include "map.h"

#define MAX_MAPS 256
#define BASE_MAP_DIR "maps"

typedef struct {
    MapInfo maps[MAX_MAPS];
    int mapCount;
} MapList;

const char* formatFileSize(size_t bytes);
void loadMapList(MapList* list, const char* folder);
void saveMapToFolder(Map* map, char* name, const char* folder);
MapFile* loadMapFromPath(const char* path);
void deleteMapFile(const char* path);

#endif