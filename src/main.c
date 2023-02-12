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
    InitWindow(640, 480, "raylib [core] example - basic window");
//    ToggleFullscreen();

    SetTargetFPS(60);

    Camera2D camera;
    CLEAR(camera);
    camera.zoom = 1.0F;

    circuit_library library = circuit_library_init();

    circuit_component* connectionComponent = NULL;
    u32 connectionOutput = 0;

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
            ITERATE(circuit_graph_node_list, library.current->components->nodes, nodeItem) {
                if (nodeItem->data == NULL) break;
                circuit_component* component = nodeItem->data->data;

                for (u64 j = 0; j < component->numOutputs; j++) {
                    if (Vector2Distance(cursorPos, Vector2Add(get_output_position(component, j), (Vector2){ 100, 0 })) < 32.0F) {
                        connectionComponent = component;
                        connectionOutput = j;
                        break;
                    }
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && connectionComponent != NULL) {
            ITERATE(circuit_graph_node_list, library.current->components->nodes, nodeItem) {
                if (nodeItem->data == NULL) break;
                circuit_component* component = nodeItem->data->data;

                u32 numInputs = circuit_graph_edge_list_length(nodeItem->data->edges);

                for (u32 j = 0; j < numInputs; j++) {
                    if (Vector2Distance(cursorPos, Vector2Add(get_input_position(library.current, component, j), (Vector2){ -100, 0})) < 32.0F) {
                        circuit_connect(library.current, component, j, connectionComponent, connectionOutput);
                        break;
                    }
                }
            }

            connectionComponent = NULL;
            connectionOutput = 0;
        }

        // Component Movement
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (circuit_graph_node_list* nodeItem = library.current->components->nodes; nodeItem != NULL; nodeItem = nodeItem->next) {
                if (nodeItem->data == NULL) break;

                circuit_component* component = nodeItem->data->data;
                if (Vector2Distance(component->pos, cursorPos) < 32.0F) {
                    moving = component;
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
            for (circuit_circuit_list* item = library.circuits; item != NULL; item = item->next) {
                circuit_circuit* circuit = item->data;
                if (strcmp(circuit->name, inputBuffer) == 0) {
                    circuit_add_custom_component(library.current, circuit, cursorPos);
                }
            }

            insertMode = false;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                if (IsKeyPressed(KEY_I)) {
                    circuit_add_component(library.current, INPUT, cursorPos);
                }

                if (IsKeyPressed(KEY_O)) {
                    circuit_add_component(library.current, OUTPUT, cursorPos);
                }

                if (IsKeyPressed(KEY_A)) {
                    circuit_add_component(library.current, NAND, cursorPos);
                }
            } else {
                if (IsKeyPressed(KEY_A)) {
                    circuit_add_component(library.current, AND, cursorPos);
                }

                if (IsKeyPressed(KEY_O)) {
                    circuit_add_component(library.current, OR, cursorPos);
                }

                if (IsKeyPressed(KEY_N)) {
                    circuit_add_component(library.current, NOT, cursorPos);
                }

                if (IsKeyPressed(KEY_B)) {
                    circuit_add_component(library.current, BUFFER, cursorPos);
                }
            }
        }

        // Optimization Keybinds
//        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_E)) {
//            circuit_embed_custom_components(&library, library.current);
//            INFO("Embedded custom components. DO NOT SAVE");
//        }
//
//        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_N)) {
//            circuit_nandify(&library, library.current);
//            INFO("Nandified all in-memory circuits. DO NOT SAVE");
//        }

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
            ITERATE(circuit_graph_node_list, library.current->components->nodes, nodeItem) {
                if (nodeItem->data == NULL) break;
                circuit_component* component = nodeItem->data->data;

                if (Vector2Distance(component->pos, cursorPos) < 32.0F) {
                    if (component->type == INPUT) {
                        component->internallyActive = !component->internallyActive;
                    }
                }
            }
        }

        bool hasName = strlen(library.current->name) != 0;
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_S)) {
            if (!hasName || IsKeyDown(KEY_LEFT_SHIFT)) {
                saveMode = true;
                inputPtr = 0;
                memset(inputBuffer, 0, sizeof(char) * 100);
            } else {
                char buffer[100];
                sprintf(buffer, "../output/%s.circuit", library.current->name);
                save_circuit(library.current, buffer);
            }
        }

        if (saveMode && IsKeyPressed(KEY_ENTER)) {
            memcpy(library.current->name, inputBuffer, inputPtr + 1); // Rename component

            char buffer[100];
            sprintf(buffer, "../output/%s.circuit", library.current->name);
            save_circuit(library.current, buffer);
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
            library.current = circuit_library_create_circuit(&library);
        }

        if (openMode && IsKeyPressed(KEY_ENTER)) {
            // Check if component is already in library
            bool found = false;
            for (circuit_circuit_list* item = library.circuits; item != NULL; item = item->next) {
                circuit_circuit* circuit = item->data;
                if (strcmp(inputBuffer, circuit->name) == 0) {
                    library.current = circuit;
                    found = true;
                }
            }

            if (!found) {
                library.current = circuit_library_create_circuit(&library);

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
            simulate(library.current);
            INFO("Ticked");
        }

        // TPS Counter
        double now = GetTime();
        double nextFrame = now + 1 / 60.0f;
        u32 ticks = 0;
        if (simulationState == SIMULATION_STATE_RUNNING) {
            while (GetTime() < nextFrame) {
                for (u32 j = 0; j < 99; j++) {
                    simulate(library.current);
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

            draw_circuit(library.current);

            if (connectionComponent) {
                DrawLineBezier(Vector2Add(get_output_position(connectionComponent, connectionOutput), (Vector2){ 100, 0 }), cursorPos, 8.0F, CONNECTION);
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
                sprintf(textBuffer, "Editing: %s\n", library.current->name);
            }

            show_messages();

            DrawText(textBuffer, 32, 32, 48, BLUE);
        }
        EndDrawing();

    }

    CloseWindow();

    return 0;
}
