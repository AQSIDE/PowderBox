#include "particle_behavior.h"

void updateBasaltTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    if (p->temperature >= 1250) {
        if (GetRandomValue(0, 100) < 30) return;

        if (GetRandomValue(0, 100) < 90) {
            Particle magma = createParticleFromType(MAGMA);
            magma.temperature = p->temperature;

            setPixel(map, x, y, magma);
        }
        else {
            Particle fire = createParticleFromType(FIRE);
            fire.temperature = p->temperature;
            setPixel(map, x, y, fire);
        }
    }
}