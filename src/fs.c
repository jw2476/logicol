#include "fs.h"

#include "cJSON.h"

cJSON* encode_component(circuit_circuit* circuit, circuit_component* component) {
    circuit_graph_node* node = circuit_graph_find(circuit->components, component);

    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "name", component->name);
    cJSON_AddNumberToObject(obj, "subcomponent_id", (double)circuit_graph_node_list_indexof(circuit->components->nodes, node));

    cJSON* position = cJSON_AddObjectToObject(obj, "position");
    cJSON_AddNumberToObject(position, "x", (double)component->pos.x);
    cJSON_AddNumberToObject(position, "y", (double)component->pos.y);

    cJSON* inputs = NULL;
    u32 numInput = 0;
    ITERATE(circuit_graph_edge_list, node->edges, edgeItem) {
        circuit_graph_edge* edge = edgeItem->data;
        if (edge == NULL || edge->data == NULL || edge->node == NULL) continue;

        if (inputs == NULL) {
            inputs = cJSON_AddArrayToObject(obj, "inputs");
        }

        cJSON* input = cJSON_CreateObject();
        cJSON_AddNumberToObject(input, "input_index", (double)numInput);
        cJSON_AddNumberToObject(input, "subcomponent_id", (double)circuit_graph_node_list_indexof(circuit->components->nodes, edge->node));
        cJSON_AddNumberToObject(input, "node_index", (double)edge->data->output);
        cJSON_AddNumberToObject(input, "bit_depth", 1);
        cJSON_AddItemToArray(inputs, input);

        numInput++;

    }

    return obj;
}

circuit_component* decode_component(circuit_library* library, cJSON* data) {
    u32 componentID = cJSON_GetObjectItemCaseSensitive(data, "subcomponent_id")->valueint;
    const char* name = cJSON_GetObjectItemCaseSensitive(data, "name")->valuestring;
    cJSON* position = cJSON_GetObjectItemCaseSensitive(data, "position");
    Vector2 pos = {(float)cJSON_GetObjectItemCaseSensitive(position, "x")->valuedouble, (float)cJSON_GetObjectItemCaseSensitive(position, "y")->valuedouble };
    circuit_component* component = circuit_graph_node_list_get(library->current->components->nodes, componentID)->data->data;

    if (strcmp(name, "AND") == 0 ) {
        circuit_init_component(library->current, component, AND, pos);
    } else if (strcmp(name, "OR") == 0) {
        circuit_init_component(library->current, component, OR, pos);
    } else if (strcmp(name, "NOT") == 0) {
        circuit_init_component(library->current, component, NOT, pos);
    } else if (strcmp(name, "IN") == 0) {
        circuit_init_component(library->current, component, INPUT, pos);
    } else if (strcmp(name, "OUT") == 0) {
        circuit_init_component(library->current, component, OUTPUT, pos);
    } else {
        circuit_circuit* inner = NULL;

        ITERATE(circuit_circuit_list, library->circuits, circutItem) {
            circuit_circuit* circuit = circutItem->data;
            if (strcmp(circuit->name, name) == 0) {
                inner = circuit;
                break;
            }
        }

        if (inner == NULL) {
            circuit_circuit* oldCurrentCircuit = library->current;
            inner = load_circuit(library, name);
            library->current = oldCurrentCircuit; // Restore currentCircuitID from before recursive call
        }

        circuit_init_custom_component(library->current, component, inner, pos);
    }

    return component;
}

void decode_connections(circuit_circuit* circuit, cJSON* data) {
    u32 componentID = cJSON_GetObjectItemCaseSensitive(data, "subcomponent_id")->valueint;
    circuit_graph_node* node = circuit_graph_node_list_get(circuit->components->nodes, componentID)->data;
    circuit_component* component = node->data;

    cJSON* inputs = cJSON_GetObjectItemCaseSensitive(data, "inputs");
    cJSON* input;
    cJSON_ArrayForEach(input, inputs) {
        u32 inputIndex = cJSON_GetObjectItemCaseSensitive(input, "input_index")->valueint;
        u32 outputIndex = cJSON_GetObjectItemCaseSensitive(input, "node_index")->valueint;
        u32 toID = cJSON_GetObjectItemCaseSensitive(input, "subcomponent_id")->valueint;
        circuit_connect(circuit, component, inputIndex, circuit_graph_node_list_get(circuit->components->nodes, toID)->data->data, outputIndex);
    }
}

void save_circuit(circuit_circuit* circuit, const char* path) {
    cJSON* document = cJSON_CreateObject();
    cJSON_AddStringToObject(document, "name", circuit->name);
    cJSON_AddStringToObject(document, "shortname", circuit->name); // TODO: Shortnames
    cJSON_AddStringToObject(document, "description", circuit->name); // TODO: Descriptions
    cJSON_AddStringToObject(document, "author", "AUTHOR"); // TODO: Authors
    cJSON_AddStringToObject(document, "version", "0.0.1"); // TODO: Versioning
    cJSON_AddStringToObject(document, "component_type", "DETERMINISTIC");
    cJSON_AddStringToObject(document, "hash", "HASH WILL GO HERE"); // TODO: Hashing

    cJSON* inputs = cJSON_AddArrayToObject(document, "inputs");
    ITERATE(circuit_graph_node_list, circuit->components->nodes, nodeItem) {
        circuit_graph_node* node = nodeItem->data;
        if (node == NULL) break;
        circuit_component* component = node->data;

        if (component->type == INPUT) {
            cJSON_AddItemToArray(inputs, encode_component(circuit, component));
        }
    }

    cJSON* outputs = cJSON_AddArrayToObject(document, "outputs");
    ITERATE(circuit_graph_node_list, circuit->components->nodes, nodeItem) {
        circuit_graph_node* node = nodeItem->data;
        if (node == NULL) break;
        circuit_component* component = node->data;

        if (component->type == OUTPUT) {
            cJSON_AddItemToArray(outputs, encode_component(circuit, component));
        }
    }

    cJSON* subcomponents = cJSON_AddArrayToObject(document, "subcomponents");
    ITERATE(circuit_graph_node_list, circuit->components->nodes, nodeItem) {
        circuit_graph_node* node = nodeItem->data;
        if (node == NULL) break;
        circuit_component* component = node->data;

        if (component->type != OUTPUT && component->type != INPUT) {
            cJSON_AddItemToArray(subcomponents, encode_component(circuit, component));
        }
    }

    FILE* file = fopen(path, "w");
    if (file == NULL) {
        ERROR("%s does not exist", path);
        return;
    }

    char* documentString = cJSON_Print(document);
    fprintf(file, "%s", documentString);

    fclose(file);

    INFO("Saved to %s", path);
}

circuit_circuit* load_circuit(circuit_library* library, const char* n) {
    char path[100];
    sprintf(path, "../output/%s.circuit", n);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        ERROR("Failed to open %s", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    u64 size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* string = malloc(size);
    fread(string, size, 1, file);
    fclose(file);

    cJSON* data = cJSON_Parse(string);
    free(string);

    library->current = circuit_library_create_circuit(library);

    const char* name = cJSON_GetObjectItemCaseSensitive(data, "name")->valuestring;
    memcpy(library->current->name, name, strlen(name));

    cJSON* inputs = cJSON_GetObjectItemCaseSensitive(data, "inputs");
    cJSON* outputs = cJSON_GetObjectItemCaseSensitive(data, "outputs");
    cJSON* subcomponents = cJSON_GetObjectItemCaseSensitive(data, "subcomponents");

    cJSON* input;
    cJSON* output;
    cJSON* subcomponent;

    u32 numComponents = 0;
    cJSON_ArrayForEach(input, inputs) {
        numComponents++;
    }
    cJSON_ArrayForEach(output, outputs) {
        numComponents++;
    }
    cJSON_ArrayForEach(subcomponent, subcomponents) {
        numComponents++;
    }

    // Preallocate component nodes
    for (u32 i = 0; i < numComponents; i++) {
        circuit_component* component = malloc(sizeof(circuit_component));
        CLEAR(*component);
        circuit_graph_add_node(library->current->components, component);
    }

    cJSON_ArrayForEach(input, inputs) {
        decode_component(library, input);
    }
    cJSON_ArrayForEach(output, outputs) {
        decode_component(library, output);
    }
    cJSON_ArrayForEach(subcomponent, subcomponents) {
        decode_component(library, subcomponent);
    }

    cJSON_ArrayForEach(input, inputs) {
        decode_connections(library->current, input);
    }
    cJSON_ArrayForEach(output, outputs) {
        decode_connections(library->current, output);
    }
    cJSON_ArrayForEach(subcomponent, subcomponents) {
        decode_connections(library->current, subcomponent);
    }

    INFO("Loaded: %s", name);

    return library->current;
}