#include "graph.h"

graph_graph* graph_new() {
    graph_graph* graph = malloc(sizeof(graph_graph));
    CLEAR(*graph);

    graph->nodes = graph_node_list_new(NULL);

    return graph;
}

void graph_add_node(graph_graph* graph, void* data) {
    graph_node* node = malloc(sizeof(graph_node));
    CLEAR(*node);

    node->data = data;
    node->edges = graph_edge_list_new(NULL);
    node->marked = false;

    graph_node_list_append(graph->nodes, node);
}

void graph_connect(graph_graph* graph, void* from, void* to, void* data) {
    graph_node* fromNode = graph_find(graph, from);
    graph_node* toNode;

    if (to != NULL) {
        toNode = graph_find(graph, to);
    } else {
        toNode = NULL;
    }

    graph_edge* edge = malloc(sizeof(graph_edge));
    CLEAR(*edge);

    edge->data = data;
    edge->node = toNode;

    graph_edge_list_append(fromNode->edges, edge);
}

graph_node* graph_find(graph_graph* graph, void* data) {
    for (graph_node_list* nodeItem = graph->nodes; nodeItem != NULL; nodeItem = nodeItem->next) {
        if (nodeItem->data->data == data) {
            return nodeItem->data;
        }
    }

    return NULL;
}

void graph_delete(graph_graph* graph, void* data) {
    graph_node* node = graph_find(graph, data);
    graph_node_list_delete(graph->nodes, node);

    for (graph_node_list* nodeItem = graph->nodes; nodeItem != NULL; nodeItem = nodeItem->next) {
        for (graph_edge_list* edgeItem = ((graph_node*)nodeItem->data)->edges; edgeItem != NULL; edgeItem = edgeItem->next) {
            if (edgeItem->data->node == node) {
                graph_edge_list_delete(nodeItem->data->edges, edgeItem->data);
            }
        }
    }
}

void graph_topological_sort_visit(graph_graph* graph, graph_node_list* sorted, graph_node* node) {
    if (node->marked) return;

    ITERATE(graph_edge_list, node->edges, edgeItem) {
        if (edgeItem->data == NULL || edgeItem->data->node == NULL) continue;
        graph_topological_sort_visit(graph, sorted, edgeItem->data->node);
    }

    node->marked = true;
    graph_node_list_append(sorted, node);
}

graph_node_list* graph_topological_sort(graph_graph* graph) {
    if (graph->nodes->data == NULL)  {
        return graph_node_list_new(NULL);
    }

    ITERATE(graph_node_list, graph->nodes, nodeItem) {
        nodeItem->data->marked = false;
    }

    graph_node_list* sorted = graph_node_list_new(NULL);

    ITERATE(graph_node_list, graph->nodes, nodeItem) {
        if (!nodeItem->data->marked) {
            graph_topological_sort_visit(graph, sorted, nodeItem->data);
        }
    }

    return sorted;
}