#pragma once

#include "base.h"
#include "raylib.h"

typedef enum wire_direction_t wire_direction;
typedef struct circuit_connection_t circuit_connection;
typedef enum circuit_component_type_t circuit_component_type;
typedef struct circuit_component_t circuit_component;
typedef struct circuit_circuit_t circuit_circuit;

typedef enum wire_direction_t {
    LEFT = -1,
    RIGHT = 1
} wire_direction;

typedef struct circuit_connection_t {
    circuit_component* component;
    u32 outputID;
} circuit_connection;

typedef enum circuit_component_type_t {
    AND,
    OR
} circuit_component_type;

typedef struct circuit_component_t {
    u64 id;
    circuit_component_type type;

    u32 numInputs;
    circuit_connection* inputs;

    u32 numOutputs;

    Vector2 pos;
} circuit_component;

typedef struct circuit_circuit_t {
    u64 numComponents;
    circuit_component* components;
} circuit_circuit;

circuit_circuit circuit_init();
void circuit_destroy(circuit_circuit* circuit);

circuit_component* circuit_add_component(circuit_circuit* circuit, circuit_component_type type, Vector2 pos);
void circuit_connect(circuit_circuit* circuit, circuit_component* from, u32 inputID, circuit_component* to, u32 outputID);

Vector2 get_input_position(circuit_component* component, u32 inputID);
Vector2 get_output_position(circuit_component* component, u32 outputID);
