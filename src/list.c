#include "list.h"

list* list_new(void* data) {
    list* output = malloc(sizeof(list));
    output->data = data;
    output->next = NULL;

    return output;
}

list* list_append(list* root, void* data) {
    list* item = root;
    while(item->next != NULL) {
        item = item->next;
    }

    list* output = list_new(data);
    item->next = output;

    return output;
}

list* list_insert(list* root, u32 index, void* data) {
    if (index == 0) {
        CRITICAL("Linked list implementation doesn't support changing the root node.");
    }

    list* item = root;
    for (u32 i = 0; i < (index - 1); i++) {
        item = item->next;
    }

    list* output = list_new(data);
    output->next = item->next;
    item->next = output;

    return output;
}

void list_remove(list* root, u32 index, void* data) {
    if (index == 0) {
        CRITICAL("Linked list implementation doesn't support changing the root node.");
    }

    list* item = root;
    for (u32 i = 0; i < (index - 1); i++) {
        item = item->next;
    }

    list* toRemove = item->next;
    item->next = item->next->next;
    free(toRemove);
}

void list_traverse(list* root, list_traversal_func callback) {
    list* item = root;
    while (item != NULL) {
        callback(item);
        item = item->next;
    }
}