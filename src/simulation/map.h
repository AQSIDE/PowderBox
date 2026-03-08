#include "raylib.h"
#include "particle.h"

#define EMPTY_COLOR      (Color){ 50, 50, 50, 255 }
#define EMPTY_DARK_COLOR (Color){ 42, 42, 42, 255 }

typedef struct Map {
    int width;
    int height;
    int filledPixels;
    __int8 isDirty;
    __int8 isDirtyTemp;
    Particle* particles;
    Color* colorBuffer;
    Color* colorTempBuffer;
    Texture2D backgroundTexture;
    Texture2D pixelsTexture;
    Texture2D temperatureTexture;
} Map;

typedef struct NeighborList {
    int count;
    int indices[128];
} NeighborList;

void getNeighbors(Map* map, int x, int y, int radius, NeighborList* out);
Color getColorFromTemperature(uint16_t temp, int8_t glow);

Vector2 getCoordFromIndex(Map* map, int index);

void movePixel(Map* map, uint32_t currentFrame, int fromX, int fromY, int toX, int toY);
void swapPixels(Map* map, uint32_t currentFrame, int aX, int aY, int bX, int bY);
void setPixel(Map* map, int x, int y, Particle particle);
void clearPixel(Map* map, int x, int y);
Particle* getPixel(Map* map, int x, int y);
void setTemperature(Map* map, int x, int y, uint16_t temp);

void freeMap(Map* map);
Map createMap(int width, int height);
void clearMap(Map* map);
void drawMap(Map* map);
int isInBounds(Map* map, int x, int y);
Vector2 getPointInMap(float x, float y, Map* map);
int getIndex(Map* map, int x, int y);