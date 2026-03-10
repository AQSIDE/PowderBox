#include "file.h"
#include "particle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

Texture2D createTextureFromParticles(ParticleFile* particles, int width, int height) {
    int scale = 4;
    int previewW = width / scale;
    int previewH = height / scale;

    Image previewImage = GenImageColor(previewW, previewH, WHITE);

    for (int y = 0; y < previewH; y++) {
        for (int x = 0; x < previewW; x++) {
            int origX = x * scale;
            int origY = y * scale;
            ParticleFile* p = &particles[origY * width + origX];

            Color color = getColorFromType(p->type);

            ((Color*)previewImage.data)[y * previewW + x] = color;
        }
    }

    Texture2D texture = LoadTextureFromImage(previewImage);
    UnloadImage(previewImage);

    return texture;
}

MapFile* loadMapFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) return NULL;

    char magic[6];
    uint16_t width, height;

    if (fread(magic, 1, 5, file) != 5) {
        fclose(file);
        return NULL;
    }

    magic[5] = '\0';

    if (strcmp(magic, "PWBOX") != 0) {
        fclose(file);
        return NULL;
    }

    long long timestamp;

    fread(&width, sizeof(uint16_t), 1, file);
    fread(&height, sizeof(uint16_t), 1, file);
    fread(&timestamp, sizeof(long long), 1, file);

    MapFile* mFile = malloc(sizeof(MapFile));
    if (!mFile) {
        fclose(file);
        return NULL;
    }

    mFile->width = width;
    mFile->height = height;
    mFile->timestamp = timestamp;

    strcpy(mFile->magic, magic);

    size_t count = width * height;
    mFile->particles = malloc(count * sizeof(ParticleFile));

    if (!mFile->particles || fread(mFile->particles, sizeof(ParticleFile), count, file) != count) {
        if (mFile->particles) free(mFile->particles);
        free(mFile);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return mFile;
}

void loadMapList(MapList* list, const char* folder) {
    DIR* dir = opendir(folder);
    if (!dir) return;

    struct dirent* entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL && count < MAX_MAPS) {
        if (entry->d_name[0] == '.') continue;

        char fullPath[256];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", folder, entry->d_name);

        MapFile* mFile = loadMapFile(fullPath);
        if (!mFile) continue;

        MapInfo* info = &list->maps[count];

        strncpy(info->name, entry->d_name, MAX_MAP_NAME);

        char* dot = strrchr(info->name, '.');
        if (dot != NULL) {
            *dot = '\0';
        }

        strncpy(info->path, fullPath, 128);
        info->width = mFile->width;
        info->height = mFile->height;
        info->timestamp = mFile->timestamp;

        info->preview = createTextureFromParticles(mFile->particles, mFile->width, mFile->height);

        free(mFile->particles);
        free(mFile);

        count++;
    }

    list->mapCount = count;
    closedir(dir);
}

MapFile* loadMapFromPath(const char* path) {
    return loadMapFile(path);
}

void saveMapToFolder(Map* map, char* name, const char* folder) {
    if (!DirectoryExists(folder)) {
        MakeDirectory(folder);
    }

    char path[256];
    snprintf(path, sizeof(path), "%s/%s.pwbox", folder, name);

    FILE* file = fopen(path, "wb");

    int count = map->width * map->height;

    MapFile fileMap;
    strcpy(fileMap.magic, "PWBOX");
    fileMap.width = map->width;
    fileMap.height = map->height;
    fileMap.particles = malloc(count * sizeof(ParticleFile));

    for (int i = 0; i < count; i++) {
        Particle* p = &map->particles[i];
        ParticleFile pFile;

        pFile.type = (uint16_t)p->type;
        pFile.temperature = (uint16_t)p->temperature;
        fileMap.particles[i] = pFile;
    }

    fwrite(fileMap.magic, sizeof(char), 5, file);
    fwrite(&fileMap.width, sizeof(uint16_t), 1, file);
    fwrite(&fileMap.height, sizeof(uint16_t), 1, file);
    time_t now = time(NULL);

    fwrite(&now, sizeof(long long), 1, file);
    fwrite(fileMap.particles, sizeof(ParticleFile), count, file);

    fclose(file);
    free(fileMap.particles);
}

void deleteMapFile(const char* path) {
    remove(path);
}