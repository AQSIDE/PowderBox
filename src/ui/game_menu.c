#include "game_menu.h"
#include "button.h"
#include "ui_windows.h"

#define MENU_BTN_COUNT 9
#define MENU_SETTINGS_BTN_COUNT 5

static Button menuButtons[MENU_BTN_COUNT];
static Button settingsButtons[MENU_SETTINGS_BTN_COUNT];

static Vector2 menuButtonSize = (Vector2){ .x = 100, .y = 50 };
static Vector2 settingsButtonSize = (Vector2){ .x = 150, .y = 30 };
static float heightMenu = 110.0f;

static float scrollX = 0.0f;
static float scrollSpeed = 30.0f;

static int selectedId = 1;

void initGameMenu(Map* map) {
    char* btnMenuLabels[MENU_BTN_COUNT] = { "WALL", "SAND", "WATER", "ICE", "STEAM", "FIRE", "WOOD", "MAGMA", "BASALT" };
    char* btnSettingsLabels[MENU_SETTINGS_BTN_COUNT] = { "MAP", "SAVE/LOAD", "CLEAR", "SETTINGS", "EXIT" };
    Color btnColors[MENU_BTN_COUNT] = {
        WALL_COLOR, SAND_COLOR, WATER_COLOR, ICE_COLOR,
        STEAM_COLOR, FIRE_COLOR, WOOD_COLOR, MAGMA_COLOR, BASALT_COLOR
    };
    Color ctrlBase = (Color){ 60, 60, 65, 255 };
    Color ctrlHover = (Color){ 90, 90, 95, 255 };

    Color dangerBase = (Color){ 150, 40, 40, 255 };
    Color dangerHover = (Color){ 190, 50, 50, 255 };

    for (int i = 0; i < MENU_BTN_COUNT; i++) {
        menuButtons[i] = createButton(i, (Vector2) { .x = 0, .y = 0, }, menuButtonSize, btnMenuLabels[i], btnColors[i]);
    }

    for (int i = 0; i < MENU_SETTINGS_BTN_COUNT; i++) {
        Color currentBase = ctrlBase;

        if (i == 2 || i == 4) {
            currentBase = dangerBase;
        }

        settingsButtons[i] = createButton(i, (Vector2) { .x = 0, .y = 0, }, settingsButtonSize, btnSettingsLabels[i], currentBase);
    }

    initWidows(map);
}

void drawMenuBtns(Player* player, CursorState* cur, FrameContext* fc, float currentMenuY) {
    float spacing = 10.0f;
    float centerY = currentMenuY + (heightMenu / 2) - (menuButtonSize.y / 2);

    for (int i = 0; i < MENU_BTN_COUNT; i++) {
        float xPos = spacing + (float)i * (menuButtonSize.x + spacing) + scrollX;
        Button* b = &menuButtons[i];

        float currentY = centerY;

        if (b->id == selectedId) {
            currentY -= 15.0f;
        }

        b->pos = (Vector2){ .x = xPos, .y = currentY };

        drawButton(b, cur);
    }
}

void drawSettingsBtns(Player* player, CursorState* cur, FrameContext* fc, float currentMenuY) {
    float spacing = 10.0f;

    float topRowY = currentMenuY - settingsButtonSize.y - 5.0f;

    for (int i = 0; i < MENU_SETTINGS_BTN_COUNT; i++) {
        float xPos = 10.0f + i * (settingsButtonSize.x + spacing);
        Button* b = &settingsButtons[i];

        b->pos = (Vector2){ xPos, topRowY };

        drawButton(b, cur);
    }
}

void drawGameMenu(Player* player, Map* map, CursorState* cur, FrameContext* fc) {
    Color menuBgColor = (Color){ 40, 40, 40, 255 };
    Color borderColor = (Color){ 80, 80, 80, 255 };
    float borderThickness = 2.0f;

    bool isWindowOpen = player->currentWindow != UI_NONE;
    float currentMenuY = fc->window->height - heightMenu;

    Rectangle menuRect = { 0, (float)fc->window->height - heightMenu, (float)fc->window->width, heightMenu };

    player->isOverUI = isWindowOpen || CheckCollisionPointRec(GetMousePosition(), menuRect);

    if (isWindowOpen) {
        drawWindow(player, map, cur, fc);
    }
    else {
        DrawRectangle(0, currentMenuY, fc->window->width, heightMenu, menuBgColor);
        DrawRectangleLinesEx(menuRect, borderThickness, borderColor);

        drawMenuBtns(player, cur, fc, currentMenuY);
        drawSettingsBtns(player, cur, fc, currentMenuY);
    }
}

void updateGameMenu(Player* player, CursorState* cur, Map* map) {
    bool isWindowOpen = player->currentWindow != UI_NONE;

    if (isWindowOpen) {
        updateWindow(player, cur, map);
        return;
    }

    if (player->isOverUI) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            scrollX += wheel * scrollSpeed;
        }
    }

    if (scrollX > 0) scrollX = 0;

    for (int i = 0; i < MENU_BTN_COUNT; i++) {
        Button* b = &menuButtons[i];

        if (isButtonClicked(b)) {
            selectedId = i;

            switch (b->id)
            {
            case 0:
                player->selectedParticle = WALL;
                break;
            case 1:
                player->selectedParticle = SAND;
                break;
            case 2:
                player->selectedParticle = WATER;
                break;
            case 3:
                player->selectedParticle = ICE;
                break;
            case 4:
                player->selectedParticle = STEAM;
                break;
            case 5:
                player->selectedParticle = FIRE;
                break;
            case 6:
                player->selectedParticle = WOOD;
                break;
            case 7:
                player->selectedParticle = MAGMA;
                break;
            case 8:
                player->selectedParticle = BASALT;
                break;

            default:
                break;
            }
        }
    }

    for (int i = 0; i < MENU_SETTINGS_BTN_COUNT; i++) {
        Button* b = &settingsButtons[i];

        if (isButtonClicked(b)) {
            switch (b->id)
            {
            case 0:
                player->currentWindow = UI_MAP;
                break;
            case 1:
                cur->currentType = CUSTOM_CURSOR_BUSY;
                player->currentWindow = UI_FILE;
                loadMapList(&player->mapList, BASE_MAP_DIR);
                setMapNameToInputField(map->name);
                break;
            case 2:
                player->currentWindow = UI_CLEAR_MAP;
                break;
            case 3:
                player->currentWindow = UI_SETTINGS;
                break;
            case 4:
                player->currentWindow = UI_EXIT;
                break;

            default:
                break;
            }
        }
    }
}

void drawPlayerCrosshair(Player* player, CursorState* cur, FrameContext* fc) {
    if (player->currentWindow != UI_NONE || player->isOverUI) return;

    if (!fc->pointedParticle) {
        cur->currentType = CUSTOM_CURSOR_CROSS;
        return;
    }

    float thickness = 1.0f / player->camera.zoom;

    if (player->brush.type == BRUSH_SQUARE) {
        int targetX = (int)fc->worldMouse.x - ((int)player->brush.width / 2);
        int targetY = (int)fc->worldMouse.y - ((int)player->brush.height / 2);

        Rectangle rect = {
            (float)targetX,
            (float)targetY,
            (float)player->brush.width,
            (float)player->brush.height
        };

        DrawRectangleLinesEx(rect, thickness, WHITE);
    }
    else {
        Vector2 intMouse = { (float)((int)fc->worldMouse.x), (float)((int)fc->worldMouse.y) };
        float innerRadius = (float)((int)player->brush.size) - thickness;

        DrawRing(intMouse, innerRadius, (float)((int)player->brush.size), 0, 360, 36, WHITE);
    }
}

void drawGameMenuDebug(Player* p, Map* map, Simulation* sim, FrameContext* fc) {
    int startY = 10;
    int stepY = 25;
    Color textColor = RAYWHITE;

    Particle* pointedParticle = fc->pointedParticle;
    int8_t hasPointed = pointedParticle != NULL;

    DrawText(TextFormat("Selected: [%d, %d]", (int)fc->mapPoint.x, (int)fc->mapPoint.y), 10, startY, 20, textColor);

    if (hasPointed) {
        const char* pName = getParticleNameFromType(pointedParticle->type);

        DrawText(TextFormat("Type: %s", pName), 10, startY + stepY * 5, 20, YELLOW);

        DrawText(TextFormat("Temp: %u K (%d C)",
            pointedParticle->temperature,
            (int)pointedParticle->temperature - 273),
            10, startY + stepY * 6, 20, ORANGE);

        const char* sleepStatus = pointedParticle->isSleeping ? "ASLEEP" : "AWAKE";
        DrawText(TextFormat("State: %s", sleepStatus), 10, startY + stepY * 7, 20, LIGHTGRAY);
    }
    else {
        DrawText("Type: OUT OF BOUNDS", 10, startY + stepY * 5, 20, RED);
    }

    const char* typeName = getParticleNameFromType(p->selectedParticle);

    const char* mode = p->canReplaceParticles ? "REPLACE" : "NORMAL";
    const char* brushModeName = (p->brushMode == BRUSH_PARTICLE) ? "PARTICLE" :
        (p->brushMode == BRUSH_HEAT) ? "HEAT (+)" : "COLD (-)";

    DrawText(TextFormat("Tool: %s Write Mode: %s (R) / Brush Mode: %s (C)",
        typeName, mode, brushModeName),
        10, startY + stepY, 20, textColor);

    const char* status = sim->isPaused ? "PAUSED (Space)" : "RUNNING (Space)";
    Color statusColor = sim->isPaused ? ORANGE : GREEN;
    DrawText(TextFormat("Status: %s", status), 10, startY + stepY * 2, 20, statusColor);

    DrawText(TextFormat("Pixels: %d/%d", map->filledPixels, map->width * map->height), 10, startY + stepY * 3, 20, textColor);

    DrawText(TextFormat("Sim Speed: %.1f (Q/E)", sim->simulationSpeed), 10, startY + stepY * 4, 20, textColor);

    DrawFPS(fc->window->width - 100, 10);
}