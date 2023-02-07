#include "raylib.h"
#include "base.h"
#include "draw.h"
#include "circuit.h"
#include "raymath.h"

int main(void) {
    init();

    i32 width = GetScreenWidth();
    i32 height = GetScreenHeight();
    InitWindow(width, height, "raylib [core] example - basic window");
    ToggleFullscreen();

    Camera2D camera;
    CLEAR(camera);

    camera.zoom = 1.0F;

    circuit_circuit circuit = circuit_init();
    circuit_component* and = circuit_add_component(&circuit, AND, (Vector2){ 100, 100 });
    circuit_component* or = circuit_add_component(&circuit, OR, (Vector2){ 1000, 1000 });

    circuit_connection connecting;
    CLEAR(connecting);

    circuit_component* moving = NULL;

    while (!WindowShouldClose())
    {
        // Connections
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 pos = GetMousePosition();
            for (u64 i = 0; i < circuit.numComponents; i++) {
                for (u64 j = 0; j < circuit.components[i].numOutputs; j++) {
                    if (Vector2Distance(pos, Vector2Add(get_output_position(&circuit.components[i], j), (Vector2){ 100, 0 })) < 32.0F) {
                        connecting.component = &circuit.components[i];
                        connecting.outputID = j;
                        break;
                    }
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (connecting.component != NULL)) {
            Vector2 pos = GetMousePosition();
            for (u64 i = 0; i < circuit.numComponents; i++) {
                for (u64 j = 0; j < circuit.components[i].numInputs; j++) {
                    if (Vector2Distance(pos, Vector2Add(get_input_position(&circuit.components[i], j), (Vector2){ -100, 0})) < 32.0F) {
                        circuit_connect(&circuit, &circuit.components[i], j, connecting.component, connecting.outputID);
                        break;
                    }
                }
            }

            CLEAR(connecting);
        }

        // Component Movement
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (u64 i = 0; i < circuit.numComponents; i++) {
                if (Vector2Distance(circuit.components[i].pos, GetMousePosition()) < 32.0F) {
                    moving = &circuit.components[i];
                }
            }
        }

        if (moving != NULL) {
            moving->pos.x += GetMouseDelta().x;
            moving->pos.y += GetMouseDelta().y;
        }

        if ((moving != NULL) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            moving = NULL;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            camera.target.x -= GetMouseDelta().x * (1/camera.zoom);
            camera.target.y -= GetMouseDelta().y * (1/camera.zoom);
        }

        float deltaZoom = (float)GetMouseWheelMove() * 0.05F;
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos;
        camera.zoom += deltaZoom;

        if (camera.zoom > 3.0F) camera.zoom = 3.0F;
        else if (camera.zoom < 0.1F) camera.zoom = 0.1F;

        BeginDrawing();
        BeginMode2D(camera);
        {
            ClearBackground(RAYWHITE);
            draw_circuit(&circuit);

            if (connecting.component) {
                Vector2 pos = GetMousePosition();
                DrawLineBezier(Vector2Add(get_output_position(connecting.component, connecting.outputID), (Vector2){ 100, 0 }), pos, 8.0F, GRAY);
            }
        }
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
