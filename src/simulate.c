#include "simulate.h"

bool is_true(circuit_circuit* circuit, circuit_graph_node* node, u32 numOutput) {
    circuit_component_type type = node->data->type;

    switch (type) {
        case AND:
            return circuit_graph_edge_list_get(node->edges, 0)->data->data->on && circuit_graph_edge_list_get(node->edges, 1)->data->data->on;
        case NAND:
            return !(circuit_graph_edge_list_get(node->edges, 0)->data->data->on && circuit_graph_edge_list_get(node->edges, 1)->data->data->on);
        case OR:
            return circuit_graph_edge_list_get(node->edges, 0)->data->data->on || circuit_graph_edge_list_get(node->edges, 1)->data->data->on;
        case NOT:
            return !circuit_graph_edge_list_get(node->edges, 0)->data->data->on;
        case INPUT:
            return node->data->internallyActive;
        case OUTPUT:
            CRITICAL("Output nodes have no output wires, so is_true should never be called");
        case BUFFER:
            return node->data->internallyActive;
        case CUSTOM:
            CRITICAL("TODO: Custom component simulation");
    }
}

void set_output(circuit_circuit* circuit, circuit_graph_node* node, u32 output, bool value) {
    ITERATE(circuit_graph_node_list, circuit->components->nodes, nodeItem) {
        circuit_graph_node* n = nodeItem->data;
        ITERATE(circuit_graph_edge_list, n->edges, edgeItem) {
            circuit_graph_edge *edge = edgeItem->data;
            if (edge == NULL || edge->node == NULL) continue;

            if (edge->node == node && edge->data->output == output) {
                edge->data->on = value;
            }
        }
    }
}

void simulate(circuit_circuit* circuit) {
    circuit_graph_node_list* sorted = circuit_graph_topological_sort(circuit->components);
    
    ITERATE(circuit_graph_node_list, sorted, sortedItem) {
        if (sortedItem->data == NULL) break;

        for (u32 i = 0; i < sorted->data->data->numOutputs; i++) {
            if (sortedItem->data->data->type == OUTPUT) {
                sortedItem->data->data->internallyActive = sortedItem->data->edges->data->data->on;
                continue;
            }

            bool result = is_true(circuit, sortedItem->data, i);
            set_output(circuit, sortedItem->data, i, result);
        }
    }

    ITERATE(circuit_graph_node_list, circuit->components->nodes, nodeItem) {
        if (nodeItem->data == NULL) break;
        circuit_component* component = nodeItem->data->data;
        if (component->type == BUFFER) {
            component->internallyActive = nodeItem->data->edges->data->data->on;
        }
    }

    while (sorted != NULL) {
        circuit_graph_node_list* sortedNext = sorted->next;
        free(sorted);
        sorted = sortedNext;
    }
}