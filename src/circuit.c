#include "circuit.h"
#include "raymath.h"

void circuit_init(circuit_circuit* circuit) {
    CLEAR(*circuit);
    circuit->components = circuit_graph_new();
}

//void circuit_destroy(circuit_circuit *circuit) {
//    for (u32 i = 0; i < circuit->numComponents; i++) {
//        circuit_component_destroy(&circuit->components[i]);
//    }
//    free(circuit->components);
//}

void make_inputs(circuit_circuit* circuit, circuit_component* component, u32 numInputs) {
    for (u32 i = 0; i < numInputs; i++) {
        circuit_graph_connect(circuit->components, component, NULL, NULL);
    }
}

circuit_component* circuit_add_component(circuit_circuit *circuit, circuit_component_type type, Vector2 pos) {
    circuit_component *component = malloc(sizeof(circuit_component));
    CLEAR(*component);
    circuit_graph_add_node(circuit->components, component);

    component->type = type;

    const char* names[] = { "AND", "NAND", "OR", "NOT", "IN", "OUT", "BUF" };
    component->name = names[component->type];

    switch (type) {
        case AND:
        case NAND:
        case OR:
            make_inputs(circuit, component, 2);
            break;
        case NOT:
        case OUTPUT:
        case BUFFER:
            make_inputs(circuit, component, 1);
            break;
        case INPUT:
            break;
        case CUSTOM:
            CRITICAL("Trying to create a custom component using the normal constructor");
    }

    switch (type) {
        case AND:
        case NAND:
        case OR:
        case NOT:
        case INPUT:
        case BUFFER:
            component->numOutputs = 1;
            break;
        case OUTPUT:
            component->numOutputs = 0;
            break;
        case CUSTOM:
            CRITICAL("Trying to create a custom component using the normal constructor");
    }

    component->pos = pos;

    return component;
}

circuit_component* circuit_add_custom_component(circuit_circuit *circuit, circuit_circuit* inner, Vector2 pos) {
    circuit_component *component = malloc(sizeof(circuit_component));
    CLEAR(*component);
    circuit_graph_add_node(circuit->components, component);

    component->type = CUSTOM;
    component->name = inner->name;

    ITERATE(circuit_graph_node_list, circuit->components->nodes, nodeItem) {
        circuit_component* innerComponent = nodeItem->data->data;
        if (innerComponent->type == INPUT) {
            circuit_graph_connect(circuit->components, component, NULL, NULL);
        } else if (innerComponent->type == OUTPUT) {
            component->numOutputs++;
        }
    }

    component->pos = pos;
    component->inner = inner;

    return component;
}

void circuit_connect(circuit_circuit *circuit, circuit_component *from, u32 input, circuit_component* to, u32 output) {
    circuit_graph_node* fromNode = circuit_graph_find(circuit->components, from);
    circuit_graph_node* toNode = circuit_graph_find(circuit->components, to);
    circuit_graph_edge* edge = circuit_graph_edge_list_get(fromNode->edges, input)->data;

    if (edge->data == NULL) {
        edge->data = malloc(sizeof(circuit_connection));
        CLEAR(*edge->data);
    }

    circuit_connection* connection = edge->data;

    edge->node = toNode;
    connection->output = output;
}

Vector2 get_input_position(circuit_circuit* circuit, circuit_component* component, u32 inputID) {
    Vector2 base;

    circuit_graph_node* node = circuit_graph_find(circuit->components, component);
    u32 numInputs = circuit_graph_edge_list_length(node->edges);

    if (numInputs == 1) {
        base = (Vector2) {0, 100};
    } else {
        base = (Vector2) {0, 50.0F + ((float)inputID * (100.0F / (float)(numInputs - 1)))};
    }

    if (component->type == OR) {
        base.x += 35;
    }

    return Vector2Add(base, component->pos);
}

Vector2 get_output_position(circuit_component *component, u32 outputID) {
    Vector2 base;
    if (component->numOutputs == 1) {
        base = (Vector2) {250, 100};
    } else {
        base = (Vector2) {250, 50.0F + ((float) outputID * (100.0F / (float) (component->numOutputs - 1)))};
    }

    return Vector2Add(base, component->pos);
}

circuit_library circuit_library_init() {
    circuit_library library;
    CLEAR(library);

    circuit_circuit* circuit = malloc(sizeof(circuit_circuit));
    circuit_init(circuit);
    library.circuits = circuit_circuit_list_new(circuit);
    library.current = circuit;

    return library;
}

circuit_circuit *circuit_library_create_circuit(circuit_library *library) {
    circuit_circuit* circuit = malloc(sizeof(circuit_circuit));
    circuit_init(circuit);
    circuit_circuit_list_append(library->circuits, circuit);

    return circuit;
}