#include "raylib.h"
#include "window.h"
#include "particle.h"
#include <stdint.h>

typedef struct FrameContext {
    Vector2 screenMouse;
    Vector2 worldMouse;
    Vector2 mapPoint;
    Particle* pointedParticle;
    float wheel;
    uint32_t currentFrame;
    float deltaTime;
    WindowState* window;
} FrameContext;