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

circuit_component* decode_component(circuit_circuit* circuit, cJSON* data) {
    const char* name = cJSON_GetObjectItemCaseSensitive(data, "name")->valuestring;
    cJSON* position = cJSON_GetObjectItemCaseSensitive(data, "position");
    Vector2 pos = {(float)cJSON_GetObjectItemCaseSensitive(position, "x")->valuedouble, (float)cJSON_GetObjectItemCaseSensitive(position, "y")->valuedouble };
    circuit_component* component;

    if (strcmp(name, "AND") == 0 ) {
        component = circuit_add_component(circuit, AND, pos);
    } else if (strcmp(name, "OR") == 0) {
        component = circuit_add_component(circuit, OR, pos);
    } else if (strcmp(name, "NOT") == 0) {
        component = circuit_add_component(circuit, NOT, pos);
    } else if (strcmp(name, "IN") == 0) {
        component = circuit_add_component(circuit, INPUT, pos);
    } else if (strcmp(name, "OUT") == 0) {
        component = circuit_add_component(circuit, OUTPUT, pos);
    } else {
        // TODO: Loading Custom Component
        CRITICAL("TODO: Loading Custom Components");
    }

    component->id = cJSON_GetObjectItemCaseSensitive(data, "subcomponent_id")->valueint;

    return component;
}

void decode_connections(circuit_circuit* circuit, cJSON* data) {
    circuit_component* component = circuit_get_component(circuit, cJSON_GetObjectItemCaseSensitive(data, "subcomponent_id")->valueint);

    cJSON* inputs = cJSON_GetObjectItemCaseSensitive(data, "inputs");
    cJSON* input;
    cJSON_ArrayForEach(input, inputs) {
        u32 inputIndex = cJSON_GetObjectItemCaseSensitive(input, "input_index")->valueint;
        circuit_component* to = circuit_get_component(circuit, cJSON_GetObjectItemCaseSensitive(input, "subcomponent_id")->valueint);
        u32 outputIndex = cJSON_GetObjectItemCaseSensitive(input, "node_index")->valueint;
        circuit_connect(circuit, component, inputIndex, to, outputIndex);
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
    printf("%s\n", documentString);

    fclose(file);

    INFO("Saved to %s", path);
}

void load_circuit(circuit_circuit* circuit, const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        ERROR("Failed to open %s", path);
        return;
    }

    fseek(file, 0, SEEK_END);
    u64 size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* string = malloc(size);
    fread(string, size, 1, file);
    fclose(file);

    cJSON* data = cJSON_Parse(string);
    free(string);

    const char* name = cJSON_GetObjectItemCaseSensitive(data, "name")->valuestring;
    memcpy(circuit->name, name, strlen(name));

    cJSON* inputs = cJSON_GetObjectItemCaseSensitive(data, "inputs");
    cJSON* outputs = cJSON_GetObjectItemCaseSensitive(data, "outputs");
    cJSON* subcomponents = cJSON_GetObjectItemCaseSensitive(data, "subcomponents");

    cJSON* input;
    cJSON* output;
    cJSON* subcomponent;

    cJSON_ArrayForEach(input, inputs) {
        decode_component(circuit, input);
    }
    cJSON_ArrayForEach(output, outputs) {
        decode_component(circuit, output);
    }
    cJSON_ArrayForEach(subcomponent, subcomponents) {
        decode_component(circuit, subcomponent);
    }

    cJSON_ArrayForEach(input, inputs) {
        decode_connections(circuit, input);
    }
    cJSON_ArrayForEach(output, outputs) {
        decode_connections(circuit, output);
    }
    cJSON_ArrayForEach(subcomponent, subcomponents) {
        decode_connections(circuit, subcomponent);
    }
}