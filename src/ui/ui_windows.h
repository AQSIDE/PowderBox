#include "context.h"
#include "player.h"
#include "cursor.h"

void initWidows(Map* map);
void drawWindow(Player* player, Map* map, CursorState* cur, FrameContext* fc);
void updateWindow(Player* player, CursorState* cur, Map* map);
void setMapNameToInputField(const char* name);