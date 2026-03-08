#include "context.h"
#include "player.h"

void initGameMenu(Map* map);
void drawGameMenuDebug(Player* p, Map* map, Simulation* sim, FrameContext* fc);
void drawGameMenu(Player* player, Map* map, FrameContext* fc);
void drawPlayerCrosshair(Player* player, FrameContext* fc);
void updateGameMenu(Player* player, Map* map);
