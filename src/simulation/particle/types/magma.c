#include "particle_behavior.h"

static Vector2 directions[] = { {0, 1}, {-1, 1}, {1, 1}, {-1, 0}, {1, 0} };

int updateMagmaBehavior(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    Particle* down = getPixel(map, x, y + 1);

    if (down && down->type == EMPTY) {
        movePixel(map, currentFrame, x, y, x, y + 1);
        return 1;
    }

    //if (GetRandomValue(0, 100) < 90) return 0;

    shuffleDirections(directions, 5);
    for (int i = 0; i < 5; i++) {
        int tx = x + (int)directions[i].x;
        int ty = y + (int)directions[i].y;
        Particle* target = getPixel(map, tx, ty);

        if (!target) continue;

        if (target->type == EMPTY) {
            movePixel(map, currentFrame, x, y, tx, ty);
            return 1;
        }

        if (target->type != p->type && (target->flags & (P_GAS | P_LIQUID))) {
            swapPixels(map, currentFrame, x, y, tx, ty);
            return 1;
        }
    }

    return 0;
}

void updateMagmaTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    if (p->temperature >= 1200) {
        if (GetRandomValue(0, 1000) < 990) return;

        NeighborList neighbors;
        getNeighbors(map, x, y, 1, &neighbors);
        if (neighbors.count == 0) return;

        for (int i = 0; i < neighbors.count; i++) {
            int index = neighbors.indices[i];

            Particle* p_target = &map->particles[index];
            if (p_target->type == EMPTY && GetRandomValue(0, 100) > 90) {
                Vector2 cords = getCoordFromIndex(map, index);
                Particle spark = createParticleFromType(FIRE);

                setPixel(map, (int)cords.x, (int)cords.y, spark);
            }
        }
    }
    else if (p->temperature < 1050) {
        if (GetRandomValue(0, 100) < 10) {
            float oldTemp = p->temperature;
            Particle basalt = createParticleFromType(BASALT);
            basalt.temperature = oldTemp;

            setPixel(map, x, y, basalt);
        }
    }
}