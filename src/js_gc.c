#include <js/js_gc.h>
#include <js/macros.h>

JSGC *init_js_gc() {
  JSGC *gc = NEW(JSGC);
  gc->trash = init_list(sizeof(JSGCItem *));
  return gc;
}

void js_gc_free(JSGC *gc) {
  if (gc->trash != 0) {
    js_gc_sweep(gc);
    list_free_shallow(gc->trash);
    gc->trash = 0;
  }
  free(gc);
}

void js_gc_mark(JSGC *gc, void *item, GCFreeFunc destroy) {
  if (item == 0)
    return;
  if (js_gc_is_trash(gc, item))
    return;
  JSGCItem *gcitem = NEW(JSGCItem);
  gcitem->ptr = item;
  gcitem->destroy = destroy == (void *)0 ? free : destroy;
  list_push(gc->trash, gcitem);
}

static void js_gc_ast_list(JSGC *gc, list_T *list) {
  if (gc == 0) {
    printf("ERROR (js_gc_ast_list): trying to call NULL garbage collector\n");
    return;
  }
  if (list == 0)
    return;
  if (list->size == 0)
    return;
  if (list->items == 0)
    return;

  for (size_t i = 0; i < list->size; i++) {
    JSAST *child = (JSAST *)list->items[i];
    if (child == 0)
      continue;
    //  if (child->marked != 0)
    //    continue;
    js_gc_ast(gc, child);
  }
}

void _js_gc_ast(JSGC *gc, JSAST *item) {
  if (gc == 0) {
    printf("ERROR (js_gc_ast): trying to call NULL garbage collector\n");
    exit(1);
    return;
  }
  if (item == 0)
    return;
  if (item->marked == 1)
    return;
  item->marked = 1;
  js_gc_mark(gc, item, (GCFreeFunc *)js_ast_free);
  js_gc_ast(gc, item->left);

  js_gc_ast(gc, item->right);
  js_gc_ast(gc, item->body);
  js_gc_ast(gc, item->expr);
  js_gc_ast(gc, item->accessed);
  js_gc_ast(gc, item->prototype);
  js_gc_ast(gc, item->prototype_child);
  js_gc_ast_list(gc, item->children);
  js_gc_ast_list(gc, item->args);

  if (item->keyvalue != 0) {
    list_T *values = js_ast_get_values(item);
    if (values != 0) {
      js_gc_ast_list(gc, values);
      list_free_shallow(values);
    }
  }
}

void js_gc_sweep(JSGC *gc) {
  size_t len = gc->trash->size;
  for (size_t i = 0; i < len; i++) {
    JSGCItem *item = (JSGCItem *)gc->trash->items[i];
    if (item == 0)
      continue;
    if (item->ptr == 0)
      continue;
    item->destroy(item->ptr);
    free(item);
  }

  list_clear(gc->trash);
}

unsigned int js_gc_is_trash(JSGC *gc, void *ptr) {
  size_t len = gc->trash->size;
  for (size_t i = 0; i < len; i++) {
    JSGCItem *item = (JSGCItem *)gc->trash->items[i];
    if (item->ptr == ptr)
      return 1;
  }

  return 0;
}
