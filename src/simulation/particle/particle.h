#include "raylib.h"
#include <stdint.h>

#define SAND_COLOR (Color){194, 178, 128, 255}
#define WALL_COLOR (Color){ 100, 100, 105, 255 }
#define WATER_COLOR (Color){ 40, 100, 200, 255 }
#define ICE_COLOR   (Color){ 165, 230, 255, 255 } 
#define STEAM_COLOR (Color){ 220, 235, 245, 140 } 
#define FIRE_COLOR  (Color){ 255, 60, 0, 200 }
#define WOOD_COLOR (Color){ 101, 67, 33, 255 }
#define MAGMA_COLOR (Color){ 160, 20, 0, 255 }
#define BASALT_COLOR (Color){ 45, 45, 50, 255 }

typedef enum ParticleType {
    EMPTY = 0,
    SAND,
    WATER,
    WALL,
    ICE,
    STEAM,
    FIRE,
    WOOD,
    MAGMA,
    BASALT,
} ParticleType;

typedef enum ParticleFlags {
    P_NONE = 0,      // 00000000
    P_SOLID = 1 << 0, // 00000001
    P_POWDER = 1 << 1, // 00000010
    P_LIQUID = 1 << 2, // 00000100
    P_GAS = 1 << 3, // 00001000
} ParticleFlags;

typedef struct Particle {
    ParticleType type;
    Color color;
    uint16_t temperature;
    uint8_t glowIntensity;
    uint32_t lastUpdateFrame;
    int8_t isSleeping;
    float sleepingCounter;
    int8_t flags;
} Particle;

void shuffleDirections(Vector2* array, int length);

const char* getParticleNameFromType(ParticleType type);
int16_t getTemperatureFromType(ParticleType type);
Color getColorFromType(ParticleType type);

Particle createParticleFromType(ParticleType type);