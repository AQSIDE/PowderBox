#include "raylib.h"

typedef struct Button {
    int id;
    Vector2 pos;
    Vector2 size;
    char* text;
    int textSize;
    Color color;
    Color hoverColor;
    Color borderColor;
} Button;

Button createButton(int id, Vector2 pos, Vector2 size, char* text, Color color, Color hoverColor, Color borderColor);
bool isButtonHovered(Button* b);
bool isButtonClicked(Button* b);
void drawButton(Button* b);