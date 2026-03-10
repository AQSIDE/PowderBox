#include "raylib.h"
#include "particle.h"
#include "simulation.h"
#include <stdlib.h>
#include <stdint.h>
#include "map.h"
#include "window.h"
#include "game_menu.h"

int main() {
    int monitor = GetCurrentMonitor();
    int monutorW = GetMonitorWidth(monitor);
    int monutorH = GetMonitorHeight(monitor);

    WindowState window = createWindow(monutorW, monutorH);
    Map map = createMap(426, 240);
    Player player = createPlayer(map.width, map.height);
    Simulation sim = createSimulation();
    uint32_t currentFrame = 0;

    const float timeStep = 1.0f / 60.0f;
    float accumulator = 0.0f;

    initGameMenu(&map);

    while (!WindowShouldClose()) {
        window.width = GetScreenWidth();
        window.height = GetScreenHeight();
        window.monitor = GetCurrentMonitor();

        FrameContext fc = (FrameContext){
            .deltaTime = GetFrameTime(),
            .screenMouse = GetMousePosition(),
            .worldMouse = GetScreenToWorld2D(GetMousePosition(), player.camera),
            .wheel = GetMouseWheelMove(),
            .window = &window
        };

        updateGameMenu(&player, &map);

        fc.mapPoint = getPointInMap(fc.worldMouse.x, fc.worldMouse.y, &map);
        fc.pointedParticle = getPixel(&map, fc.mapPoint.x, fc.mapPoint.y);

        updatePlayer(&player);
        handlePlayerInput(&player, &sim, &map, &fc);

        if (player.inputTimer > 0) {
            player.inputTimer -= fc.deltaTime;
        }

        accumulator += fc.deltaTime * sim.simulationSpeed;

        while (accumulator >= timeStep) {
            fc.currentFrame = currentFrame;
            updateSimulation(&sim, &map, &fc);
            currentFrame++;

            accumulator -= timeStep;
        }

        BeginDrawing();
        ClearBackground((Color) { 30, 30, 30, 255 });

        BeginMode2D(player.camera);
        drawMap(&map);
        drawPlayerCrosshair(&player, &fc);
        EndMode2D();

        drawGameMenu(&player, &map, &fc);
        drawGameMenuDebug(&player, &map, &sim, &fc);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}