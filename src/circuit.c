#include "circuit.h"
#include "raymath.h"

void circuit_component_destroy(circuit_component* component) {
    free(component->inputs);
}

circuit_circuit circuit_init() {
    circuit_circuit circuit;
    CLEAR(circuit);

    circuit.components = malloc(0);

    return circuit;
}

void circuit_destroy(circuit_circuit* circuit) {
    for (u32 i = 0; i < circuit->numComponents; i++) {
        circuit_component_destroy(&circuit->components[i]);
    }
    free(circuit->components);
}

circuit_component* circuit_add_component(circuit_circuit* circuit, circuit_component_type type, Vector2 pos) {
    circuit->numComponents++;
    circuit->components = realloc(circuit->components, sizeof(circuit_component) * circuit->numComponents);
    circuit_component* component = &circuit->components[circuit->numComponents - 1];
    CLEAR(*component);
    component->id = ((u64)rand() << 32) | (u64)rand();
    component->type = type;

    component->numInputs = 2;
    component->numOutputs = 1;

    component->inputs = malloc(sizeof(circuit_connection) * component->numInputs);
    memset(component->inputs, 0, sizeof(circuit_connection) * component->numInputs);

    component->pos = pos;

    return component;
}

void circuit_connect(circuit_circuit* circuit, circuit_component* from, u32 inputID, circuit_component* to, u32 outputID) {
    from->inputs[inputID].component = to;
    from->inputs[inputID].outputID = outputID;
}

Vector2 get_input_position(circuit_component* component, u32 inputID) {
    Vector2 base = { 0, 50.0F + ((float)inputID * 100.0F) };
    if (component->type == OR) {
        base.x += 35;
    }

    return Vector2Add(base, component->pos);
}

Vector2 get_output_position(circuit_component* component, u32 outputID) {
    Vector2 base = (Vector2){ 250, 100 };
    return Vector2Add(base, component->pos);
}