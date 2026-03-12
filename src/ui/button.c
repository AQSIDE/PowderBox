#include "button.h"
#include "raymath.h"

Button createButton(int id, Vector2 pos, Vector2 size, char* text, Color color) {
    const int TEXT_SIZE = 20;

    Button b;
    b.id = id;
    b.pos = pos;
    b.size = size;
    b.currentSize = size;
    b.currentTextSize = TEXT_SIZE;
    b.text = text;
    b.textSize = TEXT_SIZE;
    b.color = color;
    b.hoverSize = (Vector2){ size.x * 0.9f, size.y * 0.9f };
    b.hoverTextSize = TEXT_SIZE * 0.9f;
    b.hoverColor = ColorBrightness(color, -0.2f);
    b.borderColor = WHITE;

    return b;
}

void drawButton(Button* b, CursorState* c) {
    bool hovered = isButtonHovered(b);

    Color drawColor = b->color;
    Vector2 targetSize = b->size;
    int targetTextSize = b->textSize;

    if (hovered) {
        drawColor = b->hoverColor;
        targetSize = b->hoverSize;
        targetTextSize = b->hoverTextSize;
        c->currentType = CUSTOM_CURSOR_POINTER;
    }

    float dt = GetFrameTime();
    b->currentSize = Vector2Lerp(b->currentSize, targetSize, 10.0f * dt);
    b->currentTextSize = Lerp(b->currentTextSize, targetTextSize, 10.0f * dt);
    b->currentSize = Vector2Clamp(b->currentSize, (Vector2) { 0 }, b->size);
    b->currentTextSize = Clamp(b->currentTextSize, 0, b->textSize);

    float offsetX = (b->size.x - b->currentSize.x) / 2.0f;
    float offsetY = (b->size.y - b->currentSize.y) / 2.0f;

    Rectangle btnRect = { b->pos.x + offsetX, b->pos.y + offsetY, b->currentSize.x, b->currentSize.y };

    DrawRectangleRec(btnRect, drawColor);
    DrawRectangleLinesEx(btnRect, 1.0f, b->borderColor);

    int currentTextInt = (int)b->currentTextSize;
    int textWidth = MeasureText(b->text, currentTextInt);

    float textX = btnRect.x + (btnRect.width / 2.0f) - (textWidth / 2.0f);
    float textY = btnRect.y + (btnRect.height / 2.0f) - (currentTextInt / 2.0f);

    DrawText(b->text, (int)textX, (int)textY, currentTextInt, WHITE);
}

bool isButtonHovered(Button* b) {
    Rectangle rect = { b->pos.x, b->pos.y, b->size.x, b->size.y };

    return CheckCollisionPointRec(GetMousePosition(), rect);
}

bool isButtonClicked(Button* b) {
    bool hovered = isButtonHovered(b);

    return hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}