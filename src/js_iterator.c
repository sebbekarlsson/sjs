#include <js/js_iterator.h>
#include <js/macros.h>

JSIterator js_iterator(void **items, size_t *length) {
  return (JSIterator){items[0], 0, items, length};
}

JSIterator *init_js_iterator(void **items, size_t *length) {
  JSIterator *it = NEW(JSIterator);
  it->items = items;
  it->length = length;
  return it;
}

void *js_iterator_next(JSIterator *iterator) {
  if (iterator->index >= (*iterator->length))
    return 0;
  iterator->index++;
  iterator->current = iterator->items[iterator->index];
  return iterator->current;
}
