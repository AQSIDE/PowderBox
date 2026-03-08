#include "particle_behavior.h"

void updateIceTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    if (p->temperature >= 273) {
        if (GetRandomValue(0, 100) > 5) return;

        Particle water = createParticleFromType(WATER);
        water.temperature = p->temperature;

        setPixel(map, x, y, water);
        return;
    }
}