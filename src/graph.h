#pragma once

#include "base.h"
#include "list.h"

typedef struct graph_graph_t {
    list* nodes;
} graph_graph;

typedef struct graph_node_t {
    void* data;
    list* connected;
} graph_node;

graph_graph* graph_init();
void graph_add_node(graph_graph* graph, void* data);
void graph_connect(graph_graph* graph, void* from, void* to);
graph_node* graph_find(graph_graph* graph, void* data);
void graph_delete(graph_graph* graph, void* data);