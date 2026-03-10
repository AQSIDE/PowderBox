#include "ui_windows.h"
#include "button.h"
#include "input_field.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static Vector2 currentWindowSize;

static Vector2 windowExitSize = (Vector2){ 400, 220 };
static Vector2 windowClearMapSize = (Vector2){ 400, 220 };
static Vector2 windowMapSize = (Vector2){ 600, 400 };
static Vector2 windowMapStorage = (Vector2){ 800, 700 };

static Button exitWindowBtn;

static Button yesExitButton;
static Button noExitButton;

static Button yesClearMapButton;
static Button noClearMapButton;

static Button applyMapButton;
static InputField mapWidthIF;
static InputField mapHeightIF;

static Button saveMapButton;
static Button loadMapButton;
static Button deleteMapButton;
static InputField mapNameIF;

static int selectedMapIndex = -1;
static float scrollOffset = 0;

static bool mapSettingsChanged = false;

void initWidows(Map* map) {
    Vector2 exitBtnSize = (Vector2){ 100, 50 };

    exitWindowBtn = createButton(999, (Vector2) { 0 }, (Vector2) { 30, 30 }, "-", (Color) { 80, 40, 40, 255 });
    exitWindowBtn.borderColor = (Color){ 0 };

    yesExitButton = createButton(0, (Vector2) { 0 }, exitBtnSize, "YES", RED);
    noExitButton = createButton(0, (Vector2) { 0 }, exitBtnSize, "NO", GREEN);

    yesClearMapButton = createButton(0, (Vector2) { 0 }, exitBtnSize, "YES", RED);
    noClearMapButton = createButton(0, (Vector2) { 0 }, exitBtnSize, "NO", GREEN);

    applyMapButton = createButton(0, (Vector2) { 0 }, (Vector2) { 300, 60 }, "APPLY", GREEN);
    mapWidthIF = createInputField(0, (Vector2) { 0 }, (Vector2) { 300, 60 }, "WIDTH: ", GREEN);
    mapWidthIF.onlyNumbers = true;
    mapHeightIF = createInputField(0, (Vector2) { 0 }, (Vector2) { 300, 60 }, "HEIGHT: ", GREEN);
    mapHeightIF.onlyNumbers = true;

    mapNameIF = createInputField(0, (Vector2) { 0 }, (Vector2) { 400, 60 }, "NAME: ", GREEN);
    saveMapButton = createButton(0, (Vector2) { 0 }, (Vector2) { 200, 40 }, "SAVE", GREEN);
    loadMapButton = createButton(0, (Vector2) { 0 }, (Vector2) { 200, 40 }, "LOAD", ORANGE);
    deleteMapButton = createButton(0, (Vector2) { 0 }, (Vector2) { 200, 40 }, "DELETE", RED);

    snprintf(mapWidthIF.charBuffer, 16, "%d", map->width);
    mapWidthIF.letterCount = strlen(mapWidthIF.charBuffer);

    snprintf(mapHeightIF.charBuffer, 16, "%d", map->height);
    mapHeightIF.letterCount = strlen(mapHeightIF.charBuffer);
}

void drawExit(Player* player, CursorState* cur, FrameContext* fc, float x, float y) {
    DrawText("EXIT GAME?", x + 100, y + 50, 30, GOLD);
    DrawText("Are you sure you want to quit?", x + 40, y + 100, 20, LIGHTGRAY);

    yesExitButton.pos = (Vector2){ x + 250, y + 140 };
    noExitButton.pos = (Vector2){ x + 50, y + 140 };

    drawButton(&yesExitButton, cur);
    drawButton(&noExitButton, cur);
}

void drawMapSettings(Player* player, Map* map, CursorState* cur, FrameContext* fc, float x, float y) {
    const char* title = "MAP SETTINGS";
    int fontSize = 30;

    int textWidth = MeasureText(title, fontSize);
    float titleX = x + currentWindowSize.x / 2.0f - textWidth / 2.0f;
    DrawText(title, (int)titleX, (int)y + 20, fontSize, GOLD);

    float fieldX = x + currentWindowSize.x / 2.0f - mapWidthIF.size.x / 2.0f;

    float startY = y + 80.0f;
    float verticalGap = 80.0f;

    mapWidthIF.pos = (Vector2){ fieldX, startY };
    mapHeightIF.pos = (Vector2){ fieldX, startY + verticalGap };

    drawInputField(&mapWidthIF, cur);
    drawInputField(&mapHeightIF, cur);

    float btnX = x + currentWindowSize.x / 2.0f - applyMapButton.size.x / 2.0f;
    applyMapButton.pos = (Vector2){ btnX, y + currentWindowSize.y - 100.0f };

    drawButton(&applyMapButton, cur);

    int inputW = atoi(mapWidthIF.charBuffer);
    int inputH = atoi(mapHeightIF.charBuffer);
    mapSettingsChanged = (inputW != map->width || inputH != map->height);

    if (mapSettingsChanged) {
        const char* warning = "Settings Changed! (Applying will reset current map!)";
        int warningFontSize = 18;
        int warningW = MeasureText(warning, warningFontSize);
        float warningX = x + currentWindowSize.x / 2.0f - warningW / 2.0f;

        DrawText(warning, (int)warningX, (int)y + currentWindowSize.y - 30, warningFontSize, ORANGE);
    }
}

void drawClearMap(Player* player, CursorState* cur, FrameContext* fc, float x, float y) {
    DrawText("CLEAR MAP?", x + 100, y + 50, 30, GOLD);
    DrawText("All progress will be lost.", x + 70, y + 100, 20, LIGHTGRAY);

    yesClearMapButton.pos = (Vector2){ x + 250, y + 140 };
    noClearMapButton.pos = (Vector2){ x + 50, y + 140 };

    drawButton(&yesClearMapButton, cur);
    drawButton(&noClearMapButton, cur);
}

void drawMapStorage(Player* player, CursorState* cur, FrameContext* fc, float x, float y) {
    const char* title = "MAP STORAGE";
    int fontSize = 30;

    int textWidth = MeasureText(title, fontSize);
    float titleX = x + currentWindowSize.x / 2.0f - textWidth / 2.0f;
    DrawText(title, titleX, y + 20, fontSize, GOLD);

    float padding = 30.0f;
    float line1Y = y + 80;

    mapNameIF.pos = (Vector2){ x + padding + 80, line1Y };
    drawInputField(&mapNameIF, cur);

    float blockWidth = currentWindowSize.x - padding;
    float blockPadding = 80;
    float blockHeight = 450;

    Rectangle blockRect = { x + currentWindowSize.x / 2 - blockWidth / 2,line1Y + blockPadding,blockWidth,blockHeight };
    DrawRectangleRec(blockRect, (Color) { 0, 0, 0, 100 });

    if (CheckCollisionPointRec(fc->screenMouse, blockRect)) {
        float scroll = GetMouseWheelMove();
        if (scroll != 0) {
            float scrollSpeed = 30.0f;
            scrollOffset -= scroll * scrollSpeed;
            if (scrollOffset < 0) scrollOffset = 0;
        }
    }

    float mapsTop = line1Y + blockPadding;
    float mapsLeft = x + padding;
    float mapsRight = x + currentWindowSize.x - padding;
    float blockInnerWidth = mapsRight - mapsLeft;
    float blockHeightPerMap = 100;
    float blockSpacing = 5;

    BeginScissorMode(blockRect.x, blockRect.y, blockRect.width, blockRect.height);
    for (int i = 0; i < player->mapList.mapCount; i++) {
        MapInfo* map = &player->mapList.maps[i];
        float blockY = mapsTop + i * (blockHeightPerMap + blockSpacing) - scrollOffset;
        Rectangle mapRect = { mapsLeft, blockY, blockInnerWidth, blockHeightPerMap };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(fc->screenMouse, mapRect)) {
            selectedMapIndex = (selectedMapIndex != i) ? i : -1;

            if (i < player->mapList.mapCount) {
                MapInfo* mapInfo = &player->mapList.maps[i];

                setInputFieldText(&mapNameIF, mapInfo->name);
            }
        }

        Color blockColor = (selectedMapIndex == i) ? (Color) { 70, 70, 70, 200 } : (Color) { 50, 50, 50, 150 };
        DrawRectangleRec(mapRect, blockColor);

        if (selectedMapIndex == i) {
            DrawRectangleLinesEx(mapRect, 2.0f, GOLD);
        }

        if (map->preview.id > 0) {
            float previewSize = blockHeightPerMap - 10;
            Rectangle destRect = {
                mapsLeft + 5,
                blockY + 5,
                previewSize,
                previewSize
            };

            Rectangle sourceRect = { 0, 0, (float)map->preview.width, (float)map->preview.height };

            DrawTexturePro(
                map->preview,
                sourceRect,
                destRect,
                (Vector2) {
                0, 0
            },
                0.0f,
                WHITE
            );

            DrawRectangleLinesEx(destRect, 1.0f, WHITE);
        }

        int textNameX = mapsLeft + 10 + blockHeightPerMap;
        int textNameY = blockY + (blockHeightPerMap / 2) - 18;
        DrawText(map->name, textNameX, textNameY, 20, RAYWHITE);

        time_t t = (time_t)map->timestamp;
        struct tm* dt = localtime(&t);

        char timeStr[32] = "N/A";

        if (dt) {
            strftime(timeStr, sizeof(timeStr), "%d.%m %H:%M", dt);
        }

        int sizeTextY = textNameY + 22;
        DrawText(timeStr, textNameX, sizeTextY, 16, LIGHTGRAY);

        char sizeText[32];
        snprintf(sizeText, sizeof(sizeText), "%dx%d", map->width, map->height);

        int textWidth = MeasureText(sizeText, 20);
        DrawText(sizeText, mapRect.x + mapRect.width - textWidth - 20, textNameY, 20, RAYWHITE);
    }
    EndScissorMode();

    saveMapButton.pos = (Vector2){
    x + currentWindowSize.x - saveMapButton.size.x - padding,
    line1Y + (mapNameIF.size.y - saveMapButton.size.y) / 2 };
    drawButton(&saveMapButton, cur);

    float line2Y = y + currentWindowSize.y - 60;
    deleteMapButton.pos = (Vector2){ x + currentWindowSize.x - padding - deleteMapButton.size.x, line2Y };
    drawButton(&deleteMapButton, cur);

    loadMapButton.pos = (Vector2){ x + padding, line2Y };
    drawButton(&loadMapButton, cur);
}

void drawWindow(Player* player, Map* map, CursorState* cur, FrameContext* fc) {
    DrawRectangle(0, 0, fc->window->width, fc->window->height, (Color) { 0, 0, 0, 150 });

    switch (player->currentWindow) {
    case UI_MAP:
        currentWindowSize = windowMapSize;
        break;
    case UI_FILE:
        currentWindowSize = windowMapStorage;
        break;
    case UI_SETTINGS:
        break;
    case UI_CLEAR_MAP:
        currentWindowSize = windowClearMapSize;
        break;
    case UI_EXIT:
        currentWindowSize = windowExitSize;
        break;
    default:
        break;
    }

    float x = (fc->window->width / 2.0f) - (currentWindowSize.x / 2.0f);
    float y = (fc->window->height / 2.0f) - (currentWindowSize.y / 2.0f);

    DrawRectangle((int)x, (int)y, (int)currentWindowSize.x, (int)currentWindowSize.y, (Color) { 45, 45, 45, 255 });
    DrawRectangleLinesEx((Rectangle) { x, y, currentWindowSize.x, currentWindowSize.y }, 2, GRAY);

    switch (player->currentWindow) {
    case UI_MAP:
        drawMapSettings(player, map, cur, fc, x, y);
        break;
    case UI_FILE:
        drawMapStorage(player, cur, fc, x, y);
        break;
    case UI_SETTINGS:
        break;
    case UI_CLEAR_MAP:
        drawClearMap(player, cur, fc, x, y);
        break;
    case UI_EXIT:
        drawExit(player, cur, fc, x, y);
        break;
    default:
        break;
    }

    float padding = 10.0f;
    float btnX = x + currentWindowSize.x - 30 - padding;
    float btnY = y + padding;

    exitWindowBtn.pos = (Vector2){ btnX, btnY };

    drawButton(&exitWindowBtn, cur);
}

void updateWindow(Player* player, CursorState* cur, Map* map) {
    if (isButtonClicked(&exitWindowBtn)) {
        player->currentWindow = UI_NONE;
        player->inputTimer = 0.5f;
        return;
    }

    switch (player->currentWindow) {
    case UI_MAP:
        updateInputField(&mapWidthIF);
        updateInputField(&mapHeightIF);

        int inputW = atoi(mapWidthIF.charBuffer);
        int inputH = atoi(mapHeightIF.charBuffer);

        if (mapSettingsChanged && isButtonClicked(&applyMapButton)) {
            if (inputW < 16 || inputH < 16) return;
            if (inputW > 2000 || inputH > 2000) return;

            cur->currentType = CUSTOM_CURSOR_BUSY;
            freeMap(map);

            *map = createMap(inputW, inputH);

            player->currentWindow = UI_NONE;
            player->inputTimer = 0.5f;
        }
        return;
    case UI_FILE:
        updateInputField(&mapNameIF);

        if (isButtonClicked(&saveMapButton)) {
            char name[32];
            strcpy(name, mapNameIF.charBuffer);

            if (name[0] != '\0') {
                cur->currentType = CUSTOM_CURSOR_BUSY;
                saveMapToFolder(map, name, BASE_MAP_DIR);

                strncpy(map->name, name, sizeof(map->name));
                loadMapList(&player->mapList, BASE_MAP_DIR);
            }
            return;
        }

        if (isButtonClicked(&deleteMapButton)) {
            if (selectedMapIndex < 0 || selectedMapIndex >= player->mapList.mapCount) return;

            cur->currentType = CUSTOM_CURSOR_BUSY;
            MapInfo* selectedMap = &player->mapList.maps[selectedMapIndex];

            deleteMapFile(selectedMap->path);
            UnloadTexture(selectedMap->preview);

            for (int i = selectedMapIndex; i < player->mapList.mapCount - 1; i++) {
                player->mapList.maps[i] = player->mapList.maps[i + 1];
            }

            player->mapList.mapCount--;
            selectedMapIndex = -1;

            loadMapList(&player->mapList, BASE_MAP_DIR);
        }

        if (isButtonClicked(&loadMapButton)) {
            if (selectedMapIndex <= -1) return;

            cur->currentType = CUSTOM_CURSOR_BUSY;
            MapInfo* selectedMap = &player->mapList.maps[selectedMapIndex];
            freeMap(map);

            MapFile* file = loadMapFromPath(selectedMap->path);

            *map = createMapFromFile(file, selectedMap->name);

            player->currentWindow = UI_NONE;
            player->inputTimer = 0.5f;
            return;
        }

        return;
    case UI_SETTINGS:
        player->inputTimer = 0.5f;
        return;
    case UI_CLEAR_MAP:
        if (isButtonClicked(&yesClearMapButton)) {
            clearMap(map);
            player->currentWindow = UI_NONE;
            player->inputTimer = 0.5f;
        }

        if (isButtonClicked(&noClearMapButton)) {
            player->currentWindow = UI_NONE;
            player->inputTimer = 0.5f;
        }
        return;
    case UI_EXIT:
        if (isButtonClicked(&yesExitButton)) {
            CloseWindow();
        }

        if (isButtonClicked(&noExitButton)) {
            player->currentWindow = UI_NONE;
            player->inputTimer = 0.5f;
        }

        return;
    default:
        return;
    }
}

void setMapNameToInputField(const char* name) {
    setInputFieldText(&mapNameIF, name);
    selectedMapIndex = -1;
}