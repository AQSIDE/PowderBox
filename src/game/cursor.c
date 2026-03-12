#include "cursor.h"
#include "stdlib.h"
#include "raymath.h"

static bool isCursorsLoaded = false;
static float currentCursorSize = CUR_SCALE;

CursorData createCursor(const char* path) {
    CursorData c;

    c.texture = LoadTexture(path);
    c.offset = (Vector2){ (float)c.texture.width / 2, (float)c.texture.height / 2 };

    return c;
}

CursorData* getCursorFromType(CursorState* c) {
    switch (c->currentType)
    {
    case CUSTOM_CURSOR_DEFAULT:
        return &c->defaultCursor;
    case CUSTOM_CURSOR_POINTER:
        return &c->pointerCursor;
    case CUSTOM_CURSOR_BRUSH:
        return &c->brushCursor;
    case CUSTOM_CURSOR_TEXT:
        return &c->textCursor;
    case CUSTOM_CURSOR_BUSY:
        return &c->busyCursor;
    case CUSTOM_CURSOR_CROSS:
        return &c->crossCursor;

    default:
        return &c->defaultCursor;
    }
}

CursorState loadCursors() {
    CursorState c = { 0 };
    c.defaultCursor = createCursor("resources/textures/cursors/cursor_none.png");
    c.defaultCursor.offset = (Vector2){ c.defaultCursor.offset.x - 10, c.defaultCursor.offset.y - 10 };

    c.pointerCursor = createCursor("resources/textures/cursors/hand_point.png");
    c.pointerCursor.offset = (Vector2){ c.pointerCursor.offset.x - 10, c.pointerCursor.offset.y - 10 };

    c.brushCursor = createCursor("resources/textures/cursors/drawing_brush.png");
    c.brushCursor.offset = (Vector2){ c.brushCursor.offset.x - 10, c.brushCursor.offset.y - 10 };

    c.textCursor = createCursor("resources/textures/cursors/bracket_a_vertical.png");
    c.busyCursor = createCursor("resources/textures/cursors/busy_circle.png");
    c.crossCursor = createCursor("resources/textures/cursors/cross_small.png");

    if (c.defaultCursor.texture.id > 0 &&
        c.pointerCursor.texture.id > 0) {
        HideCursor();
        isCursorsLoaded = true;
    }

    return c;
}

void drawCursor(CursorState* c, Vector2 mousePos) {
    if (!isCursorsLoaded) return;

    CursorData* cur = getCursorFromType(c);

    float targetScale = IsMouseButtonDown(MOUSE_LEFT_BUTTON) ? (CUR_SCALE * 0.8f) : CUR_SCALE;
    currentCursorSize = Lerp(currentCursorSize, targetScale, 20.0f * GetFrameTime());
    currentCursorSize = Clamp(currentCursorSize, 0, CUR_SCALE);

    Rectangle source = { 0.0f, 0.0f, (float)cur->texture.width, (float)cur->texture.height };

    Rectangle dest = {
        mousePos.x,
        mousePos.y,
        (float)cur->texture.width * currentCursorSize,
        (float)cur->texture.height * currentCursorSize
    };

    Vector2 origin = {
        cur->offset.x * currentCursorSize,
        cur->offset.y * currentCursorSize
    };

    float rotation = 0.0f;
    if (c->currentType == CUSTOM_CURSOR_BUSY) {
        rotation = (float)GetTime() * 360.0f;
    }

    DrawTexturePro(cur->texture, source, dest, origin, rotation, WHITE);
}