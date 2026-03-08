#include "particle_behavior.h"

static Vector2 directions[] = { {0, 1}, {1, 1}, {-1, 1} };

int updateSandBehavior(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    Particle* down = getPixel(map, x, y + 1);

    if (down && down->type == EMPTY) {
        movePixel(map, currentFrame, x, y, x, y + 1);
        return 1;
    }

    shuffleDirections(directions, 3);
    for (int i = 0; i < 3; i++) {
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

void updateSandTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    if (p->temperature >= 1100) {
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