#include "raylib.h"
#include "base.h"
#include "draw.h"
#include "circuit.h"
#include "raymath.h"
#include "rlgl.h"

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

    bool inserting = false;
    u32 insertionBufferPtr = 0;
    char insertionBuffer[100];
    memset(insertionBuffer, 0, sizeof(char) * 100);

    while (!WindowShouldClose())
    {
        Vector2 cursorPos = GetScreenToWorld2D(GetMousePosition(), camera);


        // Connections
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (u64 i = 0; i < circuit.numComponents; i++) {
                for (u64 j = 0; j < circuit.components[i].numOutputs; j++) {
                    if (Vector2Distance(cursorPos, Vector2Add(get_output_position(&circuit.components[i], j), (Vector2){ 100, 0 })) < 32.0F) {
                        connecting.component = &circuit.components[i];
                        connecting.outputID = j;
                        break;
                    }
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (connecting.component != NULL)) {
            for (u64 i = 0; i < circuit.numComponents; i++) {
                for (u64 j = 0; j < circuit.components[i].numInputs; j++) {
                    if (Vector2Distance(cursorPos, Vector2Add(get_input_position(&circuit.components[i], j), (Vector2){ -100, 0})) < 32.0F) {
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
                if (Vector2Distance(circuit.components[i].pos, cursorPos) < 32.0F) {
                    moving = &circuit.components[i];
                }
            }
        }

        if (moving != NULL) {
            moving->pos.x += GetMouseDelta().x / camera.zoom;
            moving->pos.y += GetMouseDelta().y / camera.zoom;
        }

        if ((moving != NULL) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            moving = NULL;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            camera.target.x -= GetMouseDelta().x * (1/camera.zoom);
            camera.target.y -= GetMouseDelta().y * (1/camera.zoom);
        }

        // Component Insertion
        if (inserting) {
            char character;
            while ((character = (char)GetCharPressed()) != 0) {
                insertionBuffer[insertionBufferPtr++] = character;
            }
        }

        if (IsKeyPressed(KEY_I) && !inserting) {
            inserting = true;
            memset(insertionBuffer, 0, sizeof(char) * 100);
            insertionBufferPtr = 0;
        }

        if (inserting && IsKeyPressed(KEY_ENTER)) {
            if (strcmp(insertionBuffer, "AND") == 0) {
                circuit_add_component(&circuit, AND, cursorPos);
            } else if (strcmp(insertionBuffer, "OR") == 0) {
                circuit_add_component(&circuit, OR, cursorPos);
            }

            inserting = false;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_A)) {
            circuit_add_component(&circuit, AND, cursorPos);
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && !IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_O)) {
            circuit_add_component(&circuit, OR, cursorPos);
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && !IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_N)) {
            circuit_add_component(&circuit, NOT, cursorPos);
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_I)) {
            circuit_add_component(&circuit, INPUT, cursorPos);
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_O)) {
            circuit_add_component(&circuit, OUTPUT, cursorPos);
        }

        // Zooming
        float deltaZoom = (float)GetMouseWheelMove() * 0.05F;
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos;
        camera.zoom += deltaZoom;

        if (camera.zoom > 3.0F) camera.zoom = 3.0F;
        else if (camera.zoom < 0.1F) camera.zoom = 0.1F;

        // Drawing
        BeginDrawing();
        BeginMode2D(camera);
        {
            ClearBackground(DARK);

            if (IsKeyDown(KEY_F)) {
                DrawFPS((int)cursorPos.x + 32, (int)cursorPos.y + 32);
            }

            draw_circuit(&circuit);

            if (connecting.component) {
                DrawLineBezier(Vector2Add(get_output_position(connecting.component, connecting.outputID), (Vector2){ 100, 0 }), cursorPos, 8.0F, CONNECTION);
            }

            if (inserting) {
                Vector2 pos = GetScreenToWorld2D((Vector2){ 32, 32 }, camera);
                DrawText(insertionBuffer, (int)pos.x, (int)pos.y, 48, BLUE);
            }
        }
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
