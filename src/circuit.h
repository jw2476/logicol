#pragma once

#include "base.h"
#include "raylib.h"

typedef enum wire_direction_t wire_direction;
typedef struct circuit_connection_t circuit_connection;
typedef enum circuit_component_type_t circuit_component_type;
typedef struct circuit_component_t circuit_component;
typedef struct circuit_circuit_t circuit_circuit;
typedef struct circuit_library_t circuit_library;

typedef enum wire_direction_t {
    LEFT = -1,
    RIGHT = 1
} wire_direction;

typedef struct circuit_connection_t {
    u64 componentID;
    u32 outputID;
    bool on;
} circuit_connection;

typedef enum circuit_component_type_t {
    AND = 0,
    OR,
    NOT,
    INPUT,
    OUTPUT,
    CUSTOM
} circuit_component_type;

typedef struct circuit_component_t {
    u32 id;
    circuit_component_type type;
    const char* name;

    u32 numInputs;
    circuit_connection* inputs;

    u32 numOutputs;

    Vector2 pos;

    u32 innerID; // Used for custom components
    bool internallyActive; // Used to show if inputs or outputs are on
} circuit_component;

typedef struct circuit_circuit_t {
    char name[100];

    u32 numComponents;
    circuit_component* components;

    u32 nextID;
} circuit_circuit;

circuit_circuit circuit_init();
void circuit_destroy(circuit_circuit* circuit);

circuit_component* circuit_add_component(circuit_circuit* circuit, circuit_component_type type, Vector2 pos);
circuit_component* circuit_add_custom_component(circuit_circuit* circuit, u32 innerID, circuit_library* library, Vector2 pos);
void circuit_connect(circuit_circuit* circuit, circuit_component* from, u32 inputID, circuit_component* to, u32 outputID);

Vector2 get_input_position(circuit_component* component, u32 inputID);
Vector2 get_output_position(circuit_component* component, u32 outputID);

circuit_component* circuit_get_component(circuit_circuit* circuit, u64 id);

typedef struct circuit_library_t {
    u32 numCircuits;
    circuit_circuit* circuits;

    u32 currentCircuitID;
} circuit_library;

circuit_library circuit_library_init();
circuit_circuit* circuit_library_create_circuit(circuit_library* library);
circuit_circuit* get_current_circuit(circuit_library* library);