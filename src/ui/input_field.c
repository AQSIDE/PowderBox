#include "input_field.h"
#include "string.h"

InputField createInputField(int id, Vector2 pos, Vector2 size, const char* label, Color color) {
    InputField f = { 0 };
    f.id = id;
    f.pos = pos;
    f.size = size;
    strncpy(f.label, label, sizeof(f.label) - 1);
    f.label[sizeof(f.label) - 1] = '\0';

    f.color = color;
    f.textSize = 20;
    f.charBuffer[0] = '\0';
    f.letterCount = 0;
    f.active = false;
    f.onlyNumbers = false;
    f.maxLetters = sizeof(f.charBuffer) - 1;
    return f;
}

void drawInputField(InputField* f, CursorState* c) {
    Color borderColor = f->active ? GOLD : DARKGRAY;
    float borderThickness = f->active ? 2.0f : 1.0f;

    if (isHoverInputField(f)) {
        c->currentType = CUSTOM_CURSOR_TEXT;
    }

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
        f->active = CheckCollisionPointRec(GetMousePosition(), rect);
    }

    if (f->active) {
        int key = GetCharPressed();
        while (key > 0) {
            bool valid = true;

            if (f->onlyNumbers) {
                valid = (key >= '0' && key <= '9');
            }

            if (valid && f->letterCount < f->maxLetters) {
                f->charBuffer[f->letterCount] = (char)key;
                f->letterCount++;
                f->charBuffer[f->letterCount] = '\0';
            }

            key = GetCharPressed();
        }

        if ((IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE)) && f->letterCount > 0) {
            f->letterCount--;
            f->charBuffer[f->letterCount] = '\0';
        }
    }
}

void setInputFieldText(InputField* f, const char* text) {
    if (!f || !text) return;

    strncpy(f->charBuffer, text, f->maxLetters);
    f->charBuffer[f->maxLetters] = '\0';

    f->letterCount = (int)strlen(f->charBuffer);
}

bool isHoverInputField(InputField* f) {
    Rectangle rect = { f->pos.x, f->pos.y, f->size.x, f->size.y };

    return CheckCollisionPointRec(GetMousePosition(), rect);
}