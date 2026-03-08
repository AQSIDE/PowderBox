#include "ui_windows.h"
#include "button.h"
#include "input_field.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static Vector2 currentWindowSize;

static Vector2 windowExitSize = (Vector2){ 400, 220 };
static Vector2 windowClearMapSize = (Vector2){ 400, 220 };
static Vector2 windowMapSize = (Vector2){ 600, 400 };

static Button exitWindowBtn;

static Button yesExitButton;
static Button noExitButton;

static Button yesClearMapButton;
static Button noClearMapButton;

static Button applyMapButton;
static InputField mapWidthIF;
static InputField mapHeightIF;

static bool mapSettingsChanged = false;

void initWidows(Map* map) {
    Vector2 exitBtnSize = (Vector2){ 100, 50 };

    exitWindowBtn = createButton(999, (Vector2) { 0 }, (Vector2) { 30, 30 }, "-", (Color) { 80, 40, 40, 255 }, RED, (Color) { 0 });

    yesExitButton = createButton(0, (Vector2) { 0 }, exitBtnSize, "YES", RED, BLACK, WHITE);
    noExitButton = createButton(0, (Vector2) { 0 }, exitBtnSize, "NO", GREEN, BLACK, WHITE);

    yesClearMapButton = createButton(0, (Vector2) { 0 }, exitBtnSize, "YES", RED, BLACK, WHITE);
    noClearMapButton = createButton(0, (Vector2) { 0 }, exitBtnSize, "NO", GREEN, BLACK, WHITE);

    applyMapButton = createButton(0, (Vector2) { 0 }, (Vector2) { 300, 60 }, "APPLY", GREEN, BLACK, WHITE);
    mapWidthIF = createInputField(0, (Vector2) { 0 }, (Vector2) { 300, 60 }, "WIDTH: ", GREEN);
    mapHeightIF = createInputField(0, (Vector2) { 0 }, (Vector2) { 300, 60 }, "HEIGHT: ", GREEN);

    snprintf(mapWidthIF.charBuffer, 16, "%d", map->width);
    mapWidthIF.letterCount = strlen(mapWidthIF.charBuffer);

    snprintf(mapHeightIF.charBuffer, 16, "%d", map->height);
    mapHeightIF.letterCount = strlen(mapHeightIF.charBuffer);
}

void drawExit(Player* player, FrameContext* fc, float x, float y) {
    DrawText("EXIT GAME?", x + 100, y + 50, 30, GOLD);
    DrawText("Are you sure you want to quit?", x + 40, y + 100, 20, LIGHTGRAY);

    yesExitButton.pos = (Vector2){ x + 250, y + 140 };
    noExitButton.pos = (Vector2){ x + 50, y + 140 };

    drawButton(&yesExitButton);
    drawButton(&noExitButton);
}

void drawMapSettings(Player* player, Map* map, FrameContext* fc, float x, float y) {
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

    drawInputField(&mapWidthIF);
    drawInputField(&mapHeightIF);

    float btnX = x + currentWindowSize.x / 2.0f - applyMapButton.size.x / 2.0f;
    applyMapButton.pos = (Vector2){ btnX, y + currentWindowSize.y - 100.0f };

    drawButton(&applyMapButton);

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

void drawClearMap(Player* player, FrameContext* fc, float x, float y) {
    DrawText("CLEAR MAP?", x + 100, y + 50, 30, GOLD);
    DrawText("All progress will be lost.", x + 70, y + 100, 20, LIGHTGRAY);

    yesClearMapButton.pos = (Vector2){ x + 250, y + 140 };
    noClearMapButton.pos = (Vector2){ x + 50, y + 140 };

    drawButton(&yesClearMapButton);
    drawButton(&noClearMapButton);
}

void drawWindow(Player* player, Map* map, FrameContext* fc) {
    DrawRectangle(0, 0, fc->window->width, fc->window->height, (Color) { 0, 0, 0, 150 });

    switch (player->currentWindow) {
    case UI_MAP:
        currentWindowSize = windowMapSize;
        break;
    case UI_FILE:
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
        drawMapSettings(player, map, fc, x, y);
        break;
    case UI_FILE:
        break;
    case UI_SETTINGS:
        break;
    case UI_CLEAR_MAP:
        drawClearMap(player, fc, x, y);
        break;
    case UI_EXIT:
        drawExit(player, fc, x, y);
        break;
    default:
        break;
    }

    float padding = 10.0f;
    float btnX = x + currentWindowSize.x - 30 - padding;
    float btnY = y + padding;

    exitWindowBtn.pos = (Vector2){ btnX, btnY };

    drawButton(&exitWindowBtn);
}

void updateWindow(Player* player, Map* map) {
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
        bool changed = (inputW != map->width || inputH != map->height);

        if (changed && isButtonClicked(&applyMapButton)) {
            if (inputW < 16 || inputH < 16) return;
            if (inputW > 2000 || inputH > 2000) return;

            freeMap(map);

            *map = createMap(inputW, inputH);

            //snprintf(mapWidthIF.charBuffer, 15, "%d", map->width);
            //mapWidthIF.letterCount = (int)strlen(mapWidthIF.charBuffer);

            //snprintf(mapHeightIF.charBuffer, 15, "%d", map->height);
            //mapHeightIF.letterCount = (int)strlen(mapHeightIF.charBuffer);

            player->camera.target = (Vector2){ map->width / 2.0f, map->height / 2.0f };
            player->currentWindow = UI_NONE;
            player->inputTimer = 0.5f;
        }
        return;
    case UI_FILE:
        player->inputTimer = 0.5f;
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