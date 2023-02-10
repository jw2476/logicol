#include "fs.h"

#include "cJSON.h"

cJSON* encode_component(circuit_circuit* circuit, circuit_component* component) {
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "name", component->name);
    cJSON_AddNumberToObject(obj, "subcomponent_id", (double)component->id);

    cJSON* position = cJSON_AddObjectToObject(obj, "position");
    cJSON_AddNumberToObject(position, "x", (double)component->pos.x);
    cJSON_AddNumberToObject(position, "y", (double)component->pos.y);

    cJSON* inputs = NULL;
    for (u32 i = 0; i < component->numInputs; i++) {
        if (component->inputs[i].componentID != 0) {
            if (inputs == NULL) {
                inputs = cJSON_AddArrayToObject(obj, "inputs");
            }

            cJSON* input = cJSON_CreateObject();
            cJSON_AddNumberToObject(input, "input_index", (double)i);
            cJSON_AddNumberToObject(input, "subcomponent_id", (double)component->inputs[i].componentID);
            cJSON_AddNumberToObject(input, "node_index", (double)component->inputs[i].outputID);
            cJSON_AddNumberToObject(input, "bit_depth", 1);
            cJSON_AddItemToArray(inputs, input);
        }
    }

    return obj;
}

circuit_component* decode_component(circuit_library* library, cJSON* data) {
    const char* name = cJSON_GetObjectItemCaseSensitive(data, "name")->valuestring;
    cJSON* position = cJSON_GetObjectItemCaseSensitive(data, "position");
    Vector2 pos = {(float)cJSON_GetObjectItemCaseSensitive(position, "x")->valuedouble, (float)cJSON_GetObjectItemCaseSensitive(position, "y")->valuedouble };
    circuit_component* component;

    if (strcmp(name, "AND") == 0 ) {
        component = circuit_add_component(get_current_circuit(library), AND, pos);
    } else if (strcmp(name, "OR") == 0) {
        component = circuit_add_component(get_current_circuit(library), OR, pos);
    } else if (strcmp(name, "NOT") == 0) {
        component = circuit_add_component(get_current_circuit(library), NOT, pos);
    } else if (strcmp(name, "IN") == 0) {
        component = circuit_add_component(get_current_circuit(library), INPUT, pos);
    } else if (strcmp(name, "OUT") == 0) {
        component = circuit_add_component(get_current_circuit(library), OUTPUT, pos);
    } else {
        i64 circuitID = -1;


        for (u32 i = 0; i < library->numCircuits; i++) {
            if (strcmp(library->circuits[i].name, name) == 0) {
                circuitID = i;
            }
        }

        if (circuitID == -1) {
            u32 oldCurrentCircuitID = library->currentCircuitID;
            circuitID = (u32)load_circuit(library, name);
            library->currentCircuitID = oldCurrentCircuitID; // Restore currentCircuitID from before recursive call
        }

        component = circuit_add_custom_component(get_current_circuit(library), circuitID, library, pos);
    }

    component->id = cJSON_GetObjectItemCaseSensitive(data, "subcomponent_id")->valueint;

    return component;
}

void decode_connections(circuit_circuit* circuit, cJSON* data) {
    u32 componentID = cJSON_GetObjectItemCaseSensitive(data, "subcomponent_id")->valueint;
    circuit_component* component = circuit_get_component(circuit, componentID);

    cJSON* inputs = cJSON_GetObjectItemCaseSensitive(data, "inputs");
    cJSON* input;
    cJSON_ArrayForEach(input, inputs) {
        u32 inputIndex = cJSON_GetObjectItemCaseSensitive(input, "input_index")->valueint;
        u32 outputIndex = cJSON_GetObjectItemCaseSensitive(input, "node_index")->valueint;
        circuit_connect(circuit, component, inputIndex, cJSON_GetObjectItemCaseSensitive(input, "subcomponent_id")->valueint, outputIndex);
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
    for (u64 i = 0; i < circuit->numComponents; i++) {
        if (circuit->components[i].type == INPUT) {
            cJSON_AddItemToArray(inputs, encode_component(circuit, &circuit->components[i]));
        }
    }

    cJSON* outputs = cJSON_AddArrayToObject(document, "outputs");
    for (u64 i = 0; i < circuit->numComponents; i++) {
        if (circuit->components[i].type == OUTPUT) {
            cJSON_AddItemToArray(outputs, encode_component(circuit, &circuit->components[i]));
        }
    }

    cJSON* subcomponents = cJSON_AddArrayToObject(document, "subcomponents");
    for (u64 i = 0; i < circuit->numComponents; i++) {
        if (circuit->components[i].type != OUTPUT && circuit->components[i].type != INPUT) {
            cJSON_AddItemToArray(subcomponents, encode_component(circuit, &circuit->components[i]));
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

i64 load_circuit(circuit_library* library, const char* n) {
    char path[100];
    sprintf(path, "../output/%s.circuit", n);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        ERROR("Failed to open %s", path);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    u64 size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* string = malloc(size);
    fread(string, size, 1, file);
    fclose(file);

    cJSON* data = cJSON_Parse(string);
    free(string);

    circuit_library_create_circuit(library);
    u32 circuitID = library->numCircuits - 1;
    library->currentCircuitID = circuitID;

    const char* name = cJSON_GetObjectItemCaseSensitive(data, "name")->valuestring;
    memcpy(get_current_circuit(library)->name, name, strlen(name));

    cJSON* inputs = cJSON_GetObjectItemCaseSensitive(data, "inputs");
    cJSON* outputs = cJSON_GetObjectItemCaseSensitive(data, "outputs");
    cJSON* subcomponents = cJSON_GetObjectItemCaseSensitive(data, "subcomponents");

    cJSON* input;
    cJSON* output;
    cJSON* subcomponent;

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
        decode_connections(get_current_circuit(library), input);
    }
    cJSON_ArrayForEach(output, outputs) {
        decode_connections(get_current_circuit(library), output);
    }
    cJSON_ArrayForEach(subcomponent, subcomponents) {
        decode_connections(get_current_circuit(library), subcomponent);
    }

    INFO("Loaded: %s", name);

    return circuitID;
}