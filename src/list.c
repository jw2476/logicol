#include "list.h"

list* list_new(void* data) {
    list* output = malloc(sizeof(list));
    output->data = data;
    output->next = NULL;

    return output;
}

list* list_get(list* root, u32 index) {
    list* item = root;
    for (u32 i = 0; i < index; i++) {
        item = item->next;
    }

    return item;
}

list* list_find(list* root, void* data) {
    for (list* item = root; item != NULL; item = item->next) {
        if (item->data == data) {
            return item;
        }
    }

    return NULL;
}

list* list_append(list* root, void* data) {
    if (root->data == NULL) {
        root->data = data;
        return root;
    }

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

    list* item = list_get(root, index);

    list* output = list_new(data);
    output->next = item->next;
    item->next = output;

    return output;
}

void list_remove(list* root, u32 index) {
    if (index == 0) {
        CRITICAL("Linked list implementation doesn't support changing the root node.");
    }

    list* item = list_get(root, index);

    list* toRemove = item->next;
    item->next = item->next->next;

    free(toRemove->data);
    free(toRemove);
}

void list_delete(list* root, void* data) {
    list_remove(root, list_indexof(root, data));
}

u32 list_indexof(list* root, void* data) {
    list* item = root;
    u32 index = 0;
    while (item != NULL && item->data != data) {
        index++;
        item = item->next;
    }

    return index;
}

void list_traverse(list* root, list_traversal_func callback) {
    list* item = root;
    while (item != NULL) {
        callback(item);
        item = item->next;
    }
}

u32 list_length(list* root) {
    u32 length = 0;
    list* item = root;
    while (item != NULL) {
        length++;
        item = item->next;
    }

    return length;
}