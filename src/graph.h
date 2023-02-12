#pragma once

#include "base.h"
#include "list.h"

typedef struct graph_graph_t graph_graph;
typedef struct graph_node_t graph_node;
typedef struct graph_edge_t graph_edge;

DEFINE_LIST(graph_node, graph_node_list);
DEFINE_LIST(graph_edge, graph_edge_list);

typedef struct graph_graph_t {
    graph_node_list* nodes;
} graph_graph;

typedef struct graph_node_t {
    void* data;
    graph_edge_list* edges;
} graph_node;

typedef struct graph_edge_t {
    void* data;
    graph_node* node;
} graph_edge;

graph_graph* graph_new();
void graph_add_node(graph_graph* graph, void* data);
void graph_connect(graph_graph* graph, void* from, void* to, void* data);
graph_node* graph_find(graph_graph* graph, void* data);
void graph_delete(graph_graph* graph, void* data);

#define DEFINE_GRAPH(N, E, NAME)                                                    \
typedef struct NAME ## _graph_t NAME ## _graph;                                     \
typedef struct NAME ## _node_t NAME ## _node;                                       \
typedef struct NAME ## _edge_t NAME ## _edge;                                       \
                                                                                    \
DEFINE_LIST(NAME ## _node, NAME ## _node_list);                                     \
DEFINE_LIST(NAME ## _edge, NAME ## _edge_list);                                     \
                                                                                    \
typedef struct NAME ## _graph_t {                                                   \
    NAME ## _node_list* nodes;                                                      \
} NAME ## _graph;                                                                   \
                                                                                    \
typedef struct NAME ## _node_t {                                                    \
    N* data;                                                                        \
    NAME ## _edge_list* edges;                                                      \
} NAME ## _node;                                                                    \
                                                                                    \
typedef struct NAME ## _edge_t {                                                    \
    E* data;                                                                        \
    NAME ## _node* node;                                                            \
} NAME ## _edge;                                                                    \
                                                                                    \
static NAME ## _graph*(*NAME ## _new)() = (void*)graph_new;                         \
static void(*NAME ## _add_node)(NAME ## _graph*, N*) = (void*)graph_add_node;       \
static void(*NAME ## _connect)(NAME ## _graph*, N*, N*, E*) = (void*)graph_connect; \
static NAME ## _node*(*NAME ## _find)(NAME ## _graph*, N*) = (void*)graph_find;     \
static void(*NAME ## _delete)(NAME ## _graph*, N*) = (void*)graph_delete;