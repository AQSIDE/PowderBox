#pragma once

#include "raylib.h"

#define CUR_SCALE 0.9f

typedef enum {
    CUSTOM_CURSOR_DEFAULT,
    CUSTOM_CURSOR_POINTER,
    CUSTOM_CURSOR_BRUSH,
    CUSTOM_CURSOR_TEXT,
    CUSTOM_CURSOR_BUSY,
} CursorType;

typedef struct {
    Texture2D texture;
    Vector2 offset;
} CursorData;

typedef struct {
    CursorType currentType;
    CursorData defaultCursor;
    CursorData pointerCursor;
    CursorData brushCursor;
    CursorData textCursor;
    CursorData busyCursor;
} CursorState;

CursorState loadCursors();
void drawCursor(CursorState* c, Vector2 mousePos);