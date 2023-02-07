#include "raylib.h"
#include "base.h"
#include "draw.h"
#include "circuit.h"
#include "raymath.h"
#include "rlgl.h"
#include "simulate.h"

int main(void) {
    init();

    i32 width = GetScreenWidth();
    i32 height = GetScreenHeight();
    InitWindow(width, height, "raylib [core] example - basic window");
    ToggleFullscreen();

    SetTargetFPS(60);

    Camera2D camera;
    CLEAR(camera);

    camera.zoom = 1.0F;

    circuit_circuit circuit = circuit_init();

    u32 numCircuits = 0;
    circuit_circuit* library = malloc(0);

    circuit_connection connecting;
    CLEAR(connecting);

    circuit_component* moving = NULL;

    bool makingSubcomponent = false;
    u32 componentNamePtr = 0;

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
                        connecting.componentID = circuit.components[i].id;
                        connecting.outputID = j;
                        break;
                    }
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (connecting.componentID != 0)) {
            for (u64 i = 0; i < circuit.numComponents; i++) {
                for (u64 j = 0; j < circuit.components[i].numInputs; j++) {
                    if (Vector2Distance(cursorPos, Vector2Add(get_input_position(&circuit.components[i], j), (Vector2){ -100, 0})) < 32.0F) {
                        circuit_connect(&circuit, &circuit.components[i], j, circuit_get_component(&circuit, connecting.componentID), connecting.outputID);
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

        if (!IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_I) && !inserting) {
            inserting = true;
            memset(insertionBuffer, 0, sizeof(char) * 100);
            insertionBufferPtr = 0;
        }

        if (inserting && IsKeyPressed(KEY_ENTER)) {
            for (u32 i = 0; i < numCircuits; i++) {
                if (strcmp(library[i].name, insertionBuffer) == 0) {
                    circuit_add_custom_component(&circuit, i, library, cursorPos);
                }
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

        // Input Toggling
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (u64 i = 0; i < circuit.numComponents; i++) {
                if (Vector2Distance(circuit.components[i].pos, cursorPos) < 32.0F) {
                    if (circuit.components[i].type == INPUT) {
                        circuit.components[i].internallyActive = !circuit.components[i].internallyActive;
                    }
                }
            }
        }

        // Subcomponents
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
            makingSubcomponent = true;
            componentNamePtr = 0;
            CLEAR(componentNamePtr);
        }

        if (makingSubcomponent) {
            char character;
            while ((character = (char)GetCharPressed()) != 0) {
                circuit.name[componentNamePtr++] = character;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                numCircuits++;
                library = realloc(library, sizeof(circuit_circuit) * numCircuits);
                library[numCircuits - 1] = circuit;
                circuit = circuit_init();
                makingSubcomponent = false;
            }
        }

        // Drawing
        BeginDrawing();
        BeginMode2D(camera);
        {
            ClearBackground(DARK);

            if (IsKeyDown(KEY_F)) {
                DrawFPS((int)cursorPos.x + 32, (int)cursorPos.y + 32);
            }

            draw_circuit(&circuit);

            if (connecting.componentID != 0) {
                DrawLineBezier(Vector2Add(get_output_position(circuit_get_component(&circuit, connecting.componentID), connecting.outputID), (Vector2){ 100, 0 }), cursorPos, 8.0F, CONNECTION);
            }

            if (makingSubcomponent) {
                Vector2 pos = GetScreenToWorld2D((Vector2){ 32, 32 }, camera);
                DrawText(circuit.name, (int)pos.x, (int)pos.y, (int)(48 / camera.zoom), BLUE);
            }

            if (inserting) {
                Vector2 pos = GetScreenToWorld2D((Vector2){ 32, 32 }, camera);
                DrawText(insertionBuffer, (int)pos.x, (int)pos.y, (int)(48 / camera.zoom), BLUE);
            }
        }
        EndMode2D();
        EndDrawing();

        simulate(&circuit, library);
    }

    CloseWindow();

    return 0;
}
