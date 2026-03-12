#include "file.h"
#include "particle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

const char* formatFileSize(size_t bytes) {
    static char buffer[32];
    if (bytes < 1024) {
        snprintf(buffer, sizeof(buffer), "%zu B", bytes);
    }
    else if (bytes < 1024 * 1024) {
        snprintf(buffer, sizeof(buffer), "%.2f KB", bytes / 1024.0f);
    }
    else {
        snprintf(buffer, sizeof(buffer), "%.2f MB", bytes / (1024.0f * 1024.0f));
    }
    return buffer;
}

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

size_t loadMapFile(MapFile* mFile, const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) return 0;

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char magic[4];
    uint16_t width, height;

    if (fread(magic, 1, 3, file) != 3) {
        fclose(file);
        return 0;
    }

    magic[3] = '\0';

    if (strcmp(magic, "PWB")) {
        fclose(file);
        return 0;
    }

    long long timestamp;
    uint32_t activePixels = 0;

    if (!fread(&width, sizeof(uint16_t), 1, file)) {
        fclose(file);
        return 0;
    }

    if (!fread(&height, sizeof(uint16_t), 1, file)) {
        fclose(file);
        return 0;
    }

    if (width > 4096 || height > 4096) {
        fclose(file);
        return 0;
    }

    if (!fread(&activePixels, sizeof(uint32_t), 1, file)) {
        fclose(file);
        return 0;
    }

    if (!fread(&timestamp, sizeof(long long), 1, file)) {
        fclose(file);
        return 0;
    }

    mFile->width = width;
    mFile->height = height;
    mFile->timestamp = timestamp;

    strcpy(mFile->magic, magic);

    size_t total = width * height;
    mFile->particles = malloc(total * sizeof(ParticleFile));
    if (!mFile->particles) {
        fclose(file);
        return 0;
    }

    for (size_t i = 0; i < total; i++) {
        Particle p = createParticleFromType(EMPTY);
        ParticleFile* pFile = &mFile->particles[i];

        pFile->type = p.type;
        pFile->temperature = p.temperature;
    }

    for (uint32_t i = 0; i < activePixels; i++) {
        ParticleFile p;
        if (fread(&p, sizeof(ParticleFile), 1, file)) {
            if (p.x < mFile->width && p.y < mFile->height) {
                mFile->particles[p.y * mFile->width + p.x] = p;
            }
        }
    }

    fclose(file);
    return size;
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

        MapFile* mFile = malloc(sizeof(MapFile));
        if (!mFile) continue;

        size_t size = loadMapFile(mFile, fullPath);
        if (size <= 0) {
            free(mFile);
            continue;
        }

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
        info->fileSize = size;

        info->preview = createTextureFromParticles(mFile->particles, mFile->width, mFile->height);

        free(mFile->particles);
        free(mFile);

        count++;
    }

    list->mapCount = count;
    closedir(dir);
}

MapFile* loadMapFromPath(const char* path) {
    MapFile* mFile = malloc(sizeof(MapFile));
    if (!mFile) return NULL;

    size_t size = loadMapFile(mFile, path);
    if (size > 0) return mFile;

    free(mFile);
    return NULL;
}

void saveMapToFolder(Map* map, char* name, const char* folder) {
    if (!DirectoryExists(folder)) {
        MakeDirectory(folder);
    }

    char path[256];
    snprintf(path, sizeof(path), "%s/%s.pwb", folder, name);

    FILE* file = fopen(path, "wb");

    int total = map->width * map->height;
    int activeCount = 0;
    for (int i = 0; i < total; i++) {
        if (map->particles[i].type != EMPTY) activeCount++;
    }

    MapFile fileMap;
    strcpy(fileMap.magic, "PWB");
    fileMap.width = map->width;
    fileMap.height = map->height;
    fileMap.activePixels = activeCount;

    fwrite(fileMap.magic, sizeof(char), 3, file);
    fwrite(&fileMap.width, sizeof(uint16_t), 1, file);
    fwrite(&fileMap.height, sizeof(uint16_t), 1, file);
    fwrite(&fileMap.activePixels, sizeof(uint32_t), 1, file);
    time_t now = time(NULL);
    fwrite(&now, sizeof(long long), 1, file);

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width;x++) {
            Particle* p = &map->particles[y * map->width + x];
            if (p->type == EMPTY) continue;

            ParticleFile pFile;
            pFile.x = x;
            pFile.y = y;
            pFile.type = p->type;
            pFile.temperature = p->temperature;
            fwrite(&pFile, sizeof(ParticleFile), 1, file);
        }
    }

    fclose(file);
}

void deleteMapFile(const char* path) {
    remove(path);
}