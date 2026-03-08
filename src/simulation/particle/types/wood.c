#include "particle_behavior.h"

void updateWoodTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    if (p->temperature >= 500) {
        if (GetRandomValue(0, 100) > 80) return;

        Particle fire = createParticleFromType(FIRE);
        setPixel(map, x, y, fire);

        if (GetRandomValue(0, 100) > 95) return;

        NeighborList neighbors;
        getNeighbors(map, x, y, 1, &neighbors);
        if (neighbors.count == 0) return;

        for (int i = 0; i < neighbors.count; i++) {
            int index = neighbors.indices[i];

            Particle* p_target = &map->particles[index];
            if (p_target->type == EMPTY && GetRandomValue(0, 100) > 90) {
                Vector2 cords = getCoordFromIndex(map, index);
                Particle spark = createParticleFromType(FIRE);
                spark.temperature = fire.temperature;

                setPixel(map, (int)cords.x, (int)cords.y, spark);
            }
        }

        return;
    }
}