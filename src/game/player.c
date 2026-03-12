#include "player.h"
#include "raymath.h"

bool isInBrush(int dx, int dy, Brush* brush) {
    if (brush->type == BRUSH_SQUARE) {
        int halfW = brush->width / 2;
        int halfH = brush->height / 2;

        return (dx >= -halfW && dx <= halfW && dy >= -halfH && dy <= halfH);
    }
    else {
        return (dx * dx + dy * dy <= brush->size * brush->size);
    }
}

void handleMouseClick(Player* player, Map* map, FrameContext* fc) {
    Vector2 point = fc->mapPoint;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        if (point.x == -1 || point.y == -1) return;

        int rangeX = (player->brush.type == BRUSH_SQUARE) ? player->brush.width / 2 : player->brush.size;
        int rangeY = (player->brush.type == BRUSH_SQUARE) ? player->brush.height / 2 : player->brush.size;

        for (int y = -rangeY; y <= rangeY; y++) {
            for (int x = -rangeX; x <= rangeX; x++) {
                if (!isInBrush(x, y, &player->brush)) continue;

                int px = point.x + x;
                int py = point.y + y;

                if (px < 0 || px >= map->width || py < 0 || py >= map->height) continue;

                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    switch (player->brushMode) {
                    case BRUSH_PARTICLE: {
                        if (!player->canReplaceParticles && getPixel(map, px, py)->type != EMPTY) break;
                        setPixel(map, px, py, createParticleFromType(player->selectedParticle));
                        break;
                    }
                    case BRUSH_HEAT: {
                        Particle* t = getPixel(map, px, py);
                        if (t) setTemperature(map, px, py, (uint16_t)Clamp(t->temperature + 1, 0, 5000));
                        break;
                    }
                    case BRUSH_COLD: {
                        Particle* t = getPixel(map, px, py);
                        if (t) setTemperature(map, px, py, (uint16_t)Clamp(t->temperature - 1, 0, 5000));
                        break;
                    }
                    }
                }
                else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    clearPixel(map, px, py);
                }
            }
        }
    }
}

void handleMouseWheel(Player* p, FrameContext* fc) {
    float scroll = fc->wheel;
    if (scroll == 0) return;

    int ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

    if (ctrl) {
        p->targetZoom += scroll * 0.2f;
        if (p->targetZoom < 0.05f) p->targetZoom = 0.05f;
        if (p->targetZoom > 10.0f) p->targetZoom = 10.0f;

        p->camera.offset = fc->screenMouse;
        p->camera.target = fc->worldMouse;
    }

    if (ctrl) return;

    if (scroll > 0) {
        switch (p->brush.type)
        {
        case BRUSH_CIRCLE:
            p->brush.size++;
            if (p->brush.size > 100) (p->brush.size) = 100;
            break;

        case BRUSH_SQUARE:
            p->brush.width++;
            p->brush.height++;
            if (p->brush.width > 100) (p->brush.width) = 100;
            if (p->brush.height > 100) (p->brush.height) = 100;
            break;
        }
    }

    if (scroll < 0) {
        switch (p->brush.type)
        {
        case BRUSH_CIRCLE:
            p->brush.size--;
            if (p->brush.size < 1) (p->brush.size) = 1;
            break;

        case BRUSH_SQUARE:
            p->brush.width--;
            p->brush.height--;
            if (p->brush.width < 1) (p->brush.width) = 1;
            if (p->brush.height < 1) (p->brush.height) = 1;
            break;
        }
    }
}

void handlePlayerInput(Player* player, Simulation* sim, Map* map, CursorState* cur, FrameContext* fc) {
    if (IsKeyPressed(KEY_SPACE)) {
        sim->isPaused = !sim->isPaused;
    }

    if (IsKeyPressed(KEY_R)) {
        player->canReplaceParticles = !player->canReplaceParticles;
    }

    if (IsKeyPressed(KEY_C)) {
        player->brushMode = (player->brushMode + 1) % 3;
    }

    if (IsKeyPressed(KEY_W)) {
        player->brush.type = (player->brush.type + 1) % 2;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        player->currentWindow = UI_NONE;
    }

    if (IsKeyPressed(KEY_F11)) {
        int monitor = GetCurrentMonitor();

        if (!IsWindowFullscreen()) {
            int width = GetMonitorWidth(monitor);
            int height = GetMonitorHeight(monitor);

            SetWindowSize(width, height);
            ToggleFullscreen();

            fc->window->isFullScreen = true;
        }
        else {
            ToggleFullscreen();

            fc->window->width = 1280;
            fc->window->height = 960;
            SetWindowSize(fc->window->width, fc->window->height);

            int scW = GetMonitorWidth(monitor);
            int scH = GetMonitorHeight(monitor);
            SetWindowPosition((scW - 1280) / 2, (scH - 960) / 2);

            fc->window->isFullScreen = false;
        }
    }

    if (IsKeyDown(KEY_Q)) {
        sim->simulationSpeed -= 1.0f * fc->deltaTime;
        if (sim->simulationSpeed < 0.1f) sim->simulationSpeed = 0.1f;
    }
    if (IsKeyDown(KEY_E)) {
        sim->simulationSpeed += 1.0f * fc->deltaTime;
        if (sim->simulationSpeed > 5) sim->simulationSpeed = 5;
    }

    if (!player->isOverUI) {
        cur->currentType = CUSTOM_CURSOR_BRUSH;
    }

    if (!player->isOverUI && player->inputTimer <= 0) {
        handleMouseWheel(player, fc);
        handleMouseClick(player, map, fc);
    }
}

Player createPlayer(int mapW, int mapH) {
    Camera2D camera = { 0 };
    camera.target = (Vector2){ mapW / 2, mapH / 2 };
    camera.offset = (Vector2){ GetScreenWidth() / 2, GetScreenHeight() / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    return (Player) {
        .selectedParticle = SAND,
            .brush = (Brush){ .size = 5, .type = BRUSH_SQUARE, .width = 5, .height = 5 },
            .camera = camera,
            .brushMode = BRUSH_PARTICLE,
            .targetZoom = 1,
            .currentWindow = UI_NONE,
            .inputTimer = 0
    };
}

void updatePlayer(Player* player) {
    player->camera.zoom = Lerp(player->camera.zoom, player->targetZoom, 0.1f);
}
