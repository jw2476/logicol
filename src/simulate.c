#include "simulate.h"

bool is_true(circuit_circuit* circuit, circuit_component* component, u32 outputID, circuit_circuit* library) {

    for (u32 i = 0; i < component->numInputs; i++) {
        if (component->inputs[i].componentID == 0) {
            component->inputs[i].on = false;
            continue;
        }

        component->inputs[i].on = is_true(circuit, circuit_get_component(circuit, component->inputs[i].componentID), component->inputs[i].outputID, library);
    }

    switch (component->type) {
        case AND:
            return component->inputs[0].on && component-> inputs[1].on;
        case OR:
            return component->inputs[0].on || component->inputs[1].on;
        case NOT:
            return !component->inputs[0].on;
        case INPUT:
            return component->internallyActive;
        case OUTPUT:
            // THIS IS IMPOSSIBLE
            break;
        case CUSTOM: {
            u32 counter = 0;
            for (u32 i = 0; i < library[component->innerID].numComponents; i++) {
                if (library[component->innerID].components[i].type == INPUT) {
                    library[component->innerID].components[i].internallyActive = component->inputs[counter++].on;
                }
            }

            simulate(&library[component->innerID], library);

            counter = 0;
            for (u32 i = 0; i < library[component->innerID].numComponents; i++) {
                if (library[component->innerID].components[i].type == OUTPUT) {
                    if (counter == outputID) {
                        return library[component->innerID].components[i].internallyActive;
                    }
                    counter++;
                }
            }

            break;
        }
    }
}

void test_output(circuit_circuit* circuit, circuit_component* component, circuit_circuit* library) {
    if (component->inputs[0].componentID == 0) return;

    component->internallyActive = is_true(circuit, circuit_get_component(circuit, component->inputs[0].componentID), component->inputs[0].outputID, library);
    component->inputs[0].on = component->internallyActive;
}

void simulate(circuit_circuit* circuit, circuit_circuit* library) {
    for (u64 i = 0; i < circuit->numComponents; i++) {
        if (circuit->components[i].type == OUTPUT) {
            test_output(circuit, &circuit->components[i], library);
        }
    }
}