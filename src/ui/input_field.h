#include "raylib.h"

typedef struct InputField {
    int id;
    Vector2 pos;
    Vector2 size;
    char* label;
    char charBuffer[16];
    int letterCount;
    bool active;
    int textSize;
    Color color;
} InputField;

InputField createInputField(int id, Vector2 pos, Vector2 size, char* label, Color color);

void drawInputField(InputField* f);
void updateInputField(InputField* f);