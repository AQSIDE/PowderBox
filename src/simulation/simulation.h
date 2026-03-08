#include "map.h"
#include "context.h"

typedef struct Simulation {
    float simulationSpeed;
    uint8_t isPaused;
} Simulation;

Simulation createSimulation();

void updateSimulation(Simulation* sim, Map* map, FrameContext* fc);