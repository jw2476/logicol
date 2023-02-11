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