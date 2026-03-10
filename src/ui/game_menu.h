#include "context.h"
#include "player.h"
#include "cursor.h"

void initGameMenu(Map* map);
void drawGameMenuDebug(Player* p, Map* map, Simulation* sim, FrameContext* fc);
void drawGameMenu(Player* player, Map* map, CursorState* cur, FrameContext* fc);
void drawPlayerCrosshair(Player* player, CursorState* cur, FrameContext* fc);
void updateGameMenu(Player* player, CursorState* cur, Map* map);
