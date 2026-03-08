#include "map.h"
#include <stdlib.h>
#include <stdio.h>

Color getColorFromTemperature(uint16_t temp, int8_t glow) {
    const uint16_t T_ABSOLUTE_ZERO = 0;
    const uint16_t T_FREEZING = 273;
    const uint16_t T_AMBIENT = 293;
    const uint16_t T_HOT = 500;
    const uint16_t T_LAVA = 1200;
    const uint16_t T_SUN = 3000;

    Color result;
    Color cold = { 0, 50, 150, 150 };
    Color ice = { 0, 121, 241, 100 };

    if (temp < T_FREEZING) {
        float t = (float)temp / T_FREEZING;
        result = ColorLerp(cold, ice, t);
    }
    else if (temp < 323) {
        if (temp < T_AMBIENT) {
            float t = (float)(temp - T_FREEZING) / (T_AMBIENT - T_FREEZING);
            result = ColorLerp((Color) { 0, 121, 241, 100 }, (Color) { 255, 255, 255, 0 }, t);
        }
        else {
            float t = (float)(temp - T_AMBIENT) / (323 - T_AMBIENT);
            result = ColorLerp((Color) { 255, 255, 255, 0 }, (Color) { 200, 100, 0, 50 }, t);
        }
    }
    else if (temp < 800) {
        float t = (float)(temp - 323) / (800 - 323);
        result = ColorLerp((Color) { 255, 100, 0, 100 }, (Color) { 255, 0, 0, 180 }, t);
    }
    else if (temp < 1500) {
        float t = (float)(temp - 800) / (1500 - 800);
        result = ColorLerp((Color) { 255, 0, 0, 180 }, (Color) { 255, 255, 100, 220 }, t);
    }
    else {
        float t = (float)(temp - 1500) / (T_SUN - 1500);
        if (t > 1.0f) t = 1.0f;
        result = ColorLerp((Color) { 255, 255, 100, 220 }, (Color) { 255, 255, 255, 255 }, t);
    }

    float multiplier = (float)((uint8_t)glow) / 255.0f;
    result.a = (unsigned char)(result.a * multiplier);

    return result;
}

Vector2 getCoordFromIndex(Map* map, int index) {
    int x = index % map->width;
    int y = index / map->width;

    return (Vector2) { (float)x, (float)y };
}

void wakeNeighbors(Map* map, int x, int y) {
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            Particle* p = getPixel(map, x + dx, y + dy);
            if (p != NULL) p->isSleeping = 0;
        }
    }
}

void getNeighbors(Map* map, int x, int y, int radius, NeighborList* out) {
    out->count = 0;

    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx == 0 && dy == 0) continue;

            int nx = x + dx;
            int ny = y + dy;

            Particle* p = getPixel(map, nx, ny);
            if (p != NULL) {
                out->indices[out->count++] = ny * map->width + nx;
                if (out->count >= 128) return;
            }
        }
    }
}

int getIndex(Map* map, int x, int y) {
    return y * map->width + x;
}

int isInBounds(Map* map, int x, int y) {
    return x >= 0 && x < map->width && y >= 0 && y < map->height;
}

void updateColorBuffer(Map* map) {
    for (int i = 0; i < map->width * map->height; i++) {
        map->colorBuffer[i] = map->particles[i].color;
    }
}

Map createMap(int width, int height)
{
    Map map = { 0 };
    printf("START CREATING MAP\n");

    map.width = width;
    map.height = height;
    map.isDirty = 1;
    map.isDirtyTemp = 1;

    int count = width * height;

    // --- allocate memory ---
    map.particles = malloc(sizeof(Particle) * count);
    map.colorBuffer = malloc(sizeof(Color) * count);
    map.colorTempBuffer = malloc(sizeof(Color) * count);

    printf("MAP ARRAYS ALLOCATED\n");

    if (!map.particles || !map.colorBuffer || !map.colorTempBuffer)
    {
        TraceLog(LOG_FATAL, "Failed to allocate map memory");
    }

    // --- initialize particles ---
    for (int i = 0; i < count; i++)
    {
        map.particles[i] = createParticleFromType(EMPTY);
    }

    printf("MAP PIXELS CREATED\n");

    updateColorBuffer(&map);

    // --- background image ---
    Image imgB = GenImageColor(width, height, BLANK);
    Color* pixelsBg = imgB.data;

    for (int y = 0; y < height; y++)
    {
        int row = y * width;

        for (int x = 0; x < width; x++)
        {
            if (x % 16 == 0 || y % 16 == 0)
                pixelsBg[row + x] = EMPTY_COLOR;
            else
                pixelsBg[row + x] = EMPTY_DARK_COLOR;
        }
    }

    printf("MAP BG CREATED\n");

    // --- temperature image ---
    Image imgT = GenImageColor(width, height, BLANK);
    Color* pixelsTemp = imgT.data;

    for (int i = 0; i < count; i++)
    {
        Color tempC = getColorFromTemperature(
            map.particles[i].temperature,
            map.particles[i].glowIntensity
        );

        pixelsTemp[i] = tempC;
        map.colorTempBuffer[i] = tempC;
    }

    printf("MAP TEMP CREATED\n");

    // --- particle image ---
    Image imgP = GenImageColor(width, height, BLANK);

    // --- textures ---
    map.pixelsTexture = LoadTextureFromImage(imgP);
    map.backgroundTexture = LoadTextureFromImage(imgB);
    map.temperatureTexture = LoadTextureFromImage(imgT);

    printf("MAP TEXTURES LOADED\n");

    // --- cleanup images ---
    UnloadImage(imgP);
    UnloadImage(imgB);
    UnloadImage(imgT);

    printf("MAP CREATED\n");

    return map;
}

void freeMap(Map* map)
{
    if (map->particles) free(map->particles);
    if (map->colorBuffer) free(map->colorBuffer);
    if (map->colorTempBuffer) free(map->colorTempBuffer);

    map->particles = NULL;
    map->colorBuffer = NULL;
    map->colorTempBuffer = NULL;

    if (map->backgroundTexture.id) UnloadTexture(map->backgroundTexture);
    if (map->temperatureTexture.id) UnloadTexture(map->temperatureTexture);
    if (map->pixelsTexture.id) UnloadTexture(map->pixelsTexture);
}

void clearMap(Map* map) {
    int pixelsCount = map->width * map->height;

    for (int i = 0; i < pixelsCount; i++) {
        map->particles[i] = createParticleFromType(EMPTY);
        map->colorBuffer[i] = getColorFromType(EMPTY);
        map->colorTempBuffer[i] = (Color){ 0 };
    }

    map->filledPixels = 0;
    map->isDirty = 1;
    map->isDirtyTemp = 1;
}

void setTemperature(Map* map, int x, int y, uint16_t temp) {
    if (!isInBounds(map, x, y)) return;

    int index = getIndex(map, x, y);
    Particle* p = &map->particles[index];
    if (p->type == EMPTY) return;

    p->temperature = temp;
    map->colorTempBuffer[index] = getColorFromTemperature(temp, p->glowIntensity);
    map->isDirtyTemp = 1;
}

void movePixel(Map* map, uint32_t currentFrame, int fromX, int fromY, int toX, int toY) {
    if (!isInBounds(map, fromX, fromY)
        || !isInBounds(map, toX, toY))
        return;

    int fromIndex = getIndex(map, fromX, fromY);
    int toIndex = getIndex(map, toX, toY);

    map->particles[toIndex] = map->particles[fromIndex];
    map->colorBuffer[toIndex] = map->colorBuffer[fromIndex];
    map->particles[toIndex].lastUpdateFrame = currentFrame;
    map->colorTempBuffer[toIndex] = getColorFromTemperature(map->particles[toIndex].temperature, map->particles[toIndex].glowIntensity);
    map->particles[toIndex].isSleeping = 0;

    Particle emptyP = createParticleFromType(EMPTY);

    map->particles[fromIndex] = emptyP;
    map->colorBuffer[fromIndex] = emptyP.color;
    map->colorTempBuffer[fromIndex] = getColorFromTemperature(emptyP.temperature, emptyP.glowIntensity);

    wakeNeighbors(map, fromX, fromY);
    wakeNeighbors(map, toX, toY);

    map->isDirty = 1;
    map->isDirtyTemp = 1;
}

void swapPixels(Map* map, uint32_t currentFrame, int aX, int aY, int bX, int bY) {
    if (!isInBounds(map, aX, aY)
        || !isInBounds(map, bX, bY))
        return;

    int aIndex = getIndex(map, aX, aY);
    int bIndex = getIndex(map, bX, bY);

    Particle tempA = map->particles[aIndex];
    Particle tempB = map->particles[bIndex];

    tempA.isSleeping = 0;
    tempB.isSleeping = 0;

    map->particles[aIndex] = tempB;
    map->colorBuffer[aIndex] = tempB.color;
    map->colorTempBuffer[aIndex] = getColorFromTemperature(tempB.temperature, tempB.glowIntensity);
    map->particles[aIndex].lastUpdateFrame = currentFrame;

    map->particles[bIndex] = tempA;
    map->colorBuffer[bIndex] = tempA.color;
    map->colorTempBuffer[bIndex] = getColorFromTemperature(tempA.temperature, tempA.glowIntensity);
    map->particles[bIndex].lastUpdateFrame = currentFrame;

    wakeNeighbors(map, aX, aY);
    wakeNeighbors(map, bX, bY);

    map->isDirty = 1;
    map->isDirtyTemp = 1;
}

void setPixel(Map* map, int x, int y, Particle particle) {
    if (!isInBounds(map, x, y)) return;

    int index = getIndex(map, x, y);
    Particle* oldParticle = &map->particles[index];

    int8_t wasEmpty = (oldParticle->type == EMPTY);
    int8_t isNewEmpty = (particle.type == EMPTY);

    if (wasEmpty && !isNewEmpty) {
        map->filledPixels++;
    }
    else if (!wasEmpty && isNewEmpty) {
        map->filledPixels--;
    }

    map->particles[index] = particle;
    map->colorBuffer[index] = particle.color;
    map->colorTempBuffer[index] = getColorFromTemperature(particle.temperature, particle.glowIntensity);

    particle.isSleeping = 0;

    wakeNeighbors(map, x, y);

    map->isDirty = 1;
    map->isDirtyTemp = 1;
}

void clearPixel(Map* map, int x, int y) {
    if (!isInBounds(map, x, y)) return;

    int index = getIndex(map, x, y);
    Particle* p = &map->particles[index];

    if (p->type != EMPTY) {
        *p = createParticleFromType(EMPTY);
        map->colorBuffer[index] = getColorFromType(EMPTY);
        map->colorTempBuffer[index] = getColorFromTemperature(p->temperature, p->glowIntensity);
        map->filledPixels--;

        wakeNeighbors(map, x, y);

        map->isDirty = 1;
        map->isDirtyTemp = 1;
    }
}

void drawMap(Map* map) {
    if (map->isDirty) {
        UpdateTexture(map->pixelsTexture, map->colorBuffer);
        map->isDirty = 0;
    }

    if (map->isDirtyTemp) {
        UpdateTexture(map->temperatureTexture, map->colorTempBuffer);
        map->isDirtyTemp = 0;
    }

    DrawRectangle(4, 4, map->width, map->height, (Color) { 10, 10, 15, 30 });

    DrawTexture(map->backgroundTexture, 0, 0, WHITE);
    DrawTexture(map->pixelsTexture, 0, 0, WHITE);

    BeginBlendMode(BLEND_ADDITIVE);
    DrawTexture(map->temperatureTexture, 0, 0, WHITE);
    EndBlendMode();

    DrawText("PowderBox", 1, -19, 20, (Color) { 0, 0, 0, 150 });
    DrawText("PowderBox", 0, -20, 20, (Color) { 200, 200, 200, 180 });
}

Particle* getPixel(Map* map, int x, int y) {
    if (!isInBounds(map, x, y)) {
        return NULL;
    }

    return &map->particles[getIndex(map, x, y)];
}

Vector2 getPointInMap(float x, float y, Map* map) {
    Vector2 result = { -1, -1 };

    if (x >= 0 && x < map->width && y >= 0 && y < map->height) {
        result.x = x;
        result.y = y;
    }

    return result;
}