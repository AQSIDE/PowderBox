#include "particle_behavior.h"

static Vector2 directions[] = { {0, -1},{-1, -1}, {1, -1},{-1, 0},  {1, 0} };

int updateFireBehavior(Map* map, uint32_t currentFrame, int x, int y) {
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

void updateFireTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    int cooling = GetRandomValue(2, 10);
    if (p->temperature > cooling) {
        p->temperature -= cooling;
    }

    if (p->temperature <= 380) {
        if (GetRandomValue(0, 100) > 10) return;

        clearPixel(map, x, y);
        return;
    }
}