#include "particle.h"
#include "map.h"

// FIRE
int updateFireBehavior(Map* map, uint32_t currentFrame, int x, int y);
void updateFireTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y);

// WATER
int updateWaterBehavior(Map* map, Particle* p, uint32_t currentFrame, int x, int y);
void updateWaterTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y);

// WOOD
void updateWoodTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y);

// STEAM
int updateSteamBehavior(Map* map, uint32_t currentFrame, int x, int y);
void updateSteamTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y);

// SAND
int updateSandBehavior(Map* map, Particle* p, uint32_t currentFrame, int x, int y);
void updateSandTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y);

// ICE
void updateIceTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y);

// MAGMA
int updateMagmaBehavior(Map* map, Particle* p, uint32_t currentFrame, int x, int y);
void updateMagmaTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y);

// BASALT
void updateBasaltTemperature(Map* map, Particle* p, uint32_t currentFrame, int x, int y);