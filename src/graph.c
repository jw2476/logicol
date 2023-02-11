#include "graph.h"

graph_graph* graph_init() {
    graph_graph* graph = malloc(sizeof(graph_graph));
    CLEAR(*graph);

    graph->nodes = list_new(NULL);

    return graph;
}

void graph_add_node(graph_graph* graph, void* data) {
    graph_node* node = malloc(sizeof(graph_node));
    CLEAR(*node);

    node->data = data;
    node->connected = list_new(NULL);

    list_append(graph->nodes, node);
}

void graph_connect(graph_graph* graph, void* from, void* to) {
    graph_node* fromNode = (graph_node*)list_find(graph->nodes, from)->data;
    graph_node* toNode = (graph_node*)list_find(graph->nodes, to)->data;

    list_append(fromNode->connected, toNode);
}

graph_node* graph_find(graph_graph* graph, void* data) {
    return (graph_node*)list_find(graph->nodes, data)->data;
}

void graph_delete(graph_graph* graph, void* data) {
    graph_node* node = graph_find(graph, data);
    list_delete(graph->nodes, node);

    for (list* nodeItem = graph->nodes; nodeItem != NULL; nodeItem = nodeItem->next) {
        for (list* connectedItem = ((graph_node*)nodeItem->data)->connected; connectedItem != NULL; connectedItem = connectedItem->next) {
            if (connectedItem->data == node) {
                list_delete(((graph_node*)nodeItem->data)->connected, node);
            }
        }
    }
}