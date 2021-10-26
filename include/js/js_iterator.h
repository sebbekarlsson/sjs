#ifndef JS_ENGINE_ITERATOR_H
#define JS_ENGINE_ITERATOR_H
#include <stddef.h>
#include <stdint.h>
typedef struct JS_ITERATOR_STRUCT {
  void *current;
  uint32_t index;
  void **items;
  size_t *length;
} JSIterator;

JSIterator js_iterator(void **items, size_t *length);
JSIterator *init_js_iterator(void **items, size_t *length);
void *js_iterator_next(JSIterator *iterator);
#endif
