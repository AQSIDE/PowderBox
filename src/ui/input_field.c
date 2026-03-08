#include "input_field.h"

InputField createInputField(int id, Vector2 pos, Vector2 size, char* label, Color color) {
    InputField f = { 0 };
    f.id = id;
    f.pos = pos;
    f.size = size;
    f.label = label;
    f.color = color;
    f.textSize = 20;
    f.charBuffer[0] = '\0';
    f.letterCount = 0;
    f.active = false;
    return f;
}

void drawInputField(InputField* f) {
    Color borderColor = f->active ? GOLD : DARKGRAY;
    float borderThickness = f->active ? 2.0f : 1.0f;

    Rectangle rect = { f->pos.x, f->pos.y, f->size.x, f->size.y };

    DrawRectangleRec(rect, (Color) { 30, 30, 30, 255 });
    DrawRectangleLinesEx(rect, borderThickness, borderColor);

    DrawText(f->charBuffer, (int)f->pos.x + 8, (int)f->pos.y + (f->size.y / 2) - (f->textSize / 2), f->textSize, WHITE);

    int labelWidth = MeasureText(f->label, f->textSize);
    DrawText(f->label, (int)f->pos.x - labelWidth - 15, (int)f->pos.y + (f->size.y / 2) - (f->textSize / 2), f->textSize, LIGHTGRAY);

    if (f->active) {
        if (((int)(GetTime() * 2) % 2) == 0) {
            int textWidth = MeasureText(f->charBuffer, f->textSize);
            DrawRectangle((int)f->pos.x + 10 + textWidth, (int)f->pos.y + 8, 2, (int)f->size.y - 16, GOLD);
        }
    }
}

void updateInputField(InputField* f) {
    Rectangle rect = { f->pos.x, f->pos.y, f->size.x, f->size.y };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), rect)) {
            f->active = true;
        }
        else {
            f->active = false;
        }
    }

    if (f->active) {
        int key = GetCharPressed();

        while (key > 0) {
            if ((key >= '0') && (key <= '9') && (f->letterCount < 5)) {
                f->charBuffer[f->letterCount] = (char)key;
                f->charBuffer[f->letterCount + 1] = '\0';
                f->letterCount++;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (f->letterCount > 0) {
                f->letterCount--;
                f->charBuffer[f->letterCount] = '\0';
            }
        }
    }
}