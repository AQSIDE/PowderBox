#include "particle_behavior.h"

static Vector2 directions[] = { {0, -1},{-1, -1}, {1, -1},{-1, 0},  {1, 0} };

int updateSteamBehavior(Map* map, uint32_t currentFrame, int x, int y) {
    int startIdx = GetRandomValue(1, 4);
    for (int i = 0; i < 4; i++) {
        int idx = 1 + (startIdx + i) % 4;
        int tx = x + (int)directions[idx].x;
        int ty = y + (int)directions[idx].y;

        Particle* target = getPixel(map, tx, ty);
        if (target != NULL && target->type == EMPTY) {
            movePixel(map, currentFrame, x, y, tx, ty);
            return 1;
        }
    }

    return 0;
}

void updateSteamTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    if (GetRandomValue(0, 10000) > 9995) {
        clearPixel(map, x, y);
        return;
    }

    if (GetRandomValue(0, 100) > 95) {
        int cooling = GetRandomValue(1, 2);
        if (p->temperature > cooling) {
            p->temperature -= cooling;
        }
    }

    if (p->temperature <= 360) {
        if (GetRandomValue(0, 100) > 5) return;

        Particle water = createParticleFromType(WATER);
        water.temperature = p->temperature;

        setPixel(map, x, y, water);
        return;
    }

    if (p->temperature > 500) {
        if (GetRandomValue(0, 100) > 10) return;

        clearPixel(map, x, y);
    }
}