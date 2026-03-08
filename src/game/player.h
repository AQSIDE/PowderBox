#ifndef PLAYER_H
#define PLAYER_H

#include "map.h"
#include "context.h"
#include "simulation.h"

typedef enum {
    UI_NONE,
    UI_MAP,
    UI_FILE,
    UI_SETTINGS,
    UI_CLEAR_MAP,
    UI_EXIT
} UIWindow;

typedef enum BrushMode {
    BRUSH_PARTICLE,
    BRUSH_HEAT,
    BRUSH_COLD
} BrushMode;

typedef enum { BRUSH_CIRCLE, BRUSH_SQUARE } BrushType;

typedef struct {
    BrushType type;
    uint16_t size;
    uint16_t width;
    uint16_t height;
} Brush;

typedef struct Player {
    BrushMode brushMode;
    ParticleType selectedParticle;
    UIWindow currentWindow;
    Brush brush;
    Camera2D camera;
    float targetZoom;
    bool isOverUI;
    float inputTimer;
    int8_t canReplaceParticles;
} Player;

Player createPlayer(int mapW, int mapH);

void updatePlayer(Player* player);
void handlePlayerInput(Player* player, Simulation* sim, Map* map, FrameContext* fc);
void drawPlayerUI(Player* player, FrameContext* fc);

#endif