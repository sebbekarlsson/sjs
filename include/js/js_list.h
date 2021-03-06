#ifndef JS_ENGINE_LIST_H
#define JS_ENGINE_LIST_H
#include <stdlib.h>

#define LOOP(list, C, NAME, WHAT)                                              \
  if (list && list->items && list->size) {                                     \
    for (unsigned int C = 0; C < list->size; C++) {                            \
      void *NAME = (void *)list->items[C];                                     \
      WHAT;                                                                    \
    }                                                                          \
  }

typedef struct LIST_STRUCT {
  void **items;
  size_t size;
  size_t item_size;
} list_T;

list_T *init_list(size_t item_size);

void list_push(list_T *list, void *item);

void list_push_at(list_T *list, void *item, void *ptr);

void list_push_safe(list_T *list, void *item);

void list_push_safe_at(list_T *list, void *item, void *ptr);

void list_shift_left(list_T *list, int index);

void list_shift_right(list_T *list, int index);

void list_remove(list_T *list, void *element, void (*free_method)(void *item));

void list_prefix(list_T *list, void *item);

unsigned int ptr_in_list(list_T *list, void *ptr);

int list_indexof_str(list_T *list, char *item);

int list_indexof_ptr(list_T *list, void *ptr);

unsigned int list_contains_str(list_T *list, char *item);

list_T *list_filter(list_T *list, unsigned int (*filter_method)(void *item));

list_T *list_merge(list_T *a, list_T *b);

list_T *list_copy(list_T *a);

void *list_pop(list_T *list);

void list_free_full(list_T *list, void (*free_method)(void *item));

char *list_join(list_T *list, const char *delim);

typedef char *(*ListMapStrFunc)(void *item, void *arg1, void *arg2, void *arg3);

char *list_map_str(list_T *list, ListMapStrFunc func, void *arg1, void *arg2,
                   void *arg3);

void list_clear(list_T *list);

void list_free_shallow(list_T *list);

void list_concat(list_T *list1, list_T *list2);

#endif
