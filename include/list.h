#ifndef SCC_LIST_HEADER
#define SCC_LIST_HEADER

typedef struct List {
    int size;
    void **items;
} List;

#define INITIAL_LIST_SIZE 32

List *list_new(void);

int list_length(List *list);

void list_free(List *list);

void list_append(List *list, void *item);

void list_push(List *list, void *item);

void *list_pop(List *list);

void *list_get(List *list, int index);

void list_set(List *list, int index, void *value);

#endif
