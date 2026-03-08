#include "window.h"
#include "raylib.h"

WindowState createWindow(int width, int height) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(width, height, "Sand Game");
    SetExitKey(KEY_NULL);

    ToggleFullscreen();

    SetWindowMinSize(640, 480);

    return (WindowState) { .width = width, .height = height, .isFullScreen = true };
}