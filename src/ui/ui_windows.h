#include "context.h"
#include "player.h"

void initWidows(Map* map);
void drawWindow(Player* player, Map* map, FrameContext* fc);
void updateWindow(Player* player, Map* map);
void setMapNameToInputField(const char* name);