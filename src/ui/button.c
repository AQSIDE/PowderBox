#include "button.h"

Button createButton(int id, Vector2 pos, Vector2 size, char* text, Color color, Color hoverColor, Color borderColor) {
    Button b;
    b.id = id;
    b.pos = pos;
    b.size = size;
    b.text = text;
    b.textSize = 20;
    b.color = color;
    b.hoverColor = hoverColor;
    b.borderColor = borderColor;

    return b;
}

void drawButton(Button* b) {
    bool hovered = isButtonHovered(b);

    Color drawColor = b->color;
    if (hovered) {
        drawColor = b->hoverColor;
    }

    DrawRectangle(b->pos.x, b->pos.y, b->size.x, b->size.y, drawColor);

    Rectangle btnRect = { b->pos.x, b->pos.y, b->size.x, b->size.y };
    DrawRectangleLinesEx(btnRect, 1.0f, b->borderColor);

    int textWidth = MeasureText(b->text, b->textSize);

    float textX = b->pos.x + (b->size.x / 2.0f) - (textWidth / 2.0f);
    float textY = b->pos.y + (b->size.y / 2.0f) - (b->textSize / 2.0f);
    DrawText(b->text, (int)textX, (int)textY, b->textSize, WHITE);
}

bool isButtonHovered(Button* b) {
    Rectangle rect = { b->pos.x, b->pos.y, b->size.x, b->size.y };

    return CheckCollisionPointRec(GetMousePosition(), rect);
}

bool isButtonClicked(Button* b) {
    bool hovered = isButtonHovered(b);

    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}