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

    graph_node_list_append(graph->nodes, node);
}

void graph_connect(graph_graph* graph, void* from, void* to, void* data) {
    graph_node* fromNode = graph_node_list_find(graph->nodes, from)->data;
    graph_node* toNode = graph_node_list_find(graph->nodes, to)->data;

    graph_edge* edge = malloc(sizeof(graph_edge));
    CLEAR(*edge);

    edge->data = data;
    edge->node = toNode;

    graph_edge_list_append(fromNode->edges, edge);
}

graph_node* graph_find(graph_graph* graph, void* data) {
    return graph_node_list_find(graph->nodes, data)->data;
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