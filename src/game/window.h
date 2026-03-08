#include <stdint.h>

typedef struct WindowState {
    int width;
    int height;
    int monitor;
    int8_t isFullScreen;
} WindowState;

WindowState createWindow(int width, int height);