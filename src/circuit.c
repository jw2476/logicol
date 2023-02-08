#include "circuit.h"
#include "raymath.h"

void circuit_component_destroy(circuit_component* component) {
    free(component->inputs);
}

circuit_circuit circuit_init() {
    circuit_circuit circuit;
    CLEAR(circuit);

    circuit.components = malloc(0);
    circuit.nextID = 1;

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
    component->id = circuit->nextID++;
    component->type = type;

    switch (type) {
        case AND:
            component->name = "AND";
            break;
        case OR:
            component->name = "OR";
            break;
        case NOT:
            component->name = "NOT";
            break;
        case INPUT:
            component->name = "IN";
            break;
        case OUTPUT:
            component->name = "OUT";
            break;
    }

    switch (type) {
        case AND:
        case OR:
            component->numInputs = 2;
            break;
        case NOT:
        case OUTPUT:
            component->numInputs = 1;
            break;
        case INPUT:
            component->numInputs = 0;
            break;
    }

    switch (type) {
        case AND:
        case OR:
        case NOT:
        case INPUT:
            component->numOutputs = 1;
            break;
        case OUTPUT:
            component->numOutputs = 0;
            break;
    }

    component->inputs = malloc(sizeof(circuit_connection) * component->numInputs);
    memset(component->inputs, 0, sizeof(circuit_connection) * component->numInputs);

    component->pos = pos;

    return component;
}

circuit_component* circuit_add_custom_component(circuit_circuit* circuit, u32 innerID, circuit_library* library, Vector2 pos) {
    circuit->numComponents++;
    circuit->components = realloc(circuit->components, sizeof(circuit_component) * circuit->numComponents);
    circuit_component* component = &circuit->components[circuit->numComponents - 1];
    CLEAR(*component);
    component->id = circuit->nextID++;
    component->type = CUSTOM;
    component->name = library->circuits[innerID].name;

    for (u64 i = 0; i < library->circuits[innerID].numComponents; i++) {
        if (library->circuits[innerID].components[i].type == INPUT) {
            component->numInputs++;
        }
        else if (library->circuits[innerID].components[i].type == OUTPUT) {
            component->numOutputs++;
        }
    }

    component->inputs = malloc(sizeof(circuit_connection) * component->numInputs);
    memset(component->inputs, 0, sizeof(circuit_connection) * component->numInputs);

    component->pos = pos;
    component->innerID = innerID;

    return component;
}

void circuit_connect(circuit_circuit* circuit, circuit_component* from, u32 inputID, u32 toID, u32 outputID) {
    from->inputs[inputID].componentID = toID;
    from->inputs[inputID].outputID = outputID;
}

Vector2 get_input_position(circuit_component* component, u32 inputID) {
    Vector2 base;
    if (component->numInputs == 1) {
        base =(Vector2){ 0, 100 };
    } else {
        base = (Vector2){0, 50.0F + ((float) inputID * (100.0F / (float) (component->numInputs - 1)))};
    }

    if (component->type == OR) {
        base.x += 35;
    }

    return Vector2Add(base, component->pos);
}

Vector2 get_output_position(circuit_component* component, u32 outputID) {
    Vector2 base;
    if (component->numOutputs == 1) {
        base =(Vector2){ 250, 100 };
    } else {
        base = (Vector2){250, 50.0F + ((float) outputID * (100.0F / (float) (component->numOutputs - 1)))};
    }

    return Vector2Add(base, component->pos);
}

circuit_component* circuit_get_component(circuit_circuit* circuit, u64 id) {
    for (u64 i = 0; i < circuit->numComponents; i++) {
        if (circuit->components[i].id == id) {
            return &circuit->components[i];
        }
    }

    CRITICAL("Couldn't find component with ID: %d in %s", id, circuit->name);
}

circuit_library circuit_library_init() {
    circuit_library library;
    CLEAR(library);

    library.numCircuits = 1;
    library.circuits = malloc(sizeof(circuit_circuit));
    library.circuits[0] = circuit_init();

    return library;
}

circuit_circuit* circuit_library_create_circuit(circuit_library* library) {
    library->numCircuits++;
    library->circuits = realloc(library->circuits, sizeof(circuit_circuit) * library->numCircuits);
    library->circuits[library->numCircuits - 1] = circuit_init();

    return &library->circuits[library->numCircuits - 1];
}

circuit_circuit* get_current_circuit(circuit_library* library) {
    return &library->circuits[library->currentCircuitID];
}