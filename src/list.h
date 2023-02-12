#pragma once

#include "base.h"

typedef struct list_t list;
typedef void(*list_traversal_func)(void*);

typedef struct list_t {
    void* data;
    list* next;
} list;

list* list_new(void* data);
list* list_get(list* root, u32 index);
list* list_find(list* root, void* data);
list* list_append(list* root, void* data);
list* list_insert(list* root, u32 index, void* data);
void list_remove(list* root, u32 index);
void list_delete(list* root, void* data);
u32 list_indexof(list* root, void* data);
void list_traverse(list* root, list_traversal_func callback);

#define DEFINE_LIST(T, NAME)                                                           \
typedef struct NAME ## _t NAME;                                                        \
typedef struct NAME ## _t {                                                            \
    T* data;                                                                           \
    NAME* next;                                                                        \
} NAME;                                                                                \
                                                                                       \
static NAME*(*NAME ## _new)(T*) = (void*)list_new;                                     \
static NAME*(*NAME ## _get)(NAME*, u32) = (void*)list_get;                             \
static NAME*(*NAME ## _find)(NAME*, T*) = (void*)list_find;                            \
static NAME*(*NAME ## _append)(NAME*, T*) = (void*)list_append;                        \
static NAME*(*NAME ## _insert)(NAME*, u32, T*) = (void*)list_insert;                   \
static void(*NAME ## _remove)(NAME*, u32) = (void*)list_remove;                        \
static void(*NAME ## _delete)(NAME*, T*) = (void*)list_delete;                         \
static u32(*NAME ## _indexof)(NAME*, T*) = (void*)list_indexof;                        \
static void(*NAME ## _traverse)(NAME*, list_traversal_func) = (void*)list_traverse
