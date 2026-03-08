#include "simulation.h"
#include <stdlib.h>
#include <math.h>
#include <particle_behavior.h>

int updateParticleBehavior(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    if (p->type == SAND) {
        return updateSandBehavior(map, p, currentFrame, x, y);
    }
    else if (p->type == WATER) {
        return updateWaterBehavior(map, p, currentFrame, x, y);
    }
    else if (p->type == STEAM) {
        return updateSteamBehavior(map, currentFrame, x, y);
    }
    else if (p->type == FIRE) {
        return updateFireBehavior(map, currentFrame, x, y);
    }
    else if (p->type == MAGMA) {
        return updateMagmaBehavior(map, p, currentFrame, x, y);
    }

    return 0;
}

void updateParticleTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    if (p->type == ICE) {
        updateIceTemperature(map, p, currentFrame, x, y);
        return;
    }
    else if (p->type == SAND) {
        updateSandTemperature(map, p, currentFrame, x, y);
        return;
    }
    else if (p->type == WATER) {
        updateWaterTemperature(map, p, currentFrame, x, y);
        return;
    }
    else if (p->type == STEAM) {
        updateSteamTemperature(map, p, currentFrame, x, y);
        return;
    }
    else if (p->type == FIRE) {
        updateFireTemperature(map, p, currentFrame, x, y);
        return;
    }
    else if (p->type == WOOD) {
        updateWoodTemperature(map, p, currentFrame, x, y);
        return;
    }
    else if (p->type == MAGMA) {
        updateMagmaTemperature(map, p, currentFrame, x, y);
        return;
    }
    else if (p->type == BASALT) {
        updateBasaltTemperature(map, p, currentFrame, x, y);
        return;
    }
}

Simulation createSimulation() {
    return (Simulation) {
        .simulationSpeed = 1.0f,
            .isPaused = 0
    };
}

#define AMBIENT_TEMPERATURE 293 
#define COOLING_SPEED 0.01f 
#define THERMAL_CONDUCTIVITY 0.8f

void handleTemperature(Map* map, Particle* p, int x, int y) {
    float current_temp = (float)p->temperature;

    NeighborList neighbors;

    int radius = (GetRandomValue(0, 100) > 90) ? 2 : 1;
    getNeighbors(map, x, y, radius, &neighbors);
    if (neighbors.count == 0) return;

    float conduct_per_neighbor = THERMAL_CONDUCTIVITY / (float)neighbors.count;

    for (int i = 0; i < neighbors.count; i++) {
        int neighborIdx = neighbors.indices[i];
        Particle* p_neighbor = &map->particles[neighborIdx];

        if (p_neighbor->type == EMPTY) continue;

        float diff = current_temp - (float)p_neighbor->temperature;
        if (fabsf(diff) < 0.1f) continue;

        int32_t transfer = (int32_t)(diff * conduct_per_neighbor);

        if (transfer == 0) {
            transfer = (diff > 0) ? 1 : -1;
        }

        p_neighbor->temperature += transfer;
        current_temp -= transfer;

        map->colorTempBuffer[neighborIdx] = getColorFromTemperature(p_neighbor->temperature, p_neighbor->glowIntensity);
    }

    if (current_temp < 0) current_temp = 0;
    if (current_temp > 30000) current_temp = 30000;

    p->temperature = (uint16_t)current_temp;

    map->colorTempBuffer[getIndex(map, x, y)] = getColorFromTemperature(p->temperature, p->glowIntensity);
    map->isDirtyTemp = 1;
}

void updateSimulation(Simulation* sim, Map* map, FrameContext* fc) {
    if (sim->isPaused) return;

    int8_t leftToRight = (fc->currentFrame % 2 == 0);

    for (int y = map->height - 1; y >= 0; y--) {
        for (int x = 0; x < map->width; x++) {
            int rx = leftToRight ? x : (map->width - 1 - x);

            Particle* p = getPixel(map, rx, y);
            if (p == NULL || p->type == EMPTY || p->lastUpdateFrame == fc->currentFrame) continue;

            if (GetRandomValue(0, 100) < 15) continue;

            if (fc->currentFrame % 8 == 0) {
                handleTemperature(map, p, rx, y);
            }

            updateParticleTemperature(map, p, fc->currentFrame, rx, y);

            if (p->isSleeping) {
                p->sleepingCounter += fc->deltaTime;

                if (p->sleepingCounter >= 0.5f) {
                    p->sleepingCounter = 0.0f;
                    p->isSleeping = 0;
                }
                else continue;
            }

            int moved = updateParticleBehavior(map, p, fc->currentFrame, rx, y);
            if (moved) {
                p->isSleeping = 0;
                p->sleepingCounter = 0.0f;
            }
            else {
                p->isSleeping = 1;
            }
        }
    }
}