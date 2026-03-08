#include "raylib.h"
#include "raymath.h"
#include "particle.h"

void shuffleDirections(Vector2* array, int length) {
    for (int i = length - 1; i > 0; i--) {
        int j = GetRandomValue(0, i);

        Vector2 temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

Color getRandomizedColor(Color base, int min, int max) {
    int variation = GetRandomValue(min, max);

    return (Color) {
        (unsigned char)Clamp(base.r + variation, 0, 255),
            (unsigned char)Clamp(base.g + variation, 0, 255),
            (unsigned char)Clamp(base.b + variation, 0, 255),
            base.a
    };
}

Color getColorFromType(ParticleType type) {
    switch (type)
    {
    case SAND:
        return getRandomizedColor(SAND_COLOR, -15, 15);
    case WALL:
        return WALL_COLOR;
    case WOOD:
        return getRandomizedColor(WOOD_COLOR, -15, 15);
    case ICE:
        return getRandomizedColor(ICE_COLOR, -15, 15);
    case STEAM:
        return getRandomizedColor(STEAM_COLOR, -15, 15);
    case WATER:
        return getRandomizedColor(WATER_COLOR, -15, 15);
    case FIRE:
        return getRandomizedColor(FIRE_COLOR, -15, 15);
    case MAGMA:
        return getRandomizedColor(MAGMA_COLOR, -15, 15);
    case BASALT:
        return getRandomizedColor(BASALT_COLOR, -15, 15);
    default:
        return (Color) { 0, 0, 0, 0 };
    }
}

const char* getParticleNameFromType(ParticleType type) {
    switch (type) {
    case SAND:  return "SAND";
    case WALL:  return "WALL";
    case WATER: return "WATER";
    case ICE: return "ICE";
    case STEAM: return "STEAM";
    case FIRE: return "FIRE";
    case WOOD: return "WOOD";
    case MAGMA: return "MAGMA";
    case BASALT: return "BASALT";
    case EMPTY: return "EMPTY";
    default:    return "UNKNOWN";
    }
}

int16_t getTemperatureFromType(ParticleType type) {
    switch (type)
    {
    case WATER:
        return 285;
    case ICE:
        return 150;
    case STEAM:
        return 400;
    case FIRE:
        return 1300;
    case MAGMA:
        return 1600;
    case BASALT:
        return 400;
    default:
        return 293;
    }
}

int8_t getGlow(ParticleType type) {
    switch (type)
    {
    case SAND:
        return 200;
    case WALL:
        return 200;
    case WOOD:
        return 200;
    case WATER:
        return 100;
    case ICE:
        return 30;
    case STEAM:
        return 30;
    case FIRE:
        return 255;
    case MAGMA:
        return 200;
    case BASALT:
        return 100;
    case EMPTY:
        return 30;
    default:
        return 0;
    }
}

int8_t getFlagsFromType(ParticleType type) {
    switch (type) {
    case SAND:  return P_POWDER;
    case WATER: return P_LIQUID;
    case WALL:  return P_SOLID;
    case WOOD:  return P_SOLID;
    case ICE:   return P_SOLID;
    case STEAM: return P_GAS;
    case FIRE:  return P_GAS;
    case MAGMA:  return P_LIQUID;
    case BASALT:  return P_SOLID;
    default:    return P_NONE;
    }
}

Particle createParticleFromType(ParticleType type) {
    Particle p;

    p.color = getColorFromType(type);
    p.temperature = getTemperatureFromType(type);
    p.glowIntensity = getGlow(type);
    p.flags = getFlagsFromType(type);
    p.sleepingCounter = 0;
    p.type = type;
    p.lastUpdateFrame = 0;
    p.isSleeping = 0;

    return p;
}