#include "particle_behavior.h"

static Vector2 directions[] = { {-1, 1}, {1, 1}, {-1, 0}, {1, 0} };

int updateWaterBehavior(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    Particle* down = getPixel(map, x, y + 1);
    if (down != NULL) {
        if (down != NULL) {
            if (down->type == EMPTY) {
                movePixel(map, currentFrame, x, y, x, y + 1);
                return 1;
            }

            if (down->flags & P_GAS) {
                swapPixels(map, currentFrame, x, y, x, y + 1);
                return 1;
            }
        }
    }

    if (p->temperature > 320) {
        int boilChance = (p->temperature - 320) / 2;

        if (GetRandomValue(0, 100) < boilChance) {
            int dx = GetRandomValue(-1, 1);
            int dy = GetRandomValue(-1, -4);

            Particle* target = getPixel(map, x + dx, y + dy);
            if (target != NULL && target->type == EMPTY) {

                if (GetRandomValue(0, 100) < 10) {
                    Particle steam = createParticleFromType(STEAM);
                    steam.temperature = p->temperature + 30;
                    setPixel(map, x, y, steam);
                    return 1;
                }

                movePixel(map, currentFrame, x, y, x + dx, y + dy);
                return 1;
            }
        }
    }

    shuffleDirections(directions, 4);
    for (int i = 0; i < 4; i++) {
        int tx = x + (int)directions[i].x;
        int ty = y + (int)directions[i].y;

        Particle* target = getPixel(map, tx, ty);
        if (target != NULL) {
            if (target->type == EMPTY) {
                movePixel(map, currentFrame, x, y, tx, ty);
                return 1;
            }

            if (target->flags & P_GAS) {
                swapPixels(map, currentFrame, x, y, tx, ty);
                return 1;
            }
        }
    }

    return 0;
}

void updateWaterTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y) {
    if (p->temperature <= 273) {
        if (GetRandomValue(0, 100) > 5) return;

        Particle ice = createParticleFromType(ICE);
        ice.temperature = p->temperature;

        setPixel(map, x, y, ice);
    }

    if (p->temperature >= 373) {
        if (GetRandomValue(0, 100) > 5) return;

        Particle steam = createParticleFromType(STEAM);
        steam.temperature = p->temperature;

        setPixel(map, x, y, steam);
        return;
    }
}