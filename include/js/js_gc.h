#ifndef JS_ENGINE_GC_H
#define JS_ENGINE_GC_H
#include <js/js_AST.h>
#include <js/js_list.h>

typedef void(GCFreeFunc(void *__ptr));

typedef struct JS_GC_ITEM_STRUCT {
  void *ptr;
  GCFreeFunc *destroy;
} JSGCItem;

typedef struct JS_GC_STRUCT {
  list_T *trash;
} JSGC;

JSGC *init_js_gc();

void js_gc_free(JSGC *gc);

void js_gc_mark(JSGC *gc, void *item, GCFreeFunc destroy);

void js_gc_ast(JSGC *gc, JSAST *item);

void js_gc_sweep(JSGC *gc);

#endif
