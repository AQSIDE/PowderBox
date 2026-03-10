#include "raylib.h"
#include "cursor.h"

typedef struct Button {
    int id;
    Vector2 pos;
    Vector2 size;
    Vector2 currentSize;
    Vector2 hoverSize;
    char* text;
    int textSize;
    float currentTextSize;
    int hoverTextSize;
    Color color;
    Color hoverColor;
    Color borderColor;
} Button;

Button createButton(int id, Vector2 pos, Vector2 size, char* text, Color color);
bool isButtonHovered(Button* b);
bool isButtonClicked(Button* b);
void drawButton(Button* b, CursorState* c);