#include "raylib.h"
#include "base.h"
#include "draw.h"
#include "circuit.h"
#include "raymath.h"
#include "rlgl.h"
#include "simulate.h"
#include "fs.h"
#include "log.h"

typedef enum simulation_state_t {
    SIMULATION_STATE_OFF,
    SIMULATION_STATE_STEPPING,
    SIMULATION_STATE_RUNNING
} simulation_state;

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

    circuit_library library = circuit_library_init();

    circuit_connection connecting;
    CLEAR(connecting);

    circuit_component* moving = NULL;

    bool insertMode = false;
    bool saveMode = false;
    bool openMode = false;
    u32 inputPtr = 0;
    char inputBuffer[100];
    memset(inputBuffer, 0, sizeof(char) * 100);

    simulation_state simulationState = SIMULATION_STATE_OFF;

    while (!WindowShouldClose())
    {
        Vector2 cursorPos = GetScreenToWorld2D(GetMousePosition(), camera);

        // Connections
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (u64 i = 0; i < get_current_circuit(&library)->numComponents; i++) {
                for (u64 j = 0; j < get_current_circuit(&library)->components[i].numOutputs; j++) {
                    if (Vector2Distance(cursorPos, Vector2Add(get_output_position(&get_current_circuit(&library)->components[i], j), (Vector2){ 100, 0 })) < 32.0F) {
                        connecting.componentID = get_current_circuit(&library)->components[i].id;
                        connecting.outputID = j;
                        break;
                    }
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (connecting.componentID != 0)) {
            for (u64 i = 0; i < get_current_circuit(&library)->numComponents; i++) {
                for (u64 j = 0; j < get_current_circuit(&library)->components[i].numInputs; j++) {
                    if (Vector2Distance(cursorPos, Vector2Add(get_input_position(&get_current_circuit(&library)->components[i], j), (Vector2){ -100, 0})) < 32.0F) {
                        circuit_connect(get_current_circuit(&library), &get_current_circuit(&library)->components[i], j, connecting.componentID, connecting.outputID);
                        break;
                    }
                }
            }

            CLEAR(connecting);
        }

        // Component Movement
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (u64 i = 0; i < get_current_circuit(&library)->numComponents; i++) {
                if (Vector2Distance(get_current_circuit(&library)->components[i].pos, cursorPos) < 32.0F) {
                    moving = &get_current_circuit(&library)->components[i];
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
        if (saveMode || insertMode || openMode) {
            char character;
            while ((character = (char)GetCharPressed()) != 0) {
                inputBuffer[inputPtr++] = character;
            }
        }

        if (!IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_I) && !saveMode && !openMode) {
            insertMode = true;
            saveMode = false;
            memset(inputBuffer, 0, sizeof(char) * 100);
            inputPtr = 0;
        }

        if (insertMode && IsKeyPressed(KEY_ENTER)) {
            for (u32 i = 0; i < library.numCircuits; i++) {
                if (strcmp(library.circuits[i].name, inputBuffer) == 0) {
                    circuit_add_custom_component(get_current_circuit(&library), i, &library, cursorPos);
                }
            }

            insertMode = false;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                if (IsKeyPressed(KEY_I)) {
                    circuit_add_component(get_current_circuit(&library), INPUT, cursorPos);
                }

                if (IsKeyPressed(KEY_O)) {
                    circuit_add_component(get_current_circuit(&library), OUTPUT, cursorPos);
                }

                if (IsKeyPressed(KEY_A)) {
                    circuit_add_component(get_current_circuit(&library), NAND, cursorPos);
                }
            } else {
                if (IsKeyPressed(KEY_A)) {
                    circuit_add_component(get_current_circuit(&library), AND, cursorPos);
                }

                if (IsKeyPressed(KEY_O)) {
                    circuit_add_component(get_current_circuit(&library), OR, cursorPos);
                }

                if (IsKeyPressed(KEY_N)) {
                    circuit_add_component(get_current_circuit(&library), NOT, cursorPos);
                }

                if (IsKeyPressed(KEY_B)) {
                    circuit_add_component(get_current_circuit(&library), BUFFER, cursorPos);
                }
            }
        }

        // Nandify
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_N)) {
            circuit_nandify(get_current_circuit(&library));
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
            for (u64 i = 0; i < get_current_circuit(&library)->numComponents; i++) {
                if (Vector2Distance(get_current_circuit(&library)->components[i].pos, cursorPos) < 32.0F) {
                    if (get_current_circuit(&library)->components[i].type == INPUT) {
                        get_current_circuit(&library)->components[i].internallyActive = !get_current_circuit(&library)->components[i].internallyActive;
                    }
                }
            }
        }

        bool hasName = strlen(get_current_circuit(&library)->name) != 0;
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_S)) {
            if (!hasName || IsKeyDown(KEY_LEFT_SHIFT)) {
                saveMode = true;
                inputPtr = 0;
                memset(inputBuffer, 0, sizeof(char) * 100);
            } else {
                char buffer[100];
                sprintf(buffer, "../output/%s.circuit", get_current_circuit(&library)->name);
                save_circuit(get_current_circuit(&library), buffer);
            }
        }

        if (saveMode && IsKeyPressed(KEY_ENTER)) {
            memcpy(get_current_circuit(&library)->name, inputBuffer, inputPtr + 1); // Rename component

            char buffer[100];
            sprintf(buffer, "../output/%s.circuit", get_current_circuit(&library)->name);
            save_circuit(get_current_circuit(&library), buffer);
            saveMode = false;
        }

        // Open
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_O)) {
            openMode = true;
            inputPtr = 0;
            memset(inputBuffer, 0, sizeof(char) * 100);
        }

        // New
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_N)) {
            circuit_library_create_circuit(&library);
            library.currentCircuitID = library.numCircuits - 1;
        }

        if (openMode && IsKeyPressed(KEY_ENTER)) {
            // Check if component is already in library
            bool found = false;
            for (u32 i = 0; i < library.numCircuits; i++) {
                if (strcmp(inputBuffer, library.circuits[i].name) == 0) {
                    library.currentCircuitID = i;
                    found = true;
                }
            }

            if (!found) {
                // TODO: Check if file exists

                circuit_library_create_circuit(&library);
                library.currentCircuitID = library.numCircuits - 1;

                load_circuit(&library, inputBuffer); // Load circuit into new slot
            }

            openMode = false;
        }

        // Simulation State
        if (IsKeyPressed(KEY_SPACE)) {
            if (simulationState == SIMULATION_STATE_RUNNING) {
                simulationState = SIMULATION_STATE_OFF;
                INFO("Paused");
            } else {
                simulationState = SIMULATION_STATE_RUNNING;
                INFO("Running");
            }
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            simulationState = SIMULATION_STATE_STEPPING;
            simulate(get_current_circuit(&library), &library);
            INFO("Ticked");
        }

        // TPS Counter
        double now = GetTime();
        double nextFrame = now + 1 / 60.0f;
        u32 ticks = 0;
        if (simulationState == SIMULATION_STATE_RUNNING) {
            while (GetTime() < nextFrame) {
                for (u32 j = 0; j < 99; j++) {
                    simulate(get_current_circuit(&library), &library);
                    ticks++;
                }
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

            if (IsKeyDown(KEY_T)) {
                char buffer[20];
                sprintf(buffer, "%d", ticks * 60);
                DrawText(buffer, (int)cursorPos.x + 32, (int)cursorPos.y + 32, (int)(24 / camera.zoom), GREEN);
            }

            draw_circuit(get_current_circuit(&library));

            if (connecting.componentID != 0) {
                DrawLineBezier(Vector2Add(get_output_position(circuit_get_component(get_current_circuit(&library), connecting.componentID), connecting.outputID), (Vector2){ 100, 0 }), cursorPos, 8.0F, CONNECTION);
            }
        }
        EndMode2D();
        {
            char textBuffer[100];
            memset(textBuffer, 0, 100);

            if (saveMode) {
                sprintf(textBuffer, "Save as: %s\n", inputBuffer);
            }

            if (insertMode) {
                sprintf(textBuffer, "Insert: %s\n", inputBuffer);
            }

            if (openMode) {
                sprintf(textBuffer, "Open: %s\n", inputBuffer);
            }

            if (!insertMode && !saveMode && !openMode) {
                sprintf(textBuffer, "Editing: %s\n", get_current_circuit(&library)->name);
            }

            show_messages();

            DrawText(textBuffer, 32, 32, 48, BLUE);
        }
        EndDrawing();

    }

    CloseWindow();

    return 0;
}
