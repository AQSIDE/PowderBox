#include "raylib.h"

typedef struct InputField {
    int id;
    Vector2 pos;
    Vector2 size;

    char label[32];
    char charBuffer[64];
    int maxLetters;
    int letterCount;

    bool active;
    bool onlyNumbers;
    int textSize;
    Color color;
} InputField;

InputField createInputField(int id, Vector2 pos, Vector2 size, const char* label, Color color);

void drawInputField(InputField* f);
void updateInputField(InputField* f);
void setInputFieldText(InputField* f, const char* text);